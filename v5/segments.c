/**************************************************************
 *                        segments.c
 *
 *       Assignment: um
 *       Authors:    Saajid Islam (mislam08), Laila Ghabbour (lghabb01)
 *       Date:       11/20/2023
 *
 *       Summary:   Contains functions for managing memory segments,
 *                  including initialization, deallocation, creation,
 *                  and manipulation of memory segments.
 *         
 **************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "segments.h"

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
