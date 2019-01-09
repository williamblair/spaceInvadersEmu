/*
 * Cpu8080.cpp
 */

#include "Cpu8080.h"

// TEST
static int num_instructions = 0; // number of instructions ran

Cpu8080::Cpu8080() {
    m_pc = 0;
    //m_sp = 0; 
    m_sp = 0xF000; // javascript 8080 starts it at this

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
    m_shiftAmount= 0;

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
        case 0x01:  num_increment = op_lxi();  break; // LXI B,D
        case 0x04:  num_increment = op_inr();  break; // INR B
        case 0x05:  num_increment = op_dcr();  break; // DCR B
        case 0x06:  num_increment = op_mvi();  break; // MVI B, D8
        case 0x07:  num_increment = op_rlc();  break; // RLC
        case 0x09:  num_increment = op_dad();  break; // DAD B
        case 0x0C:  num_increment = op_inr();  break; // INR C
        case 0x0D:  num_increment = op_dcr();  break; // DCR C
        case 0x0E:  num_increment = op_mvi();  break; // MVI C, D8
        case 0x0F:  num_increment = op_rrc();  break; // RRC
        case 0x11:  num_increment = op_lxi();  break; // LXI D, D8
        case 0x14:  num_increment = op_inr();  break; // INR D
        case 0x15:  num_increment = op_dcr();  break; // DCR D
        case 0x19:  num_increment = op_dad();  break; // DAD D
        case 0x13:  num_increment = op_inx();  break; // INX D
        case 0x16:  num_increment = op_mvi();  break; // MVI D, D8
        case 0x1A:  num_increment = op_ldax(); break; // LDAX D
        case 0x1C:  num_increment = op_inr();  break; // INR E
        case 0x1D:  num_increment = op_dcr();  break; // DCR E
        case 0x1E:  num_increment = op_mvi();  break; // MVI E,D8
        case 0x1F:  num_increment = op_rar();  break; // RAR

        case 0x21:  num_increment = op_lxi();  break; // LXI H, D8
        case 0x23:  num_increment = op_inx();  break; // INX H
        case 0x24:  num_increment = op_inr();  break; // INR H
        case 0x25:  num_increment = op_dcr();  break; // DCR H
        case 0x26:  num_increment = op_mvi();  break; // MVI H,D8
        case 0x29:  num_increment = op_dad();  break; // DAD H
        case 0x2A:  num_increment = op_lhld(); break; // LHLD adr
        case 0x2C:  num_increment = op_inr();  break; // INR L
        case 0x2D:  num_increment = op_dcr();  break; // DCR L
        case 0x2E:  num_increment = op_mvi();  break; // MVI L,D8

        case 0x31:  num_increment = op_lxi();  break; // LXI sp
        case 0x32:  num_increment = op_sta();  break; // STA adr
        case 0x36:  num_increment = op_mvi();  break; // MVI M,D8
        case 0x3A:  num_increment = op_lda();  break; // LDA adr
        case 0x3D:  num_increment = op_dcr();  break; // DCR A
        case 0x3C:  num_increment = op_inr();  break; // INR A
        case 0x3E:  num_increment = op_mvi();  break; // MVI A,D8

        case 0x41:  num_increment = op_mov();  break; // MOV B,C
        case 0x42:  num_increment = op_mov();  break; // MOV B,D
        case 0x43:  num_increment = op_mov();  break; // MOV B,E
        case 0x44:  num_increment = op_mov();  break; // MOV B,H
        case 0x45:  num_increment = op_mov();  break; // MOV B,L
        case 0x47:  num_increment = op_mov();  break; // MOV B,A
        case 0x48:  num_increment = op_mov();  break; // MOV C,B
        case 0x4A:  num_increment = op_mov();  break; // MOV C,D
        case 0x4B:  num_increment = op_mov();  break; // MOV C,E
        case 0x4C:  num_increment = op_mov();  break; // MOV C,H
        case 0x4D:  num_increment = op_mov();  break; // MOV C,L
        case 0x4F:  num_increment = op_mov();  break; // MOV C,A

        case 0x50:  num_increment = op_mov();  break; // MOV D,B
        case 0x51:  num_increment = op_mov();  break; // MOV D,C
        case 0x53:  num_increment = op_mov();  break; // MOV D,E
        case 0x54:  num_increment = op_mov();  break; // MOV D,H
        case 0x55:  num_increment = op_mov();  break; // MOV D,L
        case 0x56:  num_increment = op_mov();  break; // MOV D,M
        case 0x57:  num_increment = op_mov();  break; // MOV D,A
        case 0x58:  num_increment = op_mov();  break; // MOV E,B
        case 0x59:  num_increment = op_mov();  break; // MOV E,C
        case 0x5A:  num_increment = op_mov();  break; // MOV E,D
        case 0x5C:  num_increment = op_mov();  break; // MOV E,H
        case 0x5D:  num_increment = op_mov();  break; // MOV E,L
        case 0x5E:  num_increment = op_mov();  break; // MOV E,M
        case 0x5F:  num_increment = op_mov();  break; // MOV E,A

        case 0x60:  num_increment = op_mov();  break; // MOV H,B
        case 0x61:  num_increment = op_mov();  break; // MOV H,C
        case 0x62:  num_increment = op_mov();  break; // MOV H,D
        case 0x63:  num_increment = op_mov();  break; // MOV H,E
        case 0x65:  num_increment = op_mov();  break; // MOV H,L
        case 0x66:  num_increment = op_mov();  break; // MOV H,M
        case 0x67:  num_increment = op_mov();  break; // MOV H,A
        case 0x68:  num_increment = op_mov();  break; // MOV L,B
        case 0x69:  num_increment = op_mov();  break; // MOV L,C
        case 0x6A:  num_increment = op_mov();  break; // MOV L,D
        case 0x6B:  num_increment = op_mov();  break; // MOV L,E
        case 0x6C:  num_increment = op_mov();  break; // MOV L,H
        case 0x6F:  num_increment = op_mov();  break; // MOV L,A

        case 0x77:  num_increment = op_mov();  break; // MOV M,A
        case 0x78:  num_increment = op_mov();  break; // MOV A,B
        case 0x79:  num_increment = op_mov();  break; // MOV A,C
        case 0x7A:  num_increment = op_mov();  break; // MOV A,D
        case 0x7B:  num_increment = op_mov();  break; // MOV A,E
        case 0x7C:  num_increment = op_mov();  break; // MOV A,H
        case 0x7D:  num_increment = op_mov();  break; // MOV A,L
        case 0x7E:  num_increment = op_mov();  break; // MOV A,M

        case 0x80:  num_increment = op_add();  break; // ADD B
        case 0x81:  num_increment = op_add();  break; // ADD C
        case 0x82:  num_increment = op_add();  break; // ADD D
        case 0x83:  num_increment = op_add();  break; // ADD E
        case 0x84:  num_increment = op_add();  break; // ADD H
        case 0x85:  num_increment = op_add();  break; // ADD L
        case 0x87:  num_increment = op_add();  break; // ADD A
        case 0x88:  num_increment = op_adc();  break; // ADC B
        case 0x89:  num_increment = op_adc();  break; // ADC C
        case 0x8A:  num_increment = op_adc();  break; // ADC D
        case 0x8B:  num_increment = op_adc();  break; // ADC E
        case 0x8C:  num_increment = op_adc();  break; // ADC H
        case 0x8D:  num_increment = op_adc();  break; // ADC L
        case 0x8F:  num_increment = op_adc();  break; // ADC A

        case 0x90:  num_increment = op_sub();  break; // SUB B
        case 0x91:  num_increment = op_sub();  break; // SUB C
        case 0x92:  num_increment = op_sub();  break; // SUB D
        case 0x93:  num_increment = op_sub();  break; // SUB E
        case 0x94:  num_increment = op_sub();  break; // SUB H
        case 0x95:  num_increment = op_sub();  break; // SUB L
        case 0x97:  num_increment = op_sub();  break; // SUB A
        case 0x98:  num_increment = op_sbb();  break; // SBB B
        case 0x99:  num_increment = op_sbb();  break; // SBB C
        case 0x9A:  num_increment = op_sbb();  break; // SBB D
        case 0x9B:  num_increment = op_sbb();  break; // SBB E
        case 0x9C:  num_increment = op_sbb();  break; // SBB H
        case 0x9D:  num_increment = op_sbb();  break; // SBB L
        case 0x9F:  num_increment = op_sbb();  break; // SBB A

        case 0xA7:  num_increment = op_ana();  break; // ANA A
        case 0xAF:  num_increment = op_xra();  break; // XRA A

        case 0xC0:  num_increment = op_rnz();  break; // RNZ
        case 0xC1:  num_increment = op_pop();  break; // POP B
        case 0xC2:  num_increment = op_jnz();  break; // JNZ adr
        case 0xC4:  num_increment = op_cnz();  break; // CNZ adr
        case 0xC3:  num_increment = op_jmp();  break; // JMP adr
        case 0xC5:  num_increment = op_push(); break; // PUSH B
        case 0xC6:  num_increment = op_adi();  break; // ADI D8
        case 0xC8:  num_increment = op_rz();   break; // RZ
        case 0xC9:  num_increment = op_ret();  break; // RET
        case 0xCA:  num_increment = op_jz();   break; // JZ adr
        case 0xCC:  num_increment = op_cz();   break; // CZ adr
        case 0xCE:  num_increment = op_aci();  break; // ACI D8

        case 0xCD:
#ifdef CPU_DIAG

        printf("\n\n!!!!!CPU DIAG CD!!!!\n\n");

        if (((m_memory[m_pc+2] << 8) | m_memory[m_pc+1]) == 5)
        {
            printf("    CPU DIAG: addr = 5\n");

            if (m_regC == 9)
            {
                printf("    CPU DIAG: regC = 9\n");

                uint16_t offset = (m_regD << 8) | m_regE;
                char *str = (char *)&m_memory[offset+3]; // skip the prefix bytes

                while (*str != '$') printf("%c", *str++);
                printf("\n");

                exit(0);
            }
            else if (m_regC == 2)
            {
                printf("    CPU DIAG: regC = 2\n");

                // saw this in the inspected code, never saw it called
                printf("Print char routine called\n");

                exit(0);
            }

            num_increment = 3;
        }
        else if (((m_memory[m_pc+2] << 8) | m_memory[m_pc+1]) == 0)
        {
            exit(0);
        }
        else {
            num_increment = op_call(); 
        }
        break;
#else
            num_increment = op_call(); 
            break; // CALL adr
#endif

        case 0xD0:  num_increment = op_rnc();  break; // RNC
        case 0xD1:  num_increment = op_pop();  break; // POP D
        case 0xD2:  num_increment = op_jnc();  break; // JNC adr
        case 0xD3:  num_increment = op_out();  break; // OUT
        case 0xD4:  num_increment = op_cnc();  break; // CNC adr
        case 0xD5:  num_increment = op_push(); break; // PUSH D
        case 0xD6:  num_increment = op_sui();  break; // SUI D8
        case 0xD8:  num_increment = op_rc();   break; // RC
        case 0xDA:  num_increment = op_jc();   break; // JC adr
        case 0xDC:  num_increment = op_cc();   break; // CC adr
        case 0xDE:  num_increment = op_sbi();  break; // SBI D8

        case 0xE0:  num_increment = op_rpo();  break; // RPO
        case 0xE1:  num_increment = op_pop();  break; // POP H
        case 0xE2:  num_increment = op_jpo();  break; // JPO adr
        case 0xE3:  num_increment = op_xthl(); break; // XTHL
        case 0xE4:  num_increment = op_cpo();  break; // CPO adr
        case 0xE5:  num_increment = op_push(); break; // PUSH H
        case 0xE6:  num_increment = op_ani();  break; // ANI D8
        case 0xE8:  num_increment = op_rpe();  break; // RPE
        case 0xEA:  num_increment = op_jpe();  break; // JPE adr
        case 0xEB:  num_increment = op_xchg(); break; // XCHG
        case 0xEC:  num_increment = op_cpe();  break; // CPE adr
        case 0xEE:  num_increment = op_xri();  break; // XRI D8

        case 0xF0:  num_increment = op_rp();   break; // RP
        case 0xF1:  num_increment = op_pop();  break; // POP PSW
        case 0xF2:  num_increment = op_jp();   break; // JP adr
        case 0xF4:  num_increment = op_cp();   break; // CP adr
        case 0xF5:  num_increment = op_push(); break; // PUSH PSW
        case 0xF6:  num_increment = op_ori();  break; // ORI D
        case 0xF8:  num_increment = op_rm();   break; // RM
        case 0xFA:  num_increment = op_jm();   break; // JM adr
        case 0xFB:  num_increment = op_ei();   break; // EI
        case 0xFC:  num_increment = op_cm();   break; // CM 
        case 0xFE:  num_increment = op_cpi();  break; // CPI D8

        /* Unhandled opcode, exit */
        default:    op_unimplemented(); break;
    }

    m_pc += num_increment;

    printf("0x%04X    Opcode: 0x%02X    %s\n", 
        m_pc, m_memory[m_pc], op_lookup[m_memory[m_pc]]);

    printf("      af   bc   de   hl   pc   sp flags   \n");
    printf("    %04X %04X %04X %04X %04X %04X %c%c%c%c\n\n", 
            (m_regA << 8) | 
                   ( m_flagC         |
                     (m_flagP  << 2) |
                     (m_flagAC << 4) |
                     (m_flagZ  << 6) |
                     (m_flagS  << 7)), 
            (m_regB << 8) | m_regC,
            (m_regD << 8) | m_regE,
            (m_regH << 8) | m_regL,
            m_pc,
            m_sp,
            m_flagC ? 'c' : '.',
            m_flagS ? 's' : '.',
            m_flagP ? 'p' : '.',
            m_flagZ ? 'z' : '.'
        );

    //getchar();

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
    num ^= (num >> 4 | num << 4);
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

int Cpu8080::op_jnz(void)
{
    if (m_flagZ == 0) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jz(void)
{
    if (m_flagZ != 0) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jnc(void)
{
    if (m_flagC == 0) {
        
        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jpe(void)
{
    if (m_flagP == 1) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jp(void)
{
    if (m_flagS == 0) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jc(void)
{
    if (m_flagC == 1) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jpo(void)
{
    if (m_flagP == 0) {

        return op_jmp();

    }

    return 3;
}

int Cpu8080::op_jm(void)
{
    if (m_flagS == 1) {

        return op_jmp();

    }

    return 3;
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

int Cpu8080::op_cpi(void)
{
    /* Compare by subtracting */
    uint8_t res = m_regA - m_memory[m_pc+1];

    /* Set flags */
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    //m_flagC = (((int8_t)m_regA > 0 && (int8_t) res < 0) ||
    //           ((int8_t)m_regA < 0 && (int8_t) res > 0));
    m_flagC = m_regA < m_memory[m_pc+1];
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented

    /* the result is NOT stored back in A */

    return 2;
}

int Cpu8080::op_ani(void)
{
    /* Get the data to add */
    uint8_t val = m_memory[m_pc+1];

    /* add the values */
    uint8_t res = m_regA & val;


    /* Set flags */
    m_flagC = 0; // the carry flag is reset to 0
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0); // according to docs, 'the zero bit will be set if and only if the low-order four bits were originally 0'
    m_flagP = !get_odd_parity(res);

    /* store back in A */
    m_regA = res;

    return 2;
}

int Cpu8080::op_ori(void)
{
    /* OR */
    uint8_t res = m_regA | m_memory[m_pc+1];

    /* Set flags */
    m_flagC = 0; // the carry flag is reset
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);

    /* Store the result */
    m_regA = res;

    return 2;
}

int Cpu8080::op_adi(void)
{
    /* ADD */
    uint8_t res = m_regA + m_memory[m_pc+1];

    /* Set flags */
    m_flagC = res < m_regA; // impies overflow
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented;

    /* Store the result back in A */
    m_regA = res;

    return 2;
}

int Cpu8080::op_aci(void)
{
    /* ADD with carry */
    uint8_t res = m_regA + m_memory[m_pc+1] + (m_flagC & 1);

    /* Set flags */
    m_flagC = res < m_regA; // impies overflow
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented;

    /* Store the result back in A */
    m_regA = res;

    return 2;
}

int Cpu8080::op_sui(void)
{
    /* Subtract */
    uint8_t res = m_regA - m_memory[m_pc+1];

    /* Set flags */
    m_flagC = res > m_regA; // impies overflow
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented;
    
    /* Store the result back in A */
    m_regA = res;

    return 2;
}

int Cpu8080::op_sbi(void)
{
    /* Subtract */
    uint8_t res = m_regA - m_memory[m_pc+1] - (m_flagC & 1);

    /* Set flags */
    m_flagC = res > m_regA; // impies overflow
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented;

    /* Store the result back in A */
    m_regA = res;

    return 2;
}

int Cpu8080::op_xri(void)
{
    /* XOR */
    uint8_t res = m_regA ^ m_memory[m_pc+1];

    /* Set flags */
    m_flagC = 0; // carry bit is set to 0
    m_flagS = ((int8_t)res < 0);
    m_flagZ = (res == 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented;

    /* Store the result in A */
    m_regA = res;

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

int Cpu8080::op_cnz(void)
{
    if (m_flagZ == 0) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cc(void)
{
    if (m_flagC == 1) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cpo(void)
{
    if (m_flagP == 0) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cm(void)
{
    if (m_flagS == 1) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cnc(void)
{
    if (m_flagC == 0) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cpe(void)
{
    if (m_flagP == 1) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cp(void)
{
    if (m_flagS == 0) {
        return op_call();
    }

    return 3;
}

int Cpu8080::op_cz(void)
{
    if (m_flagZ == 1) {
        return op_call();
    }

    return 3;
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

int Cpu8080::op_push(void)
{
    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 4) & 0x3;

    switch (reg)
    {
        /* BC */
        case 0:
            m_memory[m_sp-1] = m_regB;
            m_memory[m_sp-2] = m_regC;
            break;
        /* DE */
        case 1:
            m_memory[m_sp-1] = m_regD;
            m_memory[m_sp-2] = m_regE;
            break;
        /* HL */
        case 2:
            m_memory[m_sp-1] = m_regH;
            m_memory[m_sp-2] = m_regL;
            break;
        /* A PSW */
        case 3:
            /* Because the flags are stored seperately
             * We need to compensate
             */
            m_memory[m_sp-1] = m_regA;
            m_memory[m_sp-2] = m_flagC         |
                               (m_flagP  << 2) |
                               (m_flagAC << 4) |
                               (m_flagZ  << 6) |
                               (m_flagS  << 7);
            break;
        default:
            printf("  ** Invalid PUSH Register Pair ** \n"
                   "     This shouldn't happen :(     \n"
                );
            exit(0);
    }

    /* Decrement the stack pointer */
    m_sp -= 2;

    return 1;
}

int Cpu8080::op_pop(void)
{
    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 4) & 0x3;
    uint8_t psw;

    switch (reg)
    {
        /* BC */
        case 0:
            m_regB = m_memory[m_sp+1];
            m_regC = m_memory[m_sp];
            break;

        /* DE */
        case 1:
            m_regD = m_memory[m_sp+1];
            m_regE = m_memory[m_sp];
            break;

        /* HL */
        case 2:
            m_regH = m_memory[m_sp+1];
            m_regL = m_memory[m_sp];
            break;

        /* A/PSW */
        case 3:
            m_regA = m_memory[m_sp+1];
            psw    = m_memory[m_sp];
            
            m_flagC  = psw & 1;
            m_flagP  = (psw >> 2) & 1;
            m_flagAC = (psw >> 4) & 1;
            m_flagZ  = (psw >> 6) & 1;
            m_flagS  = (psw >> 7) & 1;

            break;

        default:
            printf("  ** Invalid POP Register Pair ** \n"
                   "     This shouldn't happen :(     \n"
                );
            exit(0);
    }

    m_sp += 2;

    return 1;
}

int Cpu8080::op_dad(void)
{
    /* Which register pair? */
    uint8_t reg = (m_memory[m_pc] >> 4) & 0x3;

    /* Pointers for the two registers, plus the
     * combined value we're going to increment
     */
    uint8_t *p1 = NULL, *p2 = NULL;
    uint16_t pair;

    switch (reg)
    {
        case 0: p1 = &m_regB; p2 = &m_regC; break; // BC
        case 1: p1 = &m_regD; p2 = &m_regE; break; // DE
        case 2: p1 = &m_regH; p2 = &m_regL; break; // HL

        /* SP */
        case 3:
            m_flagC = (0xFFFF - m_sp) < m_sp;
            m_sp += m_sp;

            break;

        default:
            printf("  ** Invalid DAD Register Pair ** \n"
                   "     This shouldn't happen :(     \n"
                );
            exit(0);
            
    }

    /* If the pair wasn't SP... */
    if (p1 != NULL) {

        /* Get the value and set carry if it will occur */
        pair = (*p1 << 8) | *p2;
        m_flagC = (0xFFFF - pair) < pair;
        pair += pair;

        /* Store it back in the two registers */
        *p1 = (pair >> 8) & 0xFF;
        *p2 = pair & 0xFF;
    }

    return 1;
}

int Cpu8080::op_xchg(void)
{
    uint8_t tempH = m_regH;
    uint8_t tempL = m_regL;

    m_regH = m_regD;
    m_regL = m_regE;

    m_regD = tempH;
    m_regE = tempL;

    return 1;
}

int Cpu8080::op_xthl(void)
{
    /* Save the current value of HL */
    uint8_t prevH = m_regH;
    uint8_t prevL = m_regL;

    /* Set the HL registers */
    m_regH = m_memory[m_sp+1];
    m_regL = m_memory[m_sp];

    /* Set the stack values */
    m_memory[m_sp+1] = prevH;
    m_memory[m_sp]   = prevL;

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
    //m_flagP = !get_odd_parity(*regp);
    m_flagP = !get_odd_parity(*regp);
    // m_flagAC = // unimplemented

    return 1;
}

int Cpu8080::op_inr(void)
{
    /* Which register? */
    uint8_t reg = (m_memory[m_pc] >> 3) & 0x7;

    /* Pointer to the register we want to increment */
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

    /* Increment the register */
    (*regp)++;

    /* Set flags */
    m_flagZ = (*regp == 0);
    m_flagS = ((int8_t)(*regp) < 0);
    m_flagP = !get_odd_parity(*regp);
    // m_flagAC = // unimplemented

    return 1;
}

//////////////////////////////////////////////////////////////
//********** Return from Subroutine Instructions ***********//
//////////////////////////////////////////////////////////////
int Cpu8080::op_ret(void)
{
    /* Get the next instruction stored on the stack */
    uint8_t low  = m_memory[m_sp];
    uint8_t high = m_memory[m_sp+1];

    m_pc = (high << 8) | low;

    /* Increment the stack pointer */
    m_sp += 2;

    /* Don't increment PC please */
    return 0;
}

int Cpu8080::op_rpe(void)
{
    if (m_flagP == 1) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rpo(void)
{
    if (m_flagP == 0) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rp(void)
{
    if (m_flagS == 0) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rm(void)
{
    if (m_flagS == 1) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rc(void)
{
    if (m_flagC == 1) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rnc(void)
{
    if (m_flagC == 0) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rz(void)
{
    if (m_flagZ == 1) {
        return op_ret();
    }

    return 1;
}

int Cpu8080::op_rnz(void)
{
    if (m_flagZ == 0) {
        return op_ret();
    }

    return 1;
}

//////////////////////////////////////////////////////////////
//************* Direct Addressing Instructions *************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_sta(void)
{
    /* Get the address to write A to */
    uint8_t low  = m_memory[m_pc+1];
    uint8_t high = m_memory[m_pc+2];
    uint16_t addr = (high << 8) | low;

    /* Write A to said addr */
    m_memory[addr] = m_regA;

    return 3;
}

int Cpu8080::op_lhld(void)
{
    /* Get the address */
    uint8_t low  = m_memory[m_pc+1];
    uint8_t high = m_memory[m_pc+2];
    uint16_t addr = (high << 8) | low;

    /* Replace H and L */
    m_regH = m_memory[addr+1];
    m_regL = m_memory[addr];

    return 3;
}

int Cpu8080::op_lda(void)
{
    /* Get the address */
    uint8_t low = m_memory[m_pc+1];
    uint8_t high = m_memory[m_pc+2];
    uint16_t addr = (high << 8) | low;

    m_regA = m_memory[addr];

    return 3;
}

//////////////////////////////////////////////////////////////
//************** Input/Output Instructions *****************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_out(void)
{
    /* Which device? */
    uint8_t device = m_memory[m_pc+1];

    switch (device)
    {
        case 0x6:
            printf("  OUT: writing to watchdog (0x6)\n");
            m_ports[0x6] = 1;
            break;

        case 0x2:
            printf("  OUT: writing to port 2 (shift amount): 0x%X\n", m_regA);
            m_shiftAmount = m_regA;
            break;

        case 0x3:
            printf("  OUT: writing to port 3 (sound): 0x%X\n", m_regA);
            // TODO
            break;

        case 0x5:
            printf("  OUT: writing to port 5 (Other sound): 0x%X\n", m_regA);
            // TODO
            break;

        default:
            printf("  Unhandled OUT Device: 0x%X\n", device);
            exit(0);
    }

    return 2;
}

//////////////////////////////////////////////////////////////
//************ Rotate Accumulator Instructions *************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_rrc(void)
{
    /* Carry bit - low order bit of accumulator */
    m_flagC = m_regA & 1;

    /* Right shift the accumulator */
    m_regA >>= 1;

    /* Set the highest bit of the accumulator to carry */
    m_regA |= m_flagC << 7;

    return 1;
}

int Cpu8080::op_rlc(void)
{
    /* Carry Bit = high order bit of accumulator */
    m_flagC = (m_regA >> 7) & 1;

    /* Left shift the accumulator */
    m_regA <<= 1;

    /* Set lowest bit of accumulator to carry */
    m_regA |= m_flagC & 1;
    
    return 1;
}

int Cpu8080::op_rar(void)
{
    /* Save the current carry bit */
    uint8_t curCarry = m_flagC & 1;

    /* Carry bit = low order bit of accumulator */
    m_flagC = m_regA & 1;

    /* Right Shift the accumulator */
    m_regA >>= 1;

    /* MSB of A = prev carry */
    m_regA |= curCarry << 7;

    return 1;
}

//////////////////////////////////////////////////////////////
//******* Register/Memory to Accumulator Instructions ******//
//////////////////////////////////////////////////////////////
int Cpu8080::op_add(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled ADD Register: 0x%X\n", reg);
            exit(0);
    }

    /* ADD */
    uint8_t res = m_regA + *regp;

    /* Set flags */
    m_flagC = (res < m_regA); // implies overflow
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented

    /* Store the result */
    m_regA = res;

    return 1;
}

int Cpu8080::op_adc(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled ADD Register: 0x%X\n", reg);
            exit(0);
    }

    /* ADD  + carry bit */
    uint8_t res = m_regA + *regp + m_flagC;

    /* Set flags */
    m_flagC = (res < m_regA); // implies overflow
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented

    /* Store the result */
    m_regA = res;

    return 1;
}

int Cpu8080::op_sub(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled ADD Register: 0x%X\n", reg);
            exit(0);
    }

    /* SUB */
    uint8_t res = m_regA - *regp;

    /* Set flags */
    m_flagC = (res < m_regA); // if there is NO carry out, the carry bit is SET; otherwise reset. this is the opposite of add operation)
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented

    /* Store the result */
    m_regA = res;

    return 1;
}

int Cpu8080::op_sbb(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled ADD Register: 0x%X\n", reg);
            exit(0);
    }

    /* SUB with carry flag */
    uint8_t res = m_regA - *regp - m_flagC;

    /* Set flags */
    m_flagC = (res < m_regA); // if there is NO carry out, the carry bit is SET; otherwise reset. this is the opposite of add operation)
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);
    // m_flagAC = // unimplemented

    /* Store the result */
    m_regA = res;

    return 1;
}


int Cpu8080::op_ana(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled ANA Register: 0x%X\n", reg);
            exit(0);
    }

    /* AND */
    uint8_t res = m_regA & *regp;

    /* Set flags */
    m_flagC = 0; // carry bit is reset to 0
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);

    /* Store the result in A */
    m_regA = res;

    return 1;
}

int Cpu8080::op_xra(void)
{
    /* Which register? */
    uint8_t reg = m_memory[m_pc] & 0x7;
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
            printf("  Unhandled XRA Register: 0x%X\n", reg);
            exit(0);
    }


    /* XOR */
    uint8_t res = m_regA ^ *regp;

    /* Set flags */
    m_flagC = 0; // carry bit is reset to 0
    m_flagZ = (res == 0);
    m_flagS = ((int8_t)res < 0);
    m_flagP = !get_odd_parity(res);
    //m_flagAC = // unimplemented

    /* Store the result in A */
    m_regA = res;

    return 1;
}


//////////////////////////////////////////////////////////////
//*********** Interrupt Flip-Flop Instructions *************//
//////////////////////////////////////////////////////////////
int Cpu8080::op_ei(void)
{
    m_interrupts = 1;

    return 1;
}



