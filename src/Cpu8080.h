/*
 * Cpu8080.h
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

#ifndef SRC_CPU8080_H_
#define SRC_CPU8080_H_

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

class Cpu8080 {

public:
    Cpu8080();
    virtual ~Cpu8080();

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

    /* Result shift for reading/writing to/from port 3 */
    uint8_t m_port3_res;

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


    /* Memory */
    uint8_t m_memory[ROM_SIZE + RAM_SIZE + VRAM_SIZE];

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

    int op_call(void);
    int op_ret (void);

    int op_push_psw(void);
    int op_push_b(void);
    int op_push_d(void);
    int op_push_h(void);

    int op_pop_b(void);
    int op_pop_d(void);
    int op_pop_h(void);
    int op_pop_psw(void);

    int op_xchg(void);

    int op_ei(void);

//////////////////////////////////////////////////////////////
//***************** Addition Operations ********************//
//////////////////////////////////////////////////////////////

    int op_inx_d(void);
    int op_inx_h(void);

    int do_dad(uint16_t num); // runs DAD num, returns how much to increment PC by
    int op_dad_b(void);
    int op_dad_d(void);
    int op_dad_h(void);

    int op_adi(void);

//////////////////////////////////////////////////////////////
//*************** Subtraction Operations *******************//
//////////////////////////////////////////////////////////////

    int op_dcr_b(void);
    int op_dcr_c(void);

//////////////////////////////////////////////////////////////
//**************** Branching Operations ********************//
//////////////////////////////////////////////////////////////
    int op_jmp(void);
    int op_jnz(void);

//////////////////////////////////////////////////////////////
//****************** Memory Operations *********************//
//////////////////////////////////////////////////////////////
    int op_lxi_sp(void);
    int op_lxi_b (void);
    int op_lxi_d (void);
    int op_lxi_h (void);

    int op_ldax_d(void);
    int op_lda   (void);

    int op_mvi_a (void);
    int op_mvi_b (void);
    int op_mvi_c (void);
    int op_mvi_h (void);
    int op_mvi_m (void);

    int op_mov_ad(void);
    int op_mov_ae(void);

    int op_mov_ma(void);
    int op_mov_ha(void);
    int op_mov_la(void);

    int op_mov_am(void);
    int op_mov_dm(void);
    int op_mov_em(void);
    int op_mov_hm(void);

    int op_sta   (void);

//////////////////////////////////////////////////////////////
//*************** Conditional Operations *******************//
//////////////////////////////////////////////////////////////
    int op_cpi(void);

//////////////////////////////////////////////////////////////
//*************** Input/Output Operations ******************//
//////////////////////////////////////////////////////////////
    int op_out(void);

//////////////////////////////////////////////////////////////
//******************* Shift Operations *********************//
//////////////////////////////////////////////////////////////
    int op_rrc_a(void);

//////////////////////////////////////////////////////////////
//******************** AND Operations **********************//
//////////////////////////////////////////////////////////////
    int op_ani(void);

    int op_ana_a(void);

//////////////////////////////////////////////////////////////
//******************** XOR Operations **********************//
//////////////////////////////////////////////////////////////
    int op_xra_a(void);

};

#endif /* SRC_CPU8080_H_ */
