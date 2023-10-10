#ifndef CHARSTATS_H
#define CHARSTATS_H

#include <stdio.h>

typedef struct char_stats {
    // Counts of each character
    int *counts;
    // Case sensitivity
    int csens;
    // Total number of characters as last counted by the sum() function
    int _sum;

    void (*free)(struct char_stats *);

    void (*printa)(struct char_stats *);
    
    int (*sum)(struct char_stats *, const char *, int);
    int (*total)(struct char_stats *);

    int (*get_count)(struct char_stats *, char);
    float (*get_freq)(struct char_stats *, char);

    char *(*get_sorted)(struct char_stats *);
    char *(*get_top_n)(struct char_stats *, int);
    char *(*get_top_n_coll)(struct char_stats *, int, const char *, int);

} CharStats;

CharStats *cstats_init_path(char *path, int case_sensitive);
CharStats *cstats_init_fp(FILE *fp, int case_sensitive);

#endif