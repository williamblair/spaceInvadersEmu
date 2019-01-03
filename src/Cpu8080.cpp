/*
 * Cpu8080.cpp
 */

#include "Cpu8080.h"
#include <cinttypes>

Cpu8080::Cpu8080() {
    m_pc = 0;
    m_sp = 2; // stack pointer is used by accessing the two indices below it

    m_regA = 0;
    m_regB = 0;
    m_regD = 0; m_regE = 0;
    m_regH = 0; m_regL = 0;

    m_flagS = 0;
    m_flagZ = 0;
    m_flagP = 0;
    m_flagC = 0;
    m_flagAC = 0;

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

    switch (opcode)
    {
        case 0x00:    num_increment = 1;           break; // NOP
        case 0x05:    num_increment = op_dcr_b();  break; // DCR B
        case 0x06:    num_increment = op_mvi_b();  break; // MVI B, D8
        case 0x11:    num_increment = op_lxi_d();  break; // LXI D, D16
        case 0x13:    num_increment = op_inx_d();  break; // INX D
        case 0x1A:    num_increment = op_ldax_d(); break; // LDAX D
        case 0x21:    num_increment = op_lxi_h();  break; // LXI H, D16
        case 0x23:    num_increment = op_inx_h();  break; // INX H

        case 0x31:    num_increment = op_lxi_sp(); break; // LXI SP, D16
        case 0x36:    num_increment = op_mvi_m();  break; // MVI M, D8

        case 0x77:    num_increment = op_mov_ma(); break; // MOV M, A
        case 0x7C:    num_increment = op_mov_ah(); break; // MOV H, A

        case 0xC2:    num_increment = op_jnz();    break; // JNZ
        case 0xC3:    num_increment = op_jmp();    break; // JMP
        case 0xC9:    num_increment = op_ret();    break; // RET
        case 0xCD:    num_increment = op_call();   break; // CALL

        case 0xFE:    num_increment = op_cpi();    break; // CPI

        /* Unhandled opcode, exit */
        default:    op_unimplemented(); break;
    }

    m_pc += num_increment;

    printf("PC: 0x%04X\n", m_pc);

    return;
}

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

//////////////////////////////////////////////////////////////
//*************** Subtraction Operations *******************//
//////////////////////////////////////////////////////////////

// 0x05     DCR B   1   Z, S, P, AC     B <- B-1
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

int Cpu8080::op_mvi_b(void)
{
    m_regB = m_memory[m_pc+1];

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

int Cpu8080::op_mov_ah(void)
{
    m_regA = m_regH;

    return 1;
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




