/**************************************************************
 *                        um.c
 *
 *       Assignment: um
 *       Authors:    Saajid Islam (mislam08), Laila Ghabbour (lghabb01)
 *       Date:       11/20/2023
 *
 *       Summary:   Universal Machine main program, responsible for
 *                  interpreting UM instructions and executing them.
 * 
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <assert.h>
#include <bitpack.h>
#include <um-dis.h>
#include "segments.h"
#include "instructions.h"

uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint32_t prog_counter = 0;
bool halted = false;

/**********************************************************************
 * Description: Converts the endianness of a 32-bit unsigned integer.
 *
 * Parameters:
 *      uint32_t value: The 32-bit unsigned integer to be converted.
 *
 * Returns:
 *      uint32_t: 32-bit unsigned integer in big endian order (every 8 bits)
 *
 * Expects:
 *      - The 'value' parameter must be a valid 32-bit unsigned integer.
 * 
 **********************************************************************/
uint32_t convert_endian(uint32_t value) {
        return ((value & 0xFF) << 24) |
               (((value >> 8) & 0xFF) << 16) |
               (((value >> 16) & 0xFF) << 8) |
               ((value >> 24) & 0xFF);
}

int main(int argc, char *argv[])
{
        assert(argc == 2);
        
        /* obtaining file size */
        struct stat fileStat;
        size_t fileSize = 0;
        if (stat(argv[1], &fileStat) == 0) {
                /* file size in bits */
                fileSize = fileStat.st_size * 8;
        }
        
        /* opening file */
        FILE* inputFile = fopen(argv[1], "rb");
        if (inputFile == NULL) {
                printf("%s: No such file or directory\n", argv[1]);
                return EXIT_FAILURE;
        }

        Segment_T segments = segment_init(fileSize / 32);
        uint32_t word = 0;

        /* while not EOF, store bits in file as words */
        while (fread(&word, sizeof(uint32_t), 1, inputFile) == 1) {
                Seq_T word_array = Seq_get(segments->mapped, 0);
                Seq_addhi(word_array, (void*)(uintptr_t)convert_endian(word));
        }

        /* iterate through instructions until a halt is read */
        while (!halted) {
                word = segment_get_word(segments, 0, prog_counter++);
                opcode op = get_opcode(word);
                assert(op <= LOADV && op >= CMOV);
                
                switch (op) {
                case CMOV:
                        cond_move(&registers[regA(word)],
                                   registers[regB(word)],
                                   registers[regC(word)]);
                        break;
                case SLOAD:
                        registers[regA(word)] = segment_get_word(segments,
                                                registers[regB(word)],
                                                registers[regC(word)]);
                        break;
                case SSTORE:
                        segment_store_word(segments, registers[regA(word)],
                                                     registers[regB(word)],
                                                     registers[regC(word)]);
                        break;
                case ADD:
                        add(&registers[regA(word)],
                             registers[regB(word)],
                             registers[regC(word)]);
                        break;
                case MULT:
                        mult(&registers[regA(word)],
                              registers[regB(word)],
                              registers[regC(word)]);
                        break;
                case DIV:
                        divide(&registers[regA(word)],
                                registers[regB(word)],
                                registers[regC(word)]);
                        break;
                case NAND:
                        bit_NAND(&registers[regA(word)],
                                  registers[regB(word)],
                                  registers[regC(word)]);
                        break;
                case HALT:
                        halted = true;
                        break;
                case MAP:
                        registers[regB(word)] = segment_new(segments,
                                                registers[regC(word)]);
                        break;
                case UNMAP:
                        segment_free(segments, registers[regC(word)]);
                        break;
                case OUTPUT:
                        output(registers[regC(word)]);
                        break;
                case INPUT:
                        input(&registers[regC(word)]);
                        break;
                case LOADP:
                        segment_duplicate(segments, registers[regB(word)]);
                        prog_counter = registers[regC(word)];
                        break;
                case LOADV:
                        registers[Bitpack_getu(word, 3, 25)] = Bitpack_getu(word, 25, 0);
                        break;
                default:
                        break;
                }
        }

        fclose(inputFile);
        segment_deinit(segments);

        return EXIT_SUCCESS;
}
