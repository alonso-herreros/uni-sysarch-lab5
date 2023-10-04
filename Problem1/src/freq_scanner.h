#ifndef FREQ_SCANNER_H
#define FREQ_SCANNER_H

#include <stdio.h>

#define ALPHABET_N 26


typedef struct letter_count {
    char c;
    int n;
} letter_count_t;


letter_count_t *letter_counts_init(char *str);

letter_count_t *count_letters(FILE *fp);

void lc_sort(letter_count_t *array, int size, int order);

int sum(letter_count_t *arr, int size);

#endif