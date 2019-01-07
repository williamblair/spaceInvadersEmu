/*
 * Cpu8080.cpp
 */

#include "Cpu8080.h"

Cpu8080::Cpu8080() {
    m_pc = 0;
    m_sp = 0; 

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

    m_memory = NULL;

    /* Load the space invaders rom into memory */
    //memcpy(&m_memory[ROM_H_START], ROM_H, ROM_H_SIZE);
    //memcpy(&m_memory[ROM_G_START], ROM_G, ROM_G_SIZE);
    //memcpy(&m_memory[ROM_F_START], ROM_F, ROM_F_SIZE);
    //memcpy(&m_memory[ROM_E_START], ROM_E, ROM_E_SIZE);
}

Cpu8080::~Cpu8080() {

}

bool Cpu8080::init(uint8_t *memory, uint8_t *ports)
{
    m_memory = memory;
    m_ports = ports;

    return true;
}

void Cpu8080::run_next_op(void)
{
    uint8_t opcode = m_memory[m_pc];
    int num_increment = 0; // how much to increase the PC by at the bottom

    // debug - show which op is going to be ran
    //std::string op;

    switch (opcode)
    {
        case 0x00:  num_increment = 1;         break; // NOP
        case 0x05:  num_increment = op_dcr();  break; // DCR B
        case 0x06:  num_increment = op_mvi();  break; // MVI B, D8
        case 0x11:  num_increment = op_lxi();  break; // LXI D, D8
        case 0x13:  num_increment = op_inx();  break; // INX D
        case 0x1A:  num_increment = op_ldax(); break; // LDAX D

        case 0x21:  num_increment = op_lxi();  break; // LXI H, D8
        case 0x23:  num_increment = op_inx();  break; // INX H

        case 0x31:  num_increment = op_lxi();  break; // LXI sp

        case 0x77:  num_increment = op_mov();  break; // MOV M,A

        case 0xC3:  num_increment = op_jmp();  break; // JMP adr

        case 0xCD:  num_increment = op_call(); break; // CALL adr

        /* Unhandled opcode, exit */
        default:    op_unimplemented(); break;
    }

    m_pc += num_increment;

    printf("PC: 0x%04X    Opcode: 0x%02X    %s\n", 
        m_pc, m_memory[m_pc], op_lookup[m_memory[m_pc]]);

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


//////////////////////////////////////////////////////////////
//******************* JUMP Instructions ********************//
//////////////////////////////////////////////////////////////

int Cpu8080::op_jmp(void)
{
    /* Get the low and high bits */
    uint8_t low  = m_memory[m_pc+1];
    uint8_t high = m_memory[m_pc+2];

    /* Set the new PC address */
    m_pc = (high << 8) | low;

    /* DON't adjust the PC at all please */
    return 0;
}


//////////////////////////////////////////////////////////////
//**************** Immediate Instructions ******************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_lxi(void)
{
    /* The immediate value */
    uint8_t byte2 = m_memory[m_pc+1];
    uint8_t byte3 = m_memory[m_pc+2];

    /* Which registers? */
    uint8_t pair = (m_memory[m_pc] >> 4) & 0x3;
    switch (pair)
    {
        /* BC */
        case 0:
            m_regB = byte3;
            m_regC = byte2;
            break;

        /* DE */
        case 1:
            m_regD = byte3;
            m_regE = byte2;
            break;

        /* HL */
        case 2:
            m_regH = byte3;
            m_regL = byte2;
            break;

        /* SP */
        case 3:
            m_sp = (byte3 << 8) | byte2;
            break;

        default:
            printf("  ** Invalid LXI Register Pair **\n"
                   "     This shouldn't happen :(    \n"
                );
            exit(0);
    }

    return 3;
}

int Cpu8080::op_mvi(void)
{
    /* The immediate value */
    uint8_t byte2 = m_memory[m_pc+1];

    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 3) & 0x7;
    switch (reg)
    {
        /* B */
        case 0:
            m_regB = byte2;
            break;
        
        /* C */
        case 1:
            m_regC = byte2;
            break;
        
        /* D */
        case 2:
            m_regD = byte2;
            break;
        
        /* E */
        case 3:
            m_regE = byte2;
            break;
        
        /* H */
        case 4:
            m_regH = byte2;
            break;
        
        /* L */
        case 5:
            m_regL = byte2;
            break;
        
        /* M */
        case 6:
            m_memory[(m_regH << 8) | m_regL] = byte2;
            break;

        /* A */
        case 7:
            m_regA = byte2;
            break;

        default:
            printf("  ** Invalid MVI Register **  \n"
                   "     This shouldn't happen :( \n"
                );
            exit(0);
    }

    return 2;
}

//////////////////////////////////////////////////////////////
//****************** Call Instructions *********************//
//////////////////////////////////////////////////////////////

int Cpu8080::op_call(void)
{
    /* Get the address to jump to */
    uint8_t low  = m_memory[m_pc+1];
    uint8_t high = m_memory[m_pc+2];
    uint16_t addr = (high << 8) | low;

    /* Store the next PC value on the stack */
    uint16_t next_adr = m_pc + 3;
    m_memory[m_sp - 1] = (next_adr >> 8) & 0xFF;
    m_memory[m_sp - 2] = next_adr & 0xFF;
    m_sp -= 2;

    /* Move PC to the call addr */
    m_pc = addr;

    /* Don't Adjust the PC please */
    return 0;
}

//////////////////////////////////////////////////////////////
//************** Data Transfer Instructions ****************//
//////////////////////////////////////////////////////////////

int Cpu8080::op_ldax(void)
{
    uint16_t addr;

    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 4) & 1;

    /* BC */
    if (reg == 0) {
        addr = (m_regB << 8) | m_regC;
    }

    /* DE */
    else {
        addr = (m_regD << 8) | m_regE;
    }

    m_regA = m_memory[addr];

    return 1;
}

int Cpu8080::op_mov(void)
{
    /* The source and destination registers */
    uint8_t source = m_memory[m_pc] & 0x7;
    uint8_t dest   = (m_memory[m_pc] >> 3) & 0x7;

    uint8_t *src_reg; // where we're actually going to store the value
    uint8_t *dst_reg;

    switch (source)
    {
        case 0:            src_reg = &m_regB; break; // B
        case 1:            src_reg = &m_regC; break; // C
        case 2:            src_reg = &m_regD; break; // D
        case 3:            src_reg = &m_regE; break; // E
        case 4:            src_reg = &m_regH; break; // H
        case 5:            src_reg = &m_regL; break; // L
        
        /* M */
        case 6:
            src_reg = &m_memory[(m_regH << 8) | m_regL];
            break;

        /* A */
        case 7:
            src_reg = &m_regA;
            break;
        default:
            printf("  ** Invalid MOV source **    \n"
                   "     This shouldn't happen :( \n"
                );
            exit(0);
    }

    switch (dest)
    {
        case 0:            dst_reg = &m_regB; break; // B
        case 1:            dst_reg = &m_regC; break; // C
        case 2:            dst_reg = &m_regD; break; // D
        case 3:            dst_reg = &m_regE; break; // E
        case 4:            dst_reg = &m_regH; break; // H
        case 5:            dst_reg = &m_regL; break; // L
        
        /* M */
        case 6:
            dst_reg = &m_memory[(m_regH << 8) | m_regL];
            break;

        /* A */
        case 7:
            dst_reg = &m_regA;
            break;
        default:
            printf("  ** Invalid MOV source **    \n"
                   "     This shouldn't happen :( \n"
                );
            exit(0);
    }

    /* Finally, set the value */
    *dst_reg = *src_reg;

    return 1;
}


//////////////////////////////////////////////////////////////
//************** Register Pair Instructions ****************//
//////////////////////////////////////////////////////////////

int Cpu8080::op_inx(void)
{
    /* Which register pair? */
    uint8_t reg = (m_memory[m_pc] >> 4) & 0x3;

    /* Pointers to the register pair we want to increment */
    uint8_t *p1 = NULL, *p2 = NULL;

    switch (reg)
    {
        case 0: p1 = &m_regB; p2 = &m_regC; break; // BC
        case 1: p1 = &m_regD; p2 = &m_regE; break; // DE
        case 2: p1 = &m_regH; p2 = &m_regL; break; // HL
        case 3: m_sp++;                     break; // sp
        default:
            printf("  ** Invalid INX Register Pair ** \n"
                   "     This shouldn't happen :(     \n"
                );
            exit(0);
    }

    /* We don't do this part if the register pair is SP */
    if (p1 != NULL) {

        /* Get the total value and increment */
        uint16_t val = (*p1 << 8) | *p2;
        val++;

        /* Re store in registers */
        *p1 = (val >> 8) & 0xFF;
        *p2 = val & 0xFF;
    }

    return 1;
}

//////////////////////////////////////////////////////////////
//************* Single Register Instructions ***************//
//////////////////////////////////////////////////////////////

int Cpu8080::op_dcr(void)
{
    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 3) & 0x7;

    /* Pointer to the register we want to decrement */
    uint8_t *regp = NULL;

    switch (reg)
    {
        case 0: regp = &m_regB; break; // B
        case 1: regp = &m_regC; break; // C
        case 2: regp = &m_regD; break; // D
        case 3: regp = &m_regE; break; // E
        case 4: regp = &m_regH; break; // H
        case 5: regp = &m_regL; break; // L
        case 6: regp = &m_memory[(m_regH << 8) | m_regL]; break; // M
        case 7: regp = &m_regA; break; // A
        default:
            printf("  ** Invalid DCR Register **  \n"
                   "     This shouldn't happen :( \n"
                );
            exit(0);
            
    }

    /* Decrement the register */
    (*regp)--;

    /* Set flags */
    m_flagZ = (*regp == 0);
    m_flagS = ((int8_t)(*regp) < 0);
    m_flagP = !get_odd_parity(*regp);
    // m_flagAC = // unimplemented

    return 1;
}







