#include "freq_scanner.h"
#include <stdlib.h>
#include <string.h>

#define UPPERCASE_A 65
#define LOWERCASE_A 97
#define MIN -2147483647-1


static char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


/// @brief Initialize an array of letter counts from a string
/// @param str The string to initialize from
/// @return A pointer to a `letter_count_t` array with all counts set to 0
letter_count_t *letter_counts_init(char *str)
{
    letter_count_t *counts = malloc(sizeof(letter_count_t) * strlen(str));
    for (int i = 0; i < strlen(str); i++) {
        counts[i].c = str[i];
        counts[i].n = 0;
    }
    return counts;
}


/// @brief Builds an array of `letter_count`s from a file. Case-insensitive.
/// @param fp The file pointer
/// @return A pointer to an array of 26 letter counts, with the counts of each letter in alphabetical order.
letter_count_t *count_letters(FILE *fp)
{
    letter_count_t *counts = letter_counts_init(ALPHABET);
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c >= 'a' && c <= 'z') {
            counts[c - 'a'].n++;
        } else if (c >= 'A' && c <= 'Z') {
            counts[c - 'A'].n++;
        }
    }
    return counts;
}


int lc_cmp(const void *a, const void *b)
{
    letter_count_t *lca = (letter_count_t *) a;
    letter_count_t *lcb = (letter_count_t *) b;
    return lca->n - lcb->n;
}
int lc_cmp_inv(const void *a, const void *b)
{
    return -lc_cmp(a, b);
}



/// @brief Sort an array of `letter_count` structs by the n field. Sorts in-place.
/// @param array The `letter_count` array to sort
/// @param size The size of the arrays. Both arrays must be the same size
/// @param order The order to sort in: `1` for ascending, `-1` for descending
/// @return Nothing
void lc_sort(letter_count_t *array, int size, int order)
{
    qsort(array, size, sizeof(letter_count_t), order>0? lc_cmp : lc_cmp_inv);
}

/// @brief Sums the elements of an array of `letter_count` structs
/// @param arr The array to sum
/// @param size The array size
/// @return The sum of the array elements's `n` fields
int sum(letter_count_t *arr, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i].n;
    }
    return sum;
}
