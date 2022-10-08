#include "bitvector.h"
#include <string.h>
#include <stdio.h>

#define THRESHHOLD (40)

void string_reverse(char *string, size_t ns)
{
    /** reverse a string in-place **/

    if (ns < 2)
    {
        return;
    }
    size_t i;
    char c;

    --ns;
    for (i = 0; i < ns; i++, ns--)
    {
        c = string[i];
        string[i] = string[ns];
        string[ns] = c;
    }
}

void print_string_as_array(char *string, size_t size)
{
    /** print the characters of a string as an array of of space-separated chars */

    if (size == 0)
    {
        printf("[]");
        return;
    }

    printf("[");
    size_t i;
    if (size > THRESHHOLD)
    {
        for (i = 0; i < THRESHHOLD - 4; i++)
            printf("%c ", string[i]);
        printf("... ");
        for (i = size - 4; i < size - 1; i++)
            printf("%c ", string[i]);
        printf("%c]\n", string[size - 1]);
    }
    else
    {
        for (i = 0; i < size - 1; i++)
            printf("%c ", string[i]);
        printf("%c]\n", string[size - 1]);
    }
}

void string_prepend(char *super_string, char *sub_string, uint64_t nsuper, uint64_t nsub)
{
    /**append the substring at the beginning of the super string**/

    string_reverse(super_string, nsuper);
    string_reverse(sub_string, nsub);
    strcat(super_string, sub_string);
    string_reverse(super_string, (nsuper + nsub));
}
