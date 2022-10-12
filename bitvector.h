/**
 * @file bitvector.h
 * @author Erastus Murungi (erastusmurungi@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef POPPY_BITVECTOR_H
#define POPPY_BITVECTOR_H

#endif // POPPY_BITVECTOR_H

#include <stdlib.h>
#include <stdbool.h>

#define ALL_ONES_MASK (0xffffffffffffffffUL)
#define BIT (1UL)
#define WORD_SIZE (64)
#define LOG_WORD_SIZE (6UL)

#define GET_FAILURE_TEXT(s) "\x1b[31m" s "\033[m"
#define GET_SUCCESS_TEXT(s) "\x1b[32m" s "\033[m"

#define BV_REPORT_ERROR_AND_EXIT(EXP, FILE_NAME, FUNCTION, LINE, ...)                   \
  do {                                                                         \
    fprintf(stderr, "%s:%d\n\t%s\n ", FILE_NAME, LINE, FUNCTION);                            \
    fprintf(stderr, GET_FAILURE_TEXT("\t\terror: "));                              \
    fprintf(stderr, "" #EXP " \n\t\t");                                            \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "\n");                                                     \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

/**
 * @brief A bit vector / bit array is composed of 
 * 1) an array of words
 * 2) the number of bits specified by the user
 * 3) the number of bits allocated
 * 
 * @note allocated >= size
 * 
 */
typedef struct
{
    uint64_t *data; // a pointer to words
    uint64_t size; // the number of bits in the bit array
    uint64_t allocated; // how many bits were allocated. Must be a multiple of the word size
} bitvector;

/**
 * @brief creates a new bit vector of the requested size
 * 
 * @return bitvector* a new bitvector with the requested size
 */
bitvector *bv_new(size_t);


/**
 * @brief Free up a bitvector from memory
 * 
 */
void bv_free(bitvector *);

uint64_t bv_hamming_distance(bitvector *, bitvector *);

bitvector *bv_xor(bitvector *, bitvector *);

bitvector *bv_complement(bitvector *);

bitvector *bv_intersection(bitvector *, bitvector *);

/**
 * @brief Check for value equality between to bitvectors
 * 
 * @return true if the bitvector values are equal
 * @return false if the bitvector values are not equal
 */
bool bv_equal(bitvector *, bitvector *);

/**
 * @brief Check for reference quality of two bitvectors
 * 
 * @return true if the two bitvector point to the same object in memory
 * @return false if they don't point to the same object in memory
 */
bool bv_is(bitvector *, bitvector *);

/**
 * @brief Get the number of bits in the bitvector
 * Works by simply acccessing the `size` field of the bitvector
 * 
 * @return size_t 
 */
size_t bv_len(bitvector *);

void bv_resize(bitvector *, size_t);

void bv_set(bitvector *, uint64_t pos);

void bv_range_set(bitvector *bv, uint8_t bit, size_t from, size_t to);

/**
 * @brief Set the bit at `pos` to `0`
 * 
 * @param bv a nonnull bitvector
 * @param pos the index of the bit to clear
 */
void bv_clear(bitvector *bv, uint64_t pos);

bool bv_toggle(bitvector *bv, uint64_t pos);

/**
 * @brief Checks that a index to a bitvector is within range, i,e 0 <= pos < bv_len(bv)
 * 
 * @param bv a nonnull bitvector
 * @param pos the index to check
 */
void bv_check_index(bitvector *bv, uint64_t pos);

/**
 * @brief Counts the number of set bits up to pos
 * 
 * @param bv a nonnull bitvector
 * @param pos last (exclusive) index to consider
 * @return uint64_t the number of set bits in bv[:pos]
 */
uint64_t bv_pop_count(bitvector *bv, uint64_t pos);

/**
 * @brief Check if the bit at position `pos` is set
 * 
 * @param bv a nonnull bitvector
 * @param pos the target position.
 * @return true if the bit at index `pos` is set
 * @return false otherwise
 */
bool bv_isset(bitvector *bv, uint64_t pos);

bitvector *bv_copy(bitvector *bv);

bitvector *bv_extend(bitvector *, bitvector *);

bitvector *bv_rshift(bitvector *, bitvector *);

bitvector *bv_lshift(bitvector *, bitvector *);

bitvector *bv_rotate(bitvector *);

bitvector *bv_reverse(bitvector *);

void bv_print(bitvector *);

void bv_print_dec(bitvector *);

int64_t bv_select(bitvector *, uint64_t);



void word_bin_rep(char *string, uint64_t x, size_t nx);

void print_string_as_array(char *string, size_t size);

void string_prepend(char *super_string, char *sub_string, uint64_t nsuper, uint64_t nsub);

void string_reverse(char *string, size_t ns);



uint64_t reverse_bits(uint64_t x);

uint64_t msb(uint64_t v);
