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
#include <seq.h>

typedef struct {
        Seq_T mapped;
        Seq_T unmapped;
} *Segment_T;

/********** segment_init ********
 * 
 * Description: Initialize a new memory segment with the specified 
 *              number of words.
 *
 * Parameters:
 *      uint32_t num_words: Number of words for the new memory segment.
 *
 * Returns:
 *      Segment_T: A pointer to the newly initialized memory segment.
 *
 * Expects:
 *      - The parameter 'num_words' must be a valid unsigned 32-bit integer.
 *
 * Notes:
 *      - The caller is responsible for managing the memory allocated for 
 *        the new segment.
 *      - If 'num_words' is zero or invalid, the behavior is undefined.
 *      - The function allocates dynamic memory using malloc; the caller is 
 *        responsible for freeing it.
 **********************************************************************/
Segment_T segment_init(uint32_t num_words)
{
        Segment_T new_segments = malloc(sizeof(*new_segments));
        new_segments->mapped = Seq_new(1);
        new_segments->unmapped = Seq_new(1);
        Seq_addhi(new_segments->mapped, Seq_new(num_words));

        return new_segments;
}

/********** segment_deinit ********
 * Description: Deallocates memory and resources associated with a memory 
 *              segment.
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment to be deallocated.
 *
 * Expects:
 *      - The parameter 'segments' must not be NULL.
 *
 * Notes:
 *      - Will cause undefined behavior if 'segments' is NULL.
 *      - The function assumes that the memory segment and its sequences were 
 *        allocated using appropriate methods.
 *      - After calling this function, the 'segments' pointer becomes invalid 
 *        and should not be used.
 **********************************************************************/
void segment_deinit(Segment_T segments)
{
        uint32_t len = Seq_length(segments->mapped);
        for (uint32_t i = 0; i < len; i++) {
                Seq_T segment = Seq_get(segments->mapped, i);
                if (segment != NULL) {
                        Seq_free(&segment);
                }
        }

        Seq_free(&(segments->mapped));
        Seq_free(&(segments->unmapped));
        free(segments);
}

/********** segment_new ********
 * Description: Creates a new memory segment with the specified number of words 
 *            and returns the index of the new segment in the mapped sequence.
 *
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment structure.
 *      uint32_t num_words: Number of words for the new memory segment.
 *
 * Returns:
 *      uint32_t: Index of the new memory segment in the mapped sequence.
 *
 * Expects:
 *      - 'segments' must not be NULL.
 *      - 'num_words' must be a valid unsigned 32-bit integer.
 *
 * Notes:
 *      - Will cause a Checked Runtime Error (CRE) if 'segments' is NULL,
 *         'num_words' is zero, or an invalid index is returned.
 *      - Memory allocated by this function must be managed by the caller.
 *      - The returned index is the position of the new segment in the mapped 
 *        sequence.
 *      - If the unmapped sequence is empty, the new segment is added at the 
 *       end of the mapped sequence.
 **********************************************************************/
uint32_t segment_new(Segment_T segments, uint32_t num_words)
{
        Seq_T word_array = Seq_new(num_words);
        
        for (uint32_t i = 0; i < num_words; i++) {
                Seq_addhi(word_array, 0);
        }

        if (Seq_length(segments->unmapped) > 0) {
                uint32_t index = (uint32_t)(uintptr_t)
                                  Seq_remlo(segments->unmapped);
                Seq_put(segments->mapped, index, word_array);
                return index;
        } else {
                Seq_addhi(segments->mapped, word_array);
                /* returns index of last inserted element */
                return Seq_length(segments->mapped) - 1;
        }
}

/********** segment_free ********
 * Description: Deallocates memory and resources associated with a specific 
 *             memory segment identified by its index.
 *
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment structure.
 *      uint32_t segment_id: Index of the memory segment to be deallocated.
 *
 * Expects:
 *      - 'segments' must not be NULL.
 *      - 'segment_id' must be a valid index within the range of the mapped 
 *        sequence.
 *      - 'segment_id' must not be equal to 0.
 *
 * Notes:
 *      - Will cause a Checked Runtime Error (CRE) if 'segments' is NULL, 
 *        'segment_id' is 0, or an invalid index is provided.
 *      - The function assumes that the memory associated with the specified 
 *        segment was allocated using appropriate methods.
 *      - The unmapped sequence maintains a pool of available indices for 
 *         reuse.
 **********************************************************************/
void segment_free(Segment_T segments, uint32_t segment_id)
{
        Seq_T word_array = Seq_get(segments->mapped, segment_id);
        Seq_free(&word_array);
        Seq_put(segments->mapped, segment_id, NULL);
        Seq_addhi(segments->unmapped, (void*)(uintptr_t)segment_id);
}

/********** segment_store_word ********
 * Description: Stores a 32-bit value at the specified offset within a 
 *              memory segment.
 *
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment structure.
 *      uint32_t segment_id: Index of the memory segment where the value will 
 *                          be stored.
 *      uint32_t offset: Offset within the memory segment where the value will 
 *                       be stored.
 *      uint32_t value: 32-bit value to be stored in the memory segment.
 *
 * Expects:
 *      - 'segments' must not be NULL.
 *      - 'segment_id' must be a valid index within the range of the mapped 
 *         sequence.
 *      - 'offset' must be a valid index within the range of the specified 
 *         memory segment.
 * Notes:
 *      - Will cause a Checked Runtime Error (CRE) if 'segments' is NULL, 
 *         'segment_id' or 'offset' is out of bounds,
 *        or if an invalid 'value' is provided.
 *      - The function assumes that the memory segment and offset are within 
 *        valid bounds.
 **********************************************************************/
void segment_store_word(Segment_T segments, uint32_t segment_id,
                        uint32_t offset, uint32_t value)
{
        Seq_T word_array = Seq_get(segments->mapped, segment_id);
        Seq_put(word_array, offset, (void*)(uintptr_t)value);
}

/********** segment_get_word ********
 * Description: Retrieves a 32-bit value from the specified offset within a 
 *              memory segment.
 *
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment structure.
 *      uint32_t segment_id: Index of the memory segment from which the value 
 *                            will be retrieved.
 *      uint32_t offset: Offset within the memory segment where the value will 
 *                       be retrived.
 *
 * Returns:
 *      uint32_t: The 32-bit value retrieved from the specified offset within 
 *                the memory segment.
 *
 * Expects:
 *      - 'segments' must not be NULL.
 *      - 'segment_id' must be a valid index within the range of the mapped 
 *         sequence.
 *      - 'offset' must be a valid index within the range of the specified 
 *         memory segment.
 * Notes:
 *      - Will cause a Checked Runtime Error (CRE) if 'segments' is NULL, 
 *        'segment_id' or 'offset' is out of bounds,
 *        or if an invalid value is encountered during retrieval.
 *      - The function assumes that the memory segment, offset, and retrieved 
 *         value are within valid bounds.
 **********************************************************************/
uint32_t segment_get_word(Segment_T segments, uint32_t segment_id,
                          uint32_t offset)
{
        Seq_T word_array = Seq_get(segments->mapped, segment_id);
        return (uint32_t)(uintptr_t)Seq_get(word_array, offset);
}

/********** segment_duplicate ********
 * Description: Duplicates the content of a specified memory segment and 
 *              replaces the content of the first memory segment (index 0) 
 *               with the duplicated content.
 *
 * Parameters:
 *      Segment_T segments: Pointer to the memory segment structure.
 *      uint32_t segment_id: Index of the memory segment to be duplicated.
 *
 * Expects:
 *      - 'segments' must not be NULL.
 *      - 'segment_id' must be a valid index within the range of the mapped 
 *         sequence.
 * Notes:
 *      - Will cause a Checked Runtime Error (CRE) if 'segments' is NULL or if
 *        'segment_id' is out of bounds.
 *      - If 'segment_id' is 0, no duplication occurs, and the function has 
 *         no effect.
 *      - The function assumes that the memory segments are properly 
 *        initialized and allocated.
 **********************************************************************/
void segment_duplicate(Segment_T segments, uint32_t segment_id)
{
        if (segment_id != 0) {
                Seq_T segment = Seq_get(segments->mapped, 0);
                Seq_free(&segment);

                Seq_T word_array = Seq_get(segments->mapped, segment_id);
                uint32_t len = Seq_length(word_array);
                Seq_T program = Seq_new(len);

                for (uint32_t i = 0; i < len; i++) {
                        Seq_addhi(program, Seq_get(word_array, i));
                }
                
                Seq_put(segments->mapped, 0, program);
        }
}

typedef enum opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MULT, DIV, NAND,
        HALT, MAP, UNMAP, OUTPUT, INPUT, LOADP, LOADV
} opcode;

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

        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t prog_counter = 0;
        bool halted = false;

        /* iterate through instructions until a halt is read */
        while (!halted) {
                word = segment_get_word(segments, 0, prog_counter++);
                opcode op = get_opcode(word);
                
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
