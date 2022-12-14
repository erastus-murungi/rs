#include <stdlib.h>
#include "bitvector.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

static uint64_t bv_get_block(bitvector *bv, uint64_t pos);

/**
 * @brief
 *
 * @param x the WORD_SIZE-bit integer whose kth bit we should set
 * @param k the position which is 0 <= k < 64
 * @return uint64_t
 */
static inline uint64_t word_set(uint64_t x, uint8_t k)
{
    return x | (BIT << k);
}

static inline uint64_t word_clear(uint64_t x, uint8_t k)
{
    return x & ~(BIT << k);
}

static inline uint64_t word_isset(uint64_t x, uint8_t k)
{
    return (x & (BIT << k));
}

static inline uint64_t word_toggle(uint64_t x, uint8_t k)
{
    return x ^ (BIT << k);
}

static inline uint64_t popcnt(uint64_t i)
{
    return __builtin_popcountll(i);
}

#define BV_CHECK_NONNULL(bv)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (bv == NULL)                                                                                                \
        {                                                                                                              \
            BV_REPORT_ERROR_AND_EXIT(bv == NULL, __FILE__, __PRETTY_FUNCTION__, __LINE__, "null pointer encountered"); \
        }                                                                                                              \
    } while (0)

bitvector *bv_new(size_t size)
{
    const uint64_t num_ints = (size >> LOG_WORD_SIZE) + BIT;
    uint64_t *data = calloc(num_ints, WORD_SIZE);
    if (data == NULL)
    {
        BV_REPORT_ERROR_AND_EXIT(data == NULL, __FILE__, __PRETTY_FUNCTION__, __LINE__, "could not allocate data array");
    }

    bitvector *bv = malloc(sizeof(bitvector));
    if (bv == NULL)
    {
        BV_REPORT_ERROR_AND_EXIT(bv == NULL, __FILE__, __PRETTY_FUNCTION__, __LINE__, "could not allocate bitvector");
    }
    *bv = (bitvector){
        data, size, (num_ints) * (BIT << LOG_WORD_SIZE)};
    return bv;
}

void bv_print(bitvector *bv)
{
    /** get the string representation of a bit vector*/

    char *super_string = malloc(sizeof(char) * ((WORD_SIZE << BIT) + BIT));
    char *sub_string = malloc(sizeof(char) * (WORD_SIZE + BIT));
    uint64_t block_count, remainder_size, nsuper;

    nsuper = 0;
    block_count = bv->allocated / WORD_SIZE;
    remainder_size = (bv_len(bv) % WORD_SIZE);

    /** get the binary representation of only the first block*/
    if (bv_len(bv) >= WORD_SIZE)
    {
        super_string[0] = '\0';
        word_bin_rep(sub_string, bv->data[0], WORD_SIZE);
        string_prepend(super_string, sub_string, 0 * WORD_SIZE, WORD_SIZE);
        nsuper += WORD_SIZE;
    }

    if (remainder_size > 0)
    {
        word_bin_rep(sub_string, bv->data[block_count - 1], remainder_size);
        string_prepend(super_string, sub_string, nsuper, remainder_size);
        nsuper += remainder_size;
    }
    else
    {
        if (block_count > 1)
        {
            word_bin_rep(sub_string, bv->data[block_count - 1], WORD_SIZE);
            string_prepend(super_string, sub_string, WORD_SIZE, WORD_SIZE);
            nsuper += WORD_SIZE;
        }
    }

    /*print the bit vector*/
    string_reverse(super_string, nsuper);
    print_string_as_array(super_string, nsuper);
    free(sub_string);
    free(super_string);
}

void bv_set(bitvector *bv, uint64_t pos)
{
    /** sets the bit at index pos to 1 **/

    bv_check_index(bv, pos);
    uint64_t block, block_index;
    uint8_t k;

    block_index = (uint64_t)pos / WORD_SIZE;
    block = bv_get_block(bv, block_index);
    k = pos % WORD_SIZE;

    block = word_set(block, k);
    bv->data[block_index] = block;
}

static inline uint64_t bv_get_block(bitvector *bv, uint64_t block_index)
{
    return bv->data[block_index];
}

void bv_check_index(bitvector *bv, uint64_t pos)
{
    if (pos >= bv_len(bv))
    {
        fprintf(stderr, "IndexError: bitvector index out of bounds. The size of this bit_vector is %zu.\n", bv_len(bv));
        exit(0);
    }
}

void bv_clear(bitvector *bv, uint64_t pos)
{
    /** clears the bit at the pos **/

    bv_check_index(bv, pos);
    uint64_t block, block_index;
    uint8_t k;

    block_index = (uint64_t)pos / WORD_SIZE;
    block = bv_get_block(bv, block_index);
    k = pos % WORD_SIZE;

    block = word_clear(block, k);
    bv->data[block_index] = block;
}

uint64_t bv_pop_count(bitvector *bv, uint64_t pos)
{
    /** return the number of 1's in the bitvector **/

    if (bv_len(bv) == 0 || pos > bv_len(bv))
    {
        return 0;
    }

    ++pos;

    uint64_t i, card, block, block_count, remainder;
    block_count = ceil((double)pos / WORD_SIZE) - 1;
    remainder = pos % WORD_SIZE;

    card = 0;
    for (i = 0; i < block_count; i++)
    {
        if (bv->data[i])
            card += popcnt(bv->data[i]);
    }
    block = bv->data[block_count];
    if (remainder > 0)
    {
        block = ~(ALL_ONES_MASK << remainder) & block;
        card += popcnt(block);
    }
    return card;
}

void bv_resize(bitvector *bv, size_t new_size)
{
    /** resize the bit vector accordingly **/

    uint64_t i, curr_size;
    uint64_t j, last_partial_block, remainder, new_actual_size;

    curr_size = bv_len(bv);
    if (curr_size == new_size)
        return;

    /*shrink a bit vector*/
    if (curr_size > new_size)
    {

        j = ((int64_t)new_size - 1) / (int64_t)WORD_SIZE;
        remainder = new_size % WORD_SIZE;
        last_partial_block = bv->data[j];
        last_partial_block = ~(ALL_ONES_MASK << remainder) & last_partial_block;
        bv->data[j] = last_partial_block;

        new_actual_size = ceil((double)new_size / WORD_SIZE);
        if (new_actual_size < bv->allocated)
        {
            /** shrink the underlying array */
            uint64_t *new_vector = realloc(bv->data, new_actual_size * WORD_SIZE);
            bv->data = new_vector;
        }
    }
    else
    {
        /** extend the bitvector **/
        uint64_t additional_size, last_block_index;

        new_actual_size = ceil((double)new_size / WORD_SIZE);
        if (new_actual_size != bv->allocated)
        {
            /** grow the underlying array */
            uint64_t *new_vector = realloc(bv->data, new_actual_size * WORD_SIZE);
            bv->data = new_vector;
        }
        if (new_size > bv->allocated)
        {
            last_block_index = (curr_size - 1) / WORD_SIZE + 1;
            additional_size = new_actual_size - bv->allocated;
            for (i = 0; i < additional_size; i++)
            {
                bv->data[last_block_index + i] = 0;
            }
        }
    }

    bv->allocated = new_actual_size * WORD_SIZE;
    bv->size = new_size;
}

size_t bv_len(bitvector *bv)
{
    /** returns the number of bits in a bit vector */
    return bv->size;
}

bool bv_isset(bitvector *bv, uint64_t pos)
{
    bv_check_index(bv, pos);
    uint64_t block, block_index;
    uint8_t k;

    block_index = (uint64_t)pos / WORD_SIZE;
    block = bv_get_block(bv, block_index);
    k = pos % WORD_SIZE;

    return word_isset(block, k);
}

static inline bitvector *bv_shorter(bitvector *a, bitvector *b)
{
    return (a->size > b->size) ? b : a;
}

static inline uint64_t max(uint64_t a, uint64_t b)
{
    return (a > b) ? a : b;
}

void bv_print_dec(bitvector *bv)
{
    /**print the bit vector in base 10 */

    if (bv->allocated == 0)
    {
        printf("[]");
    }
    else
    {
        printf("[");
        uint64_t i, num_ints;
        for (num_ints = (bv->allocated / WORD_SIZE), i = 0; i < num_ints - 1; i++)
        {
            printf("%llu ", bv->data[i]);
        }
        printf("%llu]\n", bv->data[num_ints - 1]);
    }
}

bitvector *bv_xor(bitvector *a, bitvector *b)
{
    /* assumes that the shorter vector is padded with 0's*/

    if (bv_len(a) != bv_len(b))
    {
        bitvector *shorter = bv_shorter(a, b);
        uint64_t max_length = max(bv_len(a), bv_len(b));
        bv_resize(shorter, max_length);
    }

    uint64_t nbv, i;
    bitvector *bv;

    nbv = bv_len(a) / WORD_SIZE;
    bv = bv_new(bv_len(a));

    for (i = 0; i <= nbv; i++)
        bv->data[i] = a->data[i] ^ b->data[i];

    return bv;
}

bool bv_equal(bitvector *a, bitvector *b)
{
    BV_CHECK_NONNULL(a);
    BV_CHECK_NONNULL(b);

    // if the two vectors differ in length, they can't be equal
    if (bv_len(a) != bv_len(b))
        return false;

    uint64_t *ap = a->data;
    uint64_t *bp = b->data;

    for (uint64_t i = 0; i < a->size; i++)
        if (*ap++ != *bp++)
            return false;

    return true;
}

bitvector *bv_complement(bitvector *bv)
{
    /* return the complement of the bit-vector bv such that for each bit in bv,
     * bv_reversed = ~bit. **/
    bv = bv_copy(bv);

    if (!bv_len(bv))
    {
        return bv;
    }
    else
    {
        uint64_t i, block, nb;
        nb = bv_len(bv) / WORD_SIZE;
        for (i = 0; i <= nb; i++)
        {
            block = bv_get_block(bv, i);
            block = ~block;
            bv->data[block] = block;
        }
    }
    return bv;
}

bool bv_is(bitvector *a, bitvector *b)
{
    return (a == b);
}

bitvector *bv_copy(bitvector *bv)
{
    /** deepcopy a bitvector **/
    bitvector *copy = bv_new(bv_len(bv));
    uint64_t i, nb;
    for (i = 0, nb = bv_len(bv) / WORD_SIZE; i <= nb; i++)
        copy->data[i] = bv->data[i];
    return copy;
}

uint64_t reverse_bits(uint64_t x)
{
    /** reverse the bits in a 64-bit word
     * fastest method i've found so far **/

    x = (((x & 0xaaaaaaaaaaaaaaaa) >> 1) | ((x & 0x5555555555555555) << 1));
    x = (((x & 0xcccccccccccccccc) >> 2) | ((x & 0x3333333333333333) << 2));
    x = (((x & 0xf0f0f0f0f0f0f0f0) >> 4) | ((x & 0x0f0f0f0f0f0f0f0f) << 4));
    x = (((x & 0xff00ff00ff00ff00) >> 8) | ((x & 0x00ff00ff00ff00ff) << 8));
    x = (((x & 0xffff0000ffff0000) >> 16) | ((x & 0x0000ffff0000ffff) << 16));

    return ((x >> 32) | (x << 32));
}

short signed int kth_bit(uint64_t x, uint64_t k)
{
    /** find the position of the kth set bit in x in O(w) where w = word size**/
    /** can be optimized to use binary search **/

    unsigned short int c = 0;
    short int i = 0;

    while (x)
    {
        if (x & BIT)
            c++;
        if (c == k)
            return i;
        i++;
        x = x >> BIT;
    }
    return -1;
}

int64_t bv_select(bitvector *bv, uint64_t k)
{
    /** find the position of the k'th set bit **/

    bv_check_index(bv, k);
    int64_t card, i, nb;
    int8_t remainder, in_block_popcnt;
    int8_t pos_lblock;
    uint64_t block;

    card = 0;
    nb = bv_len(bv) / WORD_SIZE;
    for (i = 0; i < nb; i++)
    {
        block = bv_get_block(bv, i);
        in_block_popcnt = popcnt(block);
        if ((uint64_t)in_block_popcnt < k)
        {
            card += WORD_SIZE;
            k -= in_block_popcnt;
        }
        else
        {
            return card + kth_bit(block, k);
        }
    }

    // last step, checking for remainder
    remainder = (bv_len(bv) - 1) % WORD_SIZE;
    if (remainder > 0)
    {
        pos_lblock = kth_bit(bv_get_block(bv, nb), k);
        if (pos_lblock == -1 || pos_lblock >= remainder)
        {
            return -1;
        }
        else
        {
            return card + pos_lblock;
        }
    }
    return -1;
}

bitvector *bv_reverse(bitvector *bv)
{
    /* works only for multiples of 64 **/

    if (bv_len(bv) < 2)
    {
        return bv_copy(bv);
    }
    bitvector *bv_reversed = bv_new(bv_len(bv));

    uint64_t j, i;
    j = bv_len(bv) / WORD_SIZE; /*index of last-occupied block */

    for (i = 0; i < j; i++)
    {
        /** minor optimization **/
        if (!bv->data[i])
        {
            bv_reversed->data[i] = 0;
        }
        else
        {
            bv_reversed->data[i] = reverse_bits(bv->data[i]);
        }
    }
    bv_reversed->data[j] = reverse_bits(bv->data[i]);
    return bv_reversed;
}

void bv_free(bitvector *bv)
{
    free(bv->data);
    free(bv);
}

uint64_t bv_hamming_distance(bitvector *a, bitvector *b)
{
    bitvector *bv_xored = bv_xor(a, b);
    const uint64_t hamming_weight = bv_pop_count(bv_xored, bv_len(bv_xored));
    free(bv_xored);
    return hamming_weight;
}

bool bv_toggle(bitvector *bv, uint64_t pos)
{
    bv_check_index(bv, pos);

    uint64_t block_index, k, block;
    block_index = pos / WORD_SIZE;
    k = pos % WORD_SIZE;
    block = bv_get_block(bv, block_index);
    block = word_toggle(block, k);
    bv->data[block_index] = block;
    return true;
}

uint64_t msb(uint64_t v)
{
    static uint8_t deBruijn[64] =
        {0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,
         62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,
         63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
         51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12};
    static uint64_t multiplier = 0x022fdd63cc95386dUL;

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    if (v == (uint64_t)-1)
        return 64;
    v++;
    return deBruijn[(v * multiplier) >> 58];
}

void word_bin_rep(char *string, uint64_t n, size_t nx)
{
    /** return the binary representation of a number using nx characters */

    if (nx < msb(n))
    {
        fprintf(stderr, "nx is too small to correctly represent x");
        return;
    }

    int64_t c;
    uint64_t k;

    for (c = --nx; c >= 0; c--)
    {
        k = n >> c;
        if (k & BIT)
            string[nx - c] = '1';
        else
            string[nx - c] = '0';
    }

    string[++nx] = '\0';
}
