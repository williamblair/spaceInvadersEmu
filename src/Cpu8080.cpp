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
        case 0x06:    num_increment = op_mvi_b();  break; // MVI B, D8
        case 0x11:    num_increment = op_lxi_d();  break; // LXI D, D16
        case 0x1A:    num_increment = op_ldax_d(); break; // LDAX D
        case 0x21:    num_increment = op_lxi_h();  break; // LXI H, D16

        case 0x31:    num_increment = op_lxi_sp(); break; // LXI SP, D16

        case 0x77:    num_increment = op_mov_ma(); break; // MOV M, A

        case 0xC3:    num_increment = op_jmp();      break; // JMP
        case 0xCD:    num_increment = op_call();     break; // CALL

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

int Cpu8080::op_unimplemented(void)
{
    printf("  ** UNIMPLEMENTED OP **\n");
    printf("  Op: 0x%02X\tPC: 0x%08X\n", m_memory[m_pc], m_pc);
    exit(0);
}

int Cpu8080::op_call(void)
{
    /* Save the NEXT instruction address */
    m_memory[m_sp-1] = ((m_pc+3) >> 8) & 0xFF;
    m_memory[m_sp-2] = (m_pc+3) & 0xFF;

    /* Increase the stack pointer */
    m_sp += 2;

    /* Move to the called address*/
    m_pc = read16(m_pc+1);

    /* Don't increment PC afterwords */
    return 0;
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

int Cpu8080::op_mov_ma(void)
{
    uint16_t addr = (m_regH << 8) | m_regL;
    m_memory[addr] = m_regA;

    return 1;
}




