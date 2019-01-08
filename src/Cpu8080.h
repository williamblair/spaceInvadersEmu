/*
 * Cpu8080.h
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

#ifndef CPU8080_H_INCLUDED
#define CPU8080_H_INCLUDED

/* ROM data */
#include "../data/romE.h"
#include "../data/romF.h"
#include "../data/romG.h"
#include "../data/romH.h"

#define ROM_SIZE  0x2000    // 0x0000 - 0x1FFF 8K ROM
#define RAM_SIZE  0x400        // 0x2000 - 0x2eFF 1K RAM
#define VRAM_SIZE 0x1C00    // 0x2400 - 0x3FFF 7K VRAM

/* Info From http://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html */
#define ROM_H_START 0x0000
#define ROM_G_START 0x0800
#define ROM_F_START 0x1000
#define ROM_E_START 0x1800

/* Op lookup table for info/logging */
static const char *op_lookup[] = {
    "NOP",
    "LXI B,D",
    "STAX B",
    "INX B",
    "INR B",
    "DCR B",
    "MVI B, D",
    "RLC",
    "-",
    "DAD B",
    "LDAX B",
    "DCX B",
    "INR C",
    "DCR C",
    "MVI C,D",
    "RRC",
    "-",
    "LXI D,D",
    "STAX D",
    "INX D",
    "INR D",
    "DCR D",
    "MVI D, D",
    "RAL",
    "-",
    "DAD D",
    "LDAX D",
    "DCX D",
    "INR E",
    "DCR E",
    "MVI E,D",
    "RAR",
    "-",
    "LXI H,D",
    "SHLD adr",
    "INX H",
    "INR H",
    "DCR H",
    "MVI H,D",
    "DAA",
    "-",
    "DAD H",
    "LHLD adr",
    "DCX H",
    "INR L",
    "DCR L",
    "MVI L, D",
    "CMA",
    "-",
    "LXI SP, D",
    "STA adr",
    "INX SP",
    "INR M",
    "DCR M",
    "MVI M,D",
    "STC",
    "-",
    "DAD SP",
    "LDA adr",
    "DCX SP",
    "INR A",
    "DCR A",
    "MVI A,D",
    "CMC",
    "MOV B,B",
    "MOV B,C",
    "MOV B,D",
    "MOV B,E",
    "MOV B,H",
    "MOV B,L",
    "MOV B,M",
    "MOV B,A",
    "MOV C,B",
    "MOV C,C",
    "MOV C,D",
    "MOV C,E",
    "MOV C,H",
    "MOV C,L",
    "MOV C,M",
    "MOV C,A",
    "MOV D,B",
    "MOV D,C",
    "MOV D,D",
    "MOV D,E",
    "MOV D,H",
    "MOV D,L",
    "MOV D,M",
    "MOV D,A",
    "MOV E,B",
    "MOV E,C",
    "MOV E,D",
    "MOV E,E",
    "MOV E,H",
    "MOV E,L",
    "MOV E,M",
    "MOV E,A",
    "MOV H,B",
    "MOV H,C",
    "MOV H,D",
    "MOV H,E",
    "MOV H,H",
    "MOV H,L",
    "MOV H,M",
    "MOV H,A",
    "MOV L,B",
    "MOV L,C",
    "MOV L,D",
    "MOV L,E",
    "MOV L,H",
    "MOV L,L",
    "MOV L,M",
    "MOV L,A",
    "MOV M,B",
    "MOV M,C",
    "MOV M,D",
    "MOV M,E",
    "MOV M,H",
    "MOV M,L",
    "HLT",
    "MOV M,A",
    "MOV A,B",
    "MOV A,C",
    "MOV A,D",
    "MOV A,E",
    "MOV A,H",
    "MOV A,L",
    "MOV A,M",
    "MOV A,A",
    "ADD B",
    "ADD C",
    "ADD D",
    "ADD E",
    "ADD H",
    "ADD L",
    "ADD M",
    "ADD A",
    "ADC B",
    "ADC C",
    "ADC D",
    "ADC E",
    "ADC H",
    "ADC L",
    "ADC M",
    "ADC A",
    "SUB B",
    "SUB C",
    "SUB D",
    "SUB E",
    "SUB H",
    "SUB L",
    "SUB M",
    "SUB A",
    "SBB B",
    "SBB C",
    "SBB D",
    "SBB E",
    "SBB H",
    "SBB L",
    "SBB M",
    "SBB A",
    "ANA B",
    "ANA C",
    "ANA D",
    "ANA E",
    "ANA H",
    "ANA L",
    "ANA M",
    "ANA A",
    "XRA B",
    "XRA C",
    "XRA D",
    "XRA E",
    "XRA H",
    "XRA L",
    "XRA M",
    "XRA A",
    "ORA B",
    "ORA C",
    "ORA D",
    "ORA E",
    "ORA H",
    "ORA L",
    "ORA M",
    "ORA A",
    "CMP B",
    "CMP C",
    "CMP D",
    "CMP E",
    "CMP H",
    "CMP L",
    "CMP M",
    "CMP A",
    "RNZ",
    "POP B",
    "JNZ adr",
    "JMP adr",
    "CNZ adr",
    "PUSH B",
    "ADI D",
    "RST",
    "RZ",
    "RET",
    "JZ adr",
    "-",
    "CZ adr",
    "CALL adr",
    "ACI D",
    "RST",
    "RNC",
    "POP D",
    "JNC adr",
    "OUT D",
    "CNC adr",
    "PUSH D",
    "SUI D",
    "RST",
    "RC",
    "-",
    "JC adr",
    "IN D",
    "CC adr",
    "-",
    "SBI D",
    "RST",
    "RPO",
    "POP H",
    "JPO adr",
    "XTHL",
    "CPO adr",
    "PUSH H",
    "ANI D",
    "RST",
    "RPE",
    "PCHL",
    "JPE adr",
    "XCHG",
    "CPE adr",
    "-",
    "XRI D",
    "RST",
    "RP",
    "POP PSW",
    "JP adr",
    "DI",
    "CP adr",
    "PUSH PSW",
    "ORI D",
    "RST",
    "RM",
    "SPHL",
    "JM adr",
    "EI",
    "CM adr",
    "-",
    "CPI D",
    "RST"
};

class Cpu8080 {

public:
    Cpu8080();
    virtual ~Cpu8080();

    bool init(uint8_t *memory, uint8_t *ports);

    void run_next_op(void);

// uncommented to test for now
//private:

    /* Program Counter */
    uint16_t m_pc;

    /* Stack Pointer */
    uint16_t m_sp;

    /* Registers */
    uint8_t m_regA; uint8_t m_regPSW;
    uint8_t m_regB; uint8_t m_regC;   // called 'B' as a 16 bit pair
    uint8_t m_regD; uint8_t m_regE;   // called 'D' as a 16 bit pair
    uint8_t m_regH; uint8_t m_regL;   // called 'H' as a 16 bit pair

    /* Shift Register */
    uint16_t m_regShift;

    /* Result shift for reading from port 3 / writing to port2 */
    uint8_t m_shiftAmount;

    /* Keyboard Port */
    uint8_t m_port1;

    /* Flags (Status Register), technically should be a single char but this is easier */
    uint8_t m_flagS;
    uint8_t m_flagZ;
    uint8_t m_flagP;
    uint8_t m_flagC;
    uint8_t m_flagAC; // this is reportedly unused by space invaders so we won't bother

    /* Interrupt flip-flop flag */
    uint8_t m_interrupts;


    /* Pointer to system Memory */
    uint8_t *m_memory;

    /* Pointer to system ports */
    uint8_t *m_ports;

    /*
     * Helper functions
     */
    uint16_t read16(uint16_t addr); // reads two bytes, converting from little endian, from memory at the specified addr

    /* Returns 1 if the number of 1s is odd
     * from https://www.microchip.com/forums/m587239.aspx */
    uint8_t get_odd_parity(uint8_t num);

    /* Sets the Zero, Sign, and Parity flags based on the before and after result */
    void set_zsp_flags(uint8_t prev, uint8_t res);

    /* Sets the carry flag if the addition of the two inputs will overflow, otherwise
     * sets to flagC to 0 */
    void set_c_16(uint16_t num1, uint16_t num2);

    /* Sets flags after logical operations, e.g. AND */
    void set_flags_logical(uint8_t res);

    /* Dummy function */
    int op_unimplemented(void);

    /*
     * Interrupts
     */
    void run_interrupt(int num); // num = which interrupt to run


//////////////////////////////////////////////////////////////
//                      Operations                          //
//                                                          //
//    Operations should return the number of bytes used by  //
//    that instruction, which is used to increment the      //
//      program counter                                     //
//                                                          //
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//******************* JUMP Instructions ********************//
//////////////////////////////////////////////////////////////
    int op_jmp(void);
    int op_jnz(void);

//////////////////////////////////////////////////////////////
//**************** Immediate Instructions ******************//
//////////////////////////////////////////////////////////////
    int op_lxi(void);
    int op_mvi(void);
    int op_cpi(void);
    int op_ani(void);
    int op_ori(void);

//////////////////////////////////////////////////////////////
//****************** Call Instructions *********************//
//////////////////////////////////////////////////////////////
    int op_call(void);
    int op_cnz(void);

//////////////////////////////////////////////////////////////
//************** Data Transfer Instructions ****************//
//////////////////////////////////////////////////////////////
    int op_ldax(void);
    int op_mov  (void);

//////////////////////////////////////////////////////////////
//************** Register Pair Instructions ****************//
//////////////////////////////////////////////////////////////
    int op_inx(void);

    int op_push(void);
    int op_pop (void);

    int op_dad (void);

    int op_xchg(void);

//////////////////////////////////////////////////////////////
//************* Single Register Instructions ***************//
//////////////////////////////////////////////////////////////
    int op_dcr(void);
    int op_inr(void);

//////////////////////////////////////////////////////////////
//********** Return from Subroutine Instructions ***********//
//////////////////////////////////////////////////////////////
    int op_ret(void);

//////////////////////////////////////////////////////////////
//************* Direct Addressing Instructions *************//
//////////////////////////////////////////////////////////////
    int op_sta(void);
    int op_lhld(void);

//////////////////////////////////////////////////////////////
//************** Input/Output Instructions *****************//
//////////////////////////////////////////////////////////////
    int op_out(void);

//////////////////////////////////////////////////////////////
//************ Rotate Accumulator Instructions *************//
//////////////////////////////////////////////////////////////
    int op_rlc(void);
    int op_rar(void);

//////////////////////////////////////////////////////////////
//******* Register/Memory to Accumulator Instructions ******//
//////////////////////////////////////////////////////////////
    int op_ana(void);

};

#endif /* SRC_CPU8080_H_ */



