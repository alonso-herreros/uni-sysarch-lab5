#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "charstats.h"
#include "strutils.h"

#define TOP_N 10
#define TOP_FREQ_N 5


int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "%s requires exactly 1 argument (%d provided)\n", argv[0], argc - 1);
        return 1;
    }

    CharStats *stats = cstats_init_path(argv[1], 0);

    int total = stats->sum(stats, ALPHABET, ALPHABET_N);

    printf("Total number of letters: %d\n", total);
    char *top_10 = stats->get_top_n_coll(stats, TOP_N, ALPHABET, ALPHABET_N);
    printf("Letters sorted by frequency: %s\n", top_10);
    printf("Most frequent letters: \n");
    for (int i = 0; i < TOP_FREQ_N; i++) {
        printf("%c: %5.2f %% (%d/%d)\n",
        top_10[i], 100.0*stats->get_freq(stats, top_10[i]), stats->get_count(stats, top_10[i]), total);
    }

    stats->free(stats);
    free(top_10);

    return 0;
}
