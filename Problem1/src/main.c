#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freq_scanner.h"

#define TOP_N 10
#define TOP_FREQ_N 5


static const char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s requires exactly 1 argument (%d provided)\n", argv[0], argc - 1);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", argv[1]);
        return 1;
    }

    letter_count_t *counts = count_letters(fp);
    fclose(fp);

    int total = sum(counts, ALPHABET_N);
    lc_sort(counts, ALPHABET_N, -1);

    char *top_10 = malloc(sizeof(char) * (1+TOP_N));
    for(int i = 0; i < TOP_N; i++) {
        top_10[i] = counts[i].c;
    }
    top_10[TOP_N] = '\0';

    printf("Total number of letters: %d\n", total);
    printf("Letters sorted by frequency: %s\n", top_10);
    printf("Most frequent letters: \n");
    for (int i = 0; i < TOP_FREQ_N; i++) {
        printf("%c: %5.2f %% (%d/%d)\n", counts[i].c, 100.0*counts[i].n/total, counts[i].n, total);
    }

    free(counts);

    return 0;
}
