/**************************************************************
 *                        instructions.c
 *
 *       Assignment: um
 *       Authors:    Saajid Islam (mislam08), Laila Ghabbour (lghabb01)
 *       Date:       11/20/2023
 *
 *       Summary: Contains functions for extracting opcode and register
 *                values from 32-bit words, as well as implementing various
 *                Universal Machine instructions such as conditional move,
 *                arithmetic operations, bitwise NAND, output, and input.
 * 
 **************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "segments.h"
#include "instructions.h"

static inline uint32_t shr(uint32_t word, unsigned bits)
{
        if (bits == 32) {
                return 0;
        } else {
                return word >> bits;
        }
}

static inline uint32_t shl(uint32_t word, unsigned bits)
{
        if (bits == 32) {
                return 0;
        } else {
                return word << bits;
        }
}

uint32_t Bitpack_getu(uint32_t word, unsigned width, unsigned lsb)
{
        return shr(shl(word, 32 - (lsb + width)), 32 - width); 
}

/**********************************************************************
 * Description: Extracts and returns the opcode from the given 32-bit word.
 *
 * Parameters:
 *      uint32_t word: The 32-bit word from which the opcode is to be 
 *       extracted.
 *
 * Returns:
 *      opcode: The extracted opcode from the specified field in the word.
 *
 * Expects:
 *      - The 'word' parameter must be a valid 32-bit unsigned integer.
 *
 * Notes:
 *      - Will cause undefined behavior if the 'word' parameter is invalid or 
 *        does not contain the specified field.
 **********************************************************************/
opcode get_opcode(uint32_t word)
{
        return (opcode)Bitpack_getu(word, 4, 28);
}

/*********************************************************************
 * Description: Extracts and returns the value of the register A field 
 *              from the given 32-bit word based on the opcode.
 *
 * Parameters:
 *      uint32_t word: The 32-bit word from which the value is to be extracted.
 *
 * Returns:
 *      uint32_t: The extracted value from the specified register A field in 
 *                the word.
 *
 * Expects:
 *      - The 'word' parameter must be a valid 32-bit unsigned integer.
 *
 * Notes:
 *      - The function assumes the presence of a valid opcode in the 'word'.
 *      - Will cause undefined behavior if the 'word' parameter is invalid or 
 *        does not contain the specified fields.
 **********************************************************************/
uint32_t regA(uint32_t word)
{
        return Bitpack_getu(word, 3, 6);
}

/**********************************************************************
 * Description: Extracts and returns the value of the field at position 3 with 
 *              a width of 3 bits from the given 32-bit word.
 *
 * Parameters:
 *      uint32_t word: The 32-bit word from which the value is to be extracted.
 *
 * Returns:
 *      uint32_t: The extracted value from the specified field in the word.
 *
 * Expects:
 *      - The 'word' parameter must be a valid 32-bit unsigned integer.
 *
 * Notes:
 *      - The function assumes that the field at position 3 with a width of 
 *       3 bits is present in the 'word'.
 *      - Will cause undefined behavior if the 'word' parameter is invalid or 
 *        does not contain the specified field.
 **********************************************************************/
uint32_t regB(uint32_t word)
{
        return Bitpack_getu(word, 3, 3);
}

/**********************************************************************
 * Description: Extracts and returns the value of the field at position 0 with 
 *              a width of 3 bits from the given 32-bit word.
 *
 * Parameters:
 *      uint32_t word: The 32-bit word from which the value is to be extracted.
 *
 * Returns:
 *      uint32_t: The extracted value from the specified field in the word.
 *
 * Expects:
 *      - The 'word' parameter must be a valid 32-bit unsigned integer.
 *
 * Notes:
 *      - The function assumes that the field at position 0 with a width of 
 *       3 bits is present in the 'word'.
 *      - Will cause undefined behavior if the 'word' parameter is invalid or 
 *        does not contain the specified field.
 **********************************************************************/
uint32_t regC(uint32_t word)
{
        return Bitpack_getu(word, 3, 0);
}

/**********************************************************************
 * Description: If the value in register C is not equal to zero, set the value
 *              in register A to the value in register B.
 *
 * Parameters:
 *      uint32_t *regA: Address of the register A.
 *      uint32_t regB: Value of register B.
 *      uint32_t regC: Value of register C.
 *
 * Expects: regA, regB, and regC must not be NULL.
 *
 * Notes:
 *      Updates the value in register A based on the condition.
 *      Will CRE if regA, regB, or regC is NULL.
 **********************************************************************/
void cond_move(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        if (regC != 0) {
             *regA = regB;
        }
}

/**********************************************************************
 * Description: Compute the sum/product/floor division of the values in
 *              registers B and C, and store the result in register A.
 *
 * Parameters:
 *      uint32_t *regA: Address of the register A.
 *      uint32_t regB: Value of register B.
 *      uint32_t regC: Value of register C.
 *
 * Expects: regA, regB, and regC must not be NULL.
 *
 * Notes:
 *      Will CRE if regA, regB, or regC is NULL.
 **********************************************************************/
void add(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        *regA = (uint32_t)(regB + regC);
}

void mult(uint32_t *regA, uint32_t regB, uint32_t regC)
{
        *regA = (uint32_t)(regB * regC);
}

void divide(uint32_t *regA, uint32_t regB, uint32_t regC)
{
        *regA = (regB / regC);
}

/**********************************************************************
 * Description: Compute the bitwise NAND of the values in registers B and C,
 *              and store the result in register A.
 *
 * Parameters:
 *      uint32_t *regA: Address of the register A.
 *      uint32_t regB: Value of register B.
 *      uint32_t regC: Value of register C.
 *
 * Expects: regA, regB, and regC must not be NULL.
 *
 * Notes:
 *      Will CRE if regA, regB, or regC is NULL.
 **********************************************************************/
void bit_NAND(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        *regA = ~(regB & regC);
}

/**********************************************************************
 * Description: Writes the value in regC to the I/O device immediately.
 *              Only values from 0 to 255 are allowed.
 *
 * Parameters:
 *      uint32_t regC: Value of register C.
 *
 * Expects: None
 *
 * Notes:
 *      Will CRE if the value in regC is greater than 255.
 **********************************************************************/
void output(uint32_t regC)
{
        printf("%c", regC);
}

/**********************************************************************
 * Description: Waits for input on the I/O device. When input arrives, stores 
 *              the input in regC, which must be a value from 0 to 255. If the
 *              end of input has been signaled, stores a full 32-bit word in
 *              regC in which every bit is 1.
 *
 * Parameters:
 *      uint32_t *regC: Address of register C.
 *
 * Expects: regC must not be NULL.
 *
 * Notes:
 *      Waits for input on the I/O device and stores the input in regC.
 *      Will CRE if regC is NULL.
 **********************************************************************/
void input(uint32_t* regC)
{
        uint32_t input = getchar();
        
        if (input <= 255) {
                *regC = input;
        } else {
                *regC = 0xFFFFFFFF;
        }
}
