/**************************************************************
 *                        segments.h
 *
 *       Assignment: um
 *       Authors:    Saajid Islam (mislam08), Laila Ghabbour (lghabb01)
 *       Date:       11/20/2023
 *
 *       Summary:   Header file for memory segment functions, including
 *                  initialization, deallocation, creation, and manipulation
 *                  of memory segments.
 *         
 **************************************************************/

#include <seq.h>

/**************************************************************
 * The Segment_T struct consists of two components:
 *      - mapped: A sequence containing mapped memory segments (sequences).
 *      - unmapped: A sequence containing indices of unmapped segments.
 *************************************************************/
typedef struct {
        Seq_T mapped;
        Seq_T unmapped;
} *Segment_T;

Segment_T segment_init(uint32_t num_words);

void segment_deinit(Segment_T segments);

uint32_t segment_new(Segment_T segments, uint32_t num_words);

void segment_free(Segment_T segments, uint32_t segment_id);

void segment_store_word(Segment_T segments, uint32_t segment_id,
                        uint32_t offset, uint32_t value);

uint32_t segment_get_word(Segment_T segments, uint32_t segment_id,
                          uint32_t offset);

void segment_duplicate(Segment_T segments, uint32_t segment_id);
