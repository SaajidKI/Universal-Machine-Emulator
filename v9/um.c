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

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <assert.h>
#include <mem.h>
#include <string.h>

const uint32_t REG_A_MASK = 7 << 6;
const uint32_t REG_B_MASK = 7 << 3;
const uint32_t REG_C_MASK = 7;
const uint32_t OP_CODE_MASK = 15UL << 28;
const uint32_t LOADVAL_VALUE_MASK = (1UL << 25) - 1;
const uint32_t LOADVAL_REG_A_MASK = 7 << 25;

typedef enum opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MULT, DIV, NAND,
        HALT, MAP, UNMAP, OUTPUT, INPUT, LOADP, LOADV
} opcode;

typedef struct {
        uint32_t* data;
        uint32_t length;
} *Array_T;

typedef struct {
        Array_T* mapped;
        uint32_t mapped_length;
        Array_T unmapped;
        uint32_t unmapped_length;
} *Segment_T;

Array_T new_array(uint32_t length)
{
        Array_T array = malloc(sizeof(*array));
        array->data = malloc(sizeof(uint32_t) * length);
        array->length = length;
        return array;
}

inline void free_array(Array_T* array)
{
        free((*array)->data);
        free(*array);
}

Segment_T segment_init(uint32_t num_words)
{
        Segment_T new_segments = malloc(sizeof(*new_segments));
        
        new_segments->mapped = malloc(num_words * 8 * sizeof(Array_T));
        new_segments->mapped_length = 1;
        new_segments->unmapped = new_array(num_words * 8);
        new_segments->unmapped_length = 0;
        
        new_segments->mapped[0] = new_array(num_words);

        return new_segments;
}

void segment_deinit(Segment_T segments)
{
        for (uint32_t i = 0; i < segments->mapped_length; i++) {
                Array_T array = segments->mapped[i];
                if (array != NULL) {
                        free_array(&array);
                }
        }

        free(segments->mapped);
        free_array(&(segments->unmapped));
        free(segments);
}

uint32_t segment_new(Segment_T segments, uint32_t num_words)
{
        uint32_t id;
        if (segments->unmapped_length != 0) {
                id = segments->unmapped->data[segments->unmapped_length - 1];
                segments->unmapped_length--;
        } else {
                id = segments->mapped_length++;
        }

        segments->mapped[id] = new_array(num_words);
        memset(segments->mapped[id]->data, 0, sizeof(uint32_t) * num_words);
        
        return id;
}

void segment_free(Segment_T segments, uint32_t segment_id)
{
        Array_T array = segments->mapped[segment_id];
        free_array(&array);
        segments->mapped[segment_id] = NULL;
        segments->unmapped->data[segments->unmapped_length++] = segment_id;
}

inline void segment_store_word(Segment_T segments, uint32_t segment_id,
                        uint32_t offset, uint32_t value)
{
        segments->mapped[segment_id]->data[offset] = value;
}

inline uint32_t segment_get_word(Segment_T segments, uint32_t segment_id,
                          uint32_t offset)
{
        return segments->mapped[segment_id]->data[offset];
}

void segment_duplicate(Segment_T segments, uint32_t segment_id)
{
        Array_T segment = segments->mapped[0];
        free_array(&segment);

        Array_T word_array = segments->mapped[segment_id];
        uint32_t len = word_array->length;
        Array_T program = new_array(len);
        
        memcpy(program->data, word_array->data, sizeof(uint32_t) * len);

        segments->mapped[0] = program;
}

inline opcode get_opcode(uint32_t word)
{
        return (opcode)((word & OP_CODE_MASK) >> 28);
}

inline uint32_t regA(uint32_t word)
{
        return (word & REG_A_MASK) >> 6;
}

inline uint32_t regB(uint32_t word)
{
        return (word & REG_B_MASK) >> 3;
}

inline uint32_t regC(uint32_t word)
{
        return word & REG_C_MASK;
}

inline void cond_move(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        if (regC != 0) {
             *regA = regB;
        }
}

inline void add(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        *regA = (uint32_t)(regB + regC);
}

inline void mult(uint32_t *regA, uint32_t regB, uint32_t regC)
{
        *regA = (uint32_t)(regB * regC);
}

inline void divide(uint32_t *regA, uint32_t regB, uint32_t regC)
{
        *regA = (regB / regC);
}

inline void bit_NAND(uint32_t* regA, uint32_t regB, uint32_t regC)
{
        *regA = ~(regB & regC);
}

inline void output(uint32_t regC)
{
        printf("%c", regC);
}

inline void input(uint32_t* regC)
{
        uint32_t input = getchar();
        
        if (input <= 255) {
                *regC = input;
        } else {
                *regC = 0xFFFFFFFF;
        }
}

inline uint32_t convert_endian(uint32_t value) {
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
        static int i = 0;
        while (fread(&word, sizeof(uint32_t), 1, inputFile) == 1) {
                segments->mapped[0]->data[i++] = convert_endian(word);
        }
        fclose(inputFile);

        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t prog_counter = 0;
        bool halted = false;

        /* iterate through instructions until a halt is read */
        while (!halted) {
                word = segments->mapped[0]->data[prog_counter++];
                opcode op = get_opcode(word);
                
                switch (op) {
                case LOADV:
                        registers[(word & LOADVAL_REG_A_MASK) >> 25] = 
                                                word & LOADVAL_VALUE_MASK;
                        break;
                case OUTPUT:
                        output(registers[regC(word)]);
                        break;
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
                case NAND:
                        bit_NAND(&registers[regA(word)],
                                  registers[regB(word)],
                                  registers[regC(word)]);
                        break;
                case INPUT:
                        input(&registers[regC(word)]);
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
                case MAP:
                        registers[regB(word)] = segment_new(segments,
                                                registers[regC(word)]);
                        break;
                case UNMAP:
                        segment_free(segments, registers[regC(word)]);
                        break;
                case LOADP:
                        if (registers[regB(word)] != 0) {
                                segment_duplicate(segments,
                                                  registers[regB(word)]);
                        }
                        prog_counter = registers[regC(word)];
                        break;
                case HALT:
                        halted = true;
                        break;
                default:
                        break;
                }
        }

        segment_deinit(segments);

        return EXIT_SUCCESS;
}
