//
// Created by Erastus M Murungi on 1/30/20.
//

#ifndef POPPY_BITVECTOR_H
#define POPPY_BITVECTOR_H

#endif // POPPY_BITVECTOR_H

#include <stdlib.h>
#include <stdbool.h>

#define THRESHHOLD (40)
#define MASK (0xffffffffffffffffUL)
#define BIT (1UL)
#define BB_SIZE (64)
#define SHIFT (6UL)

struct bitvector
{
    uint64_t *data;
    uint64_t size;
    uint64_t allocated; // multiple of BB_SIZE
};

typedef struct bitvector bitvector;

bitvector *bv_new();

void bv_free(bitvector *);

uint64_t bv_hamming_distance(bitvector *bv1, bitvector *bv2);

bitvector *bv_xor(bitvector *, bitvector *);

bitvector *bv_complement(bitvector *);

bitvector *bv_intersection(bitvector *, bitvector *);

bool bv_equal(bitvector *, bitvector *);

bool bv_is(bitvector *, bitvector *);

size_t bv_len(bitvector *);

void bv_resize(bitvector *, size_t);

void bv_set(bitvector *, uint64_t pos);

void bv_range_set(bitvector *bv, short unsigned bit, size_t from, size_t to);

bool bv_clear(bitvector *bv, uint64_t pos);

bool bv_toggle(bitvector *bv, uint64_t pos);

void bv_check_index(bitvector *bv, uint64_t pos);

uint64_t bv_pop_count(bitvector *bv, uint64_t pos);

bool bv_isset(bitvector *bv, uint64_t pos);

bitvector *bv_copy(bitvector *bv);

bitvector *bv_extend(bitvector *, bitvector *);

bitvector *bv_rshift(bitvector *, bitvector *);

bitvector *bv_lshift(bitvector *, bitvector *);

bitvector *bv_rotate(bitvector *);

bitvector *bv_reverse(bitvector *);

void bv_print(bitvector *);

void bv_print_dec(bitvector *bv);

void bin_rep(char *string, uint64_t x, size_t nx);

void print_string_as_array(char *string, size_t size);

void reverse_string(char *string, size_t ns);

void prepend(char *super_string, char *sub_string, uint64_t nsuper, uint64_t nsub);

int64_t select(bitvector *bv, uint64_t k);

uint64_t reverse_bits(uint64_t x);

uint64_t msb(uint64_t v);

static inline u_int64_t popcnt(uint64_t i)
{
    return __builtin_popcountll(i);
}