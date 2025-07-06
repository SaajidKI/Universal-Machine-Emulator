/**************************************************************
 *                        instructions.h
 *
 *       Assignment: um
 *       Authors:    Saajid Islam (mislam08), Laila Ghabbour (lghabb01)
 *       Date:       11/20/2023
 *
 *       Summary: Header file for the Universal Machine instructions. Defines
 *                an enumeration for opcodes and declares functions for
 *                extracting opcode and register values from 32-bit words.
 * 
 **************************************************************/

typedef enum opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MULT, DIV, NAND,
        HALT, MAP, UNMAP, OUTPUT, INPUT, LOADP, LOADV
} opcode;

uint32_t Bitpack_getu(uint32_t word, unsigned width, unsigned lsb);

opcode get_opcode(uint32_t word);

uint32_t regA(uint32_t word);

uint32_t regB(uint32_t word);

uint32_t regC(uint32_t word);

void cond_move(uint32_t* regA, uint32_t regB, uint32_t regC);

void add(uint32_t* regA, uint32_t regB, uint32_t regC);

void mult(uint32_t* regA, uint32_t regB, uint32_t regC);

void divide(uint32_t* regA, uint32_t regB, uint32_t regC);

void bit_NAND(uint32_t* regA, uint32_t regB, uint32_t regC);

void output(uint32_t regC);

void input(uint32_t* regC);
