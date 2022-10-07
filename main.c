#include "bitvector.h"
#include <stdio.h>

int main(void) {
    const int N = 20;
    bitvector *b = bv_new(N);
    int i;
    for (i = 4; i < 13; i++) {
        bv_set(b, i);
    }
    bv_set(b, 18);
    printf("%d\n", bv_isset(b, 18));
    bv_print(b);
    printf("%llu\n", bv_pop_count(b, N));
    printf("%llu\n", b->allocated / 64);
    bv_free(b);
    return 0;
}