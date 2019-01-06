/*
 * Cpu8080.cpp
 */

#include "Cpu8080.h"
#include <cinttypes>

Cpu8080::Cpu8080() {
    m_pc = 0;
    m_sp = 2; // stack pointer is used by accessing the two indices below it

    m_regA = 0; m_regPSW = 0;
    m_regB = 0; m_regC = 0;
    m_regD = 0; m_regE = 0;
    m_regH = 0; m_regL = 0;

    m_flagS = 0;
    m_flagZ = 0;
    m_flagP = 0;
    m_flagC = 0;
    m_flagAC = 0;

    m_interrupts = 0;

    m_port1 = 0;

    m_regShift = 0;
    m_port3_res = 0;

    /* Load the space invaders rom into memory */
    memcpy(&m_memory[ROM_H_START], ROM_H, ROM_H_SIZE);
    memcpy(&m_memory[ROM_G_START], ROM_G, ROM_G_SIZE);
    memcpy(&m_memory[ROM_F_START], ROM_F, ROM_F_SIZE);
    memcpy(&m_memory[ROM_E_START], ROM_E, ROM_E_SIZE);
}

Cpu8080::~Cpu8080() {

}

void Cpu8080::run_next_op(void)
{
    uint8_t opcode = m_memory[m_pc];
    int num_increment = 0; // how much to increase the PC by at the bottom

    // debug - show which op is going to be ran
    //std::string op;

    switch (opcode)
    {
        case 0x00:    num_increment = 1;           break; // NOP
        case 0x01:    num_increment = op_lxi_b();  break; // LXI B, D16
        case 0x05:    num_increment = op_dcr_b();  break; // DCR B
        case 0x06:    num_increment = op_mvi_b();  break; // MVI B, D8
        case 0x09:    num_increment = op_dad_b();  break; // DAD B
        case 0x0D:    num_increment = op_dcr_c();  break; // DCR C
        case 0x0E:    num_increment = op_mvi_c();  break; // MVI C, D8
        case 0x0F:    num_increment = op_rrc_a();  break; // RRC A
        case 0x11:    num_increment = op_lxi_d();  break; // LXI D, D16
        case 0x13:    num_increment = op_inx_d();  break; // INX D
        case 0x19:    num_increment = op_dad_d();  break; // DAD D
        case 0x1A:    num_increment = op_ldax_d(); break; // LDAX D
        case 0x21:    num_increment = op_lxi_h();  break; // LXI H, D16
        case 0x23:    num_increment = op_inx_h();  break; // INX H
        case 0x26:    num_increment = op_mvi_h();  break; // MVI H, D8
        case 0x29:    num_increment = op_dad_h();  break; // DAD H

        case 0x31:    num_increment = op_lxi_sp(); break; // LXI SP, D16
        case 0x32:    num_increment = op_sta();    break; // STA ADR
        case 0x36:    num_increment = op_mvi_m();  break; // MVI M, D8
        case 0x3A:    num_increment = op_lda();    break; // LDA ADR
        case 0x3E:    num_increment = op_mvi_a();  break; // MVI A, D8

        case 0x56:    num_increment = op_mov_dm(); break; // MOV D, M
        case 0x5E:    num_increment = op_mov_em(); break; // MOV E, M

        case 0x66:    num_increment = op_mov_hm(); break; // MOV H, M
        case 0x6F:    num_increment = op_mov_la(); break; // MOV L, A
        case 0x7A:    num_increment = op_mov_ad(); break; // MOV A, D
        case 0x77:    num_increment = op_mov_ma(); break; // MOV M, A
        case 0x7B:    num_increment = op_mov_ae(); break; // MOV A, E
        case 0x7C:    num_increment = op_mov_ha(); break; // MOV H, A
        case 0x7E:    num_increment = op_mov_am(); break; // MOV A, M

        case 0xA7:    num_increment = op_ana_a();  break; // ANA A
        case 0xAF:    num_increment = op_xra_a();  break; // XRA A

        case 0xC1:    num_increment = op_pop_b();  break; // POP B
        case 0xC2:    num_increment = op_jnz();    break; // JNZ
        case 0xC3:    num_increment = op_jmp();    break; // JMP
        case 0xC5:    num_increment = op_push_b(); break; // PUSH B
        case 0xC6:    num_increment = op_adi();    break; // ADI D8
        case 0xC9:    num_increment = op_ret();    break; // RET
        case 0xCD:    num_increment = op_call();   break; // CALL

        case 0xD1:    num_increment = op_pop_d();  break; // POP D
        case 0xD3:    num_increment = op_out();    break; // OUT D8
        case 0xD5:    num_increment = op_push_d(); break; // PUSH D

        case 0xE1:    num_increment = op_pop_h();  break; // POP  H
        case 0xE5:    num_increment = op_push_h(); break; // PUSH H
        case 0xE6:    num_increment = op_ani();    break; // ANI D8

        case 0xEB:    num_increment = op_xchg();   break; // XCHG

        case 0xF1:    num_increment = op_pop_psw();  break; // POP PSW
        case 0xFB:    num_increment = op_ei();       break; // EI
        case 0xFE:    num_increment = op_cpi();      break; // CPI
        case 0xF5:    num_increment = op_push_psw(); break; // PUSH PSW

        /* Unhandled opcode, exit */
        default:    op_unimplemented(); break;
    }

    m_pc += num_increment;

    printf("PC: 0x%04X\n", m_pc);

    return;
}

//////////////////////////////////////////////////////////////
//******************* Helper Functions *********************//
//////////////////////////////////////////////////////////////

uint16_t Cpu8080::read16(uint16_t addr)
{
    uint8_t byte1 = m_memory[addr];
    uint8_t byte2 = m_memory[addr+1];

    /* Reverse order since little endian */
    return (byte2 << 8) | byte1;
}

/* Returns 1 if the number of 1s is odd
 * from https://www.microchip.com/forums/m587239.aspx */
uint8_t Cpu8080::get_odd_parity(uint8_t num)
{
    num ^= (num >> 4);
    num ^= (num >> 2);
    num ^= (num >> 1);

    return num & 1;
}

/* Sets the Zero, Sign, and Parity flags based on the given result */
void Cpu8080::set_zsp_flags(uint8_t prev, uint8_t res)
{
    m_flagZ = (res == 0);
    m_flagS = (res > prev);        // if we just subtracted but the new number is LARGER, then we had underflow
    m_flagP = !get_odd_parity(res);
}

void Cpu8080::set_c_16(uint16_t num1, uint16_t num2)
{
    m_flagC = ((0xFFFF - num1) < num2);
}

void Cpu8080::set_flags_logical(uint8_t res)
{
    m_flagZ = (res == 0);
    m_flagS = ((res & 0x80) == 0x80);
    m_flagC = 0;
    m_flagP = !get_odd_parity(res);
}


/*
 * Interrupt
 */
void Cpu8080::run_interrupt(int num)
{
    /* Run 'PUSH PC' */
    m_memory[m_sp - 1] = (m_pc >> 8) & 0xFF;
    m_memory[m_sp - 2] = m_pc & 0xFF;

    m_sp -= 2;

    /* Set the PC to the low memory vector 
     * This is identical to an "RST num" instruction
     */
    m_pc = num << 3; // num * 8
}

//////////////////////////////////////////////////////////////
//                      Operations                          //
//                                                          //
//    Operations should return the number of bytes used by  //
//    that instruction, which is used to increment the      //
//      program counter                                     //
//                                                          //
//////////////////////////////////////////////////////////////

int Cpu8080::op_unimplemented(void)
{
    printf("  ** UNIMPLEMENTED OP **\n");
    printf("  Op: 0x%02X\tPC: 0x%08X\n", m_memory[m_pc], m_pc);
    exit(0);
}

int Cpu8080::op_call(void)
{
    /* Save the NEXT instruction address */
    m_memory[m_sp-2] = (m_pc+3) & 0xFF;          // save in reverse order - LOW bits first...
    m_memory[m_sp-1] = ((m_pc+3) >> 8) & 0xFF;   //                         then high...

    /* Increase the stack pointer */
    m_sp -= 2;

    /* Move to the called address*/
    m_pc = read16(m_pc+1);

    /* Don't increment PC afterwords */
    return 0;
}

int Cpu8080::op_push_psw(void)
{
    m_memory[m_sp - 1] = m_regA;

    /* Flags are technically supposed to be stored in reg PSW,
     * but we've stored them seperately so we need to 
     * compensate
     */
    m_memory[m_sp - 2] = m_flagZ       |
                         m_flagS  << 1 | 
                         m_flagP  << 2 | 
                         m_flagC  << 3 |
                         m_flagAC << 4;

    m_sp -= 2;

    return 1;
}

int Cpu8080::op_push_b(void)
{
    m_memory[m_sp - 1] = m_regB;
    m_memory[m_sp - 2] = m_regC;

    m_sp -= 2;

    return 1;
}

int Cpu8080::op_push_d(void)
{
    m_memory[m_sp - 1] = m_regD;
    m_memory[m_sp - 2] = m_regE;

    m_sp -= 2;

    return 1;
}

int Cpu8080::op_push_h(void)
{
    m_memory[m_sp - 1] = m_regH;
    m_memory[m_sp - 2] = m_regL;

    m_sp -= 2;

    return 1;
}

int Cpu8080::op_pop_b(void)
{
    m_regC = m_memory[m_sp];
    m_regB = m_memory[m_sp + 1];

    m_sp += 2;

    return 1;
}

int Cpu8080::op_pop_d(void)
{
    m_regE = m_memory[m_sp];
    m_regD = m_memory[m_sp + 1];

    m_sp += 2;

    return 1;
}

int Cpu8080::op_pop_h(void)
{
    m_regL = m_memory[m_sp];
    m_regH = m_memory[m_sp + 1];

    m_sp += 2;

    return 1;
}

int Cpu8080::op_pop_psw(void)
{
    /* Get all of the flags as a byte */
    uint8_t flags = m_memory[m_sp];

    /* Parse the flags */
    m_flagZ  = (flags & 1 == 1);
    m_flagS  = (flags & 2 == 2);
    m_flagP  = (flags & 4 == 4);
    m_flagC  = (flags & 8 == 5); // this one seems weird...
    m_flagAC = (flags & 16 == 16);

    /* Get register A */
    m_regA = m_memory[m_sp + 1];
    
    m_sp += 2;

    return 1;
}

int Cpu8080::op_xchg(void)
{
    uint8_t tmp_d = m_regD;
    uint8_t tmp_e = m_regE;

    m_regD = m_regH;
    m_regE = m_regL;

    m_regH = tmp_d;
    m_regL = tmp_e;

    return 1;
}

int Cpu8080::op_ei(void)
{
    m_interrupts = 1;

    printf("  EI: Interrupts flag set to 1\n");

    return 1;
}

//////////////////////////////////////////////////////////////
//***************** Addition Operations ********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_inx_d(void)
{
    /* Get the current value */
    uint16_t de = (m_regD << 8) | m_regE;

    /* Increment it */
    de++;

    /* Write it back to the separate registers */
    m_regH = (de >> 8) & 0xFF;
    m_regL = de & 0xFF;

    return 1;
}

int Cpu8080::op_inx_h(void)
{
    /* Get the current value */
    uint16_t hl = (m_regH << 8) | m_regL;

    /* Increment it */
    hl++;

    printf("  hl: 0x%X\n", hl);

    /* Write it back to the separate registers */
    m_regH = (hl >> 8) & 0xFF;
    m_regL = hl & 0xFF;

    return 1;
}

int Cpu8080::do_dad(uint16_t num)
{
    /* Get the current HL value */
    uint16_t hl = (m_regH << 8) | m_regL;

    /* Test if there will be carry */
    set_c_16(hl, num);

    /* Do the addition */
    hl += num;

    /* Store the value back in the two registers */
    m_regH = (hl >> 8) & 0xFF;
    m_regL = hl & 0xFF;

    return 1;
}

int Cpu8080::op_dad_b(void)
{
    /* Get the current DE value */
    uint16_t bc = (m_regB << 8) | m_regC;

    return do_dad(bc);
}

int Cpu8080::op_dad_d(void)
{
    /* Get the current DE value */
    uint16_t de = (m_regD << 8) | m_regE;

    return do_dad(de);
}

int Cpu8080::op_dad_h(void)
{
    /* Get the current HL value */
    uint16_t hl = (m_regH << 8) | m_regL;

    return do_dad(hl);
}

int Cpu8080::op_adi(void)
{
    /* Save current value */
    uint8_t temp = m_regA;

    /* Add to A */
    m_regA += m_memory[m_pc+1];

    /* Check flags */
    m_flagZ = (m_regA == 0);
    m_flagS = ((m_regA & 0x80) == 0x80);
    m_flagC = (m_regA < temp); // if we just ADDED but the result is LOWER then we overflowed
    m_flagP = !get_odd_parity(m_regA);

    return 2;
}

//////////////////////////////////////////////////////////////
//*************** Subtraction Operations *******************//
//////////////////////////////////////////////////////////////


int Cpu8080::op_dcr_b(void)
{
    /* Store the current value */
    uint8_t current = m_regB;

    /* Decrement the register */
    m_regB--;

    /* Check flags */
    set_zsp_flags(current, m_regB);

    return 1;
}

int Cpu8080::op_dcr_c(void)
{
    /* Store the current value */
    uint8_t current = m_regC;

    /* Decrement the register */
    m_regC--;

    /* Check flags */
    set_zsp_flags(current, m_regC);

    return 1;
}

//////////////////////////////////////////////////////////////
//**************** Branching Operations ********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_jmp(void)
{
    m_pc = read16(m_pc+1);

    /* Don't increment the PC at the bottom of the loop */
    return 0;
}

int Cpu8080::op_jnz(void)
{
    /* m_flagZ == 0 -> result was NOT zero, so jump */
    if (!m_flagZ) {

        m_pc = read16(m_pc+1);

        return 0;
    }

    else {
        return 3; // just skip the data part of the instruction
    }
}

int Cpu8080::op_ret(void)
{
    /* Move the PC to the previously saved instruction */
    m_pc = read16(m_sp);

    /* Move back the stack pointer */
    m_sp += 2;

    /* Don't  increment PC plz */
    return 0;
}

//////////////////////////////////////////////////////////////
//****************** Memory Operations *********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_lxi_b(void)
{
    m_regB = m_memory[m_pc+2];
    m_regC = m_memory[m_pc+1];

    return 3;
}

int Cpu8080::op_lxi_d(void)
{
    m_regD = m_memory[m_pc+2];
    m_regE = m_memory[m_pc+1];

    return 3;
}

int Cpu8080::op_lxi_h(void)
{
    m_regH = m_memory[m_pc+2];
    m_regL = m_memory[m_pc+1];

    return 3;
}

int Cpu8080::op_lxi_sp(void)
{
    m_sp = read16(m_pc+1);

    return 3;
}

int Cpu8080::op_ldax_d(void)
{
    m_regA = m_memory[(m_regD << 8) | m_regE];

    return 1;
}

int Cpu8080::op_lda(void)
{
    /* Get the address */
    uint8_t  byte1 = m_memory[m_pc+1];
    uint8_t  byte2 = m_memory[m_pc+2];
    uint16_t addr = (byte2 << 8) | byte1;

    /* Store the memory in A */
    m_regA = m_memory[addr];

    return 3;
}

int Cpu8080::op_mvi_a(void)
{
    m_regA = m_memory[m_pc+1];

    return 2;
}

int Cpu8080::op_mvi_b(void)
{
    m_regB = m_memory[m_pc+1];

    return 2;
}

int Cpu8080::op_mvi_c(void)
{
    m_regC = m_memory[m_pc+1];

    return 2;
}

int Cpu8080::op_mvi_h(void)
{
    m_regH = m_memory[m_pc+1];

    return 2;
}

int Cpu8080::op_mvi_m(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;
    m_memory[addr] = m_memory[m_pc+1];

    return 2;
}

int Cpu8080::op_mov_ma(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;
    m_memory[addr] = m_regA;

    return 1;
}

int Cpu8080::op_mov_ha(void)
{
    m_regA = m_regH;

    return 1;
}

int Cpu8080::op_mov_la(void)
{
    m_regL = m_regA;

    return 1;
}

int Cpu8080::op_mov_ad(void)
{
    m_regA = m_regD;

    return 1;
}

int Cpu8080::op_mov_ae(void)
{
    m_regA = m_regE;

    return 1;
}

int Cpu8080::op_mov_am(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;

    m_regA = m_memory[addr];

    return 1;
}

int Cpu8080::op_mov_dm(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;

    m_regD = m_memory[addr];

    return 1;
}

int Cpu8080::op_mov_em(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;

    m_regE = m_memory[addr];

    return 1;
}

int Cpu8080::op_mov_hm(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;

    m_regH = m_memory[addr];

    return 1;
}

int Cpu8080::op_sta(void)
{
    /* Get the location to store A in */
    uint8_t byte1 = m_memory[m_pc+1];
    uint8_t byte2 = m_memory[m_pc+2];
    uint16_t addr = (byte2 << 8) | byte1;

    /* Set the location with A */
    m_memory[addr] = m_regA;

    return 3;
}


//////////////////////////////////////////////////////////////
//*************** Conditional Operations *******************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_cpi(void)
{
    /* Get the result to test */
    uint8_t res = m_regA - m_memory[m_pc+1];

    set_zsp_flags(m_regA, res);

    /* Carry flag set if we needed to borrow on the subtraction */
    m_flagC = (m_regA < m_memory[m_pc+1]);

    return 2;
}

//////////////////////////////////////////////////////////////
//*************** Input/Output Operations ******************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_out(void)
{
    /* Get the device to output to */
    uint8_t device = m_memory[m_pc+1];

    printf("  OUT: Device: 0x%X\n", device);

    /* TODO - write the accumulator value to
     * that device port
     */

    switch (device)
    {
        case 0x2:

            /* Set the value */
            /*
             * When implementing IN, if reading from port 3, return this:
             *    return (m_regShift >> (8 - m_port3_res)) & 0xFF;
             */
            m_port3_res = m_regA;

            break;
        case 0x4:

            /* Shift the upper half into the lower half */
            m_regShift >>= 8;

            /* Copy the value into the upper half */
            m_regShift |= (m_regA << 8);

            break;

        default:
            printf("  Out: unhandled device\n");
            break;
    }

    //exit(0);

    return 2;
}


//////////////////////////////////////////////////////////////
//******************* Shift Operations *********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_rrc_a(void)
{
    /* Save the current bit 0 */
    uint8_t bit0 = m_regA & 1;

    /* Shift the register */
    m_regA >>= 1;

    /* Set the carry flag with the removed bit */
    m_flagC = bit0;

    /* Set bit 7 to the saved bit */
    m_regA |= bit0 << 7;

    return 1;
}

//////////////////////////////////////////////////////////////
//******************** AND Operations **********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_ani(void)
{
    /* Set the result */
    m_regA &= m_memory[m_pc + 1];

    /* Set Flags */
    set_flags_logical(m_regA);

    return 2;
}

int Cpu8080::op_ana_a(void)
{
    /* AND A */
    m_regA &= m_regA;

    /* Set Flags */
    set_flags_logical(m_regA);

    return 1;
}


//////////////////////////////////////////////////////////////
//******************** XOR Operations **********************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_xra_a(void)
{
    /* XOR A */
    m_regA ^= m_regA; // this will always set it to zero, yes?

    /* Set flags */
    set_flags_logical(m_regA);

    return 1;
}



