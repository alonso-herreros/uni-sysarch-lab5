#include "charstats.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strutils.h"

static CharStats *cstats_init(int case_sensitive);
static void cstats_free(CharStats *ptr_ptr);

static void printa(CharStats *ptr);

static int total(CharStats *ptr);
static int sum(CharStats *ptr, const char *collection, int char_n);

static int get_count(CharStats *ptr, char c);
static float get_freq(CharStats *ptr, char c);

static char *get_sorted(CharStats *ptr);
static char *get_top_n(CharStats *ptr, int n);
static char *get_top_n_coll(CharStats *ptr, int n, const char *collection, int char_n);


/** @brief Initializes a new CharStats object pointer.
 *
 * This function allocates memory for a new CharStats object and initializes its
 * counts array to all zeros. The `case_sensitive` parameter determines whether
 * the object should be case-sensitive or not.
 *
 * @param case_sensitive Whether the CharStats object should be case-sensitive.
 * @return A pointer to the newly created CharStats object.
 */
static CharStats *cstats_init(int case_sensitive)
{
    CharStats *ptr = calloc(1, sizeof(CharStats));
    ptr->counts = calloc(ASCII_N, sizeof(int));
    ptr->csens = case_sensitive;
    ptr->_sum = 0;

    ptr->free = cstats_free;

    ptr->printa = printa;

    ptr->sum = sum;
    ptr->total = total;

    ptr->get_count = get_count;
    ptr->get_freq = get_freq;
    ptr->get_sorted = get_sorted;
    ptr->get_top_n = get_top_n;
    ptr->get_top_n_coll = get_top_n_coll;
    
    return ptr;
}

/** @brief Initializes a new CharStats object and counts the occurrences of each character in a file specified by a path.
 *
 * This function opens the file specified by the given path and reads characters
 * from it, incrementing the corresponding count in the newly created CharStats
 * object. The `case_sensitive` parameter determines whether the object should
 * be case-sensitive or not. The function returns a pointer to the newly created
 * CharStats object, or `NULL` if the file could not be opened.
 *
 * @param path The path to the file to read characters from.
 * @param case_sensitive Whether the CharStats object should be case-sensitive.
 * @return A pointer to the newly created CharStats object, or `NULL` if the file could not be opened.
 */
CharStats *cstats_init_path(char *path, int case_sensitive)
{
    CharStats *ptr = NULL;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", path);
    }
    else {
        ptr = cstats_init_fp(fp, case_sensitive);
        fclose(fp);
    }
    return ptr;
}
/** @brief Initializes a new CharStats object and counts the occurrences of each character in a file.
 *
 * This function reads characters from the given file pointer and increments
 * the corresponding count in the newly created CharStats object. The `case_sensitive`
 * parameter determines whether the object should be case-sensitive or not. The
 * function returns a pointer to the newly created CharStats object.
 *
 * @param fp The file pointer to read characters from.
 * @param case_sensitive Whether the CharStats object should be case-sensitive.
 * @return A pointer to the newly created CharStats object.
 */
CharStats *cstats_init_fp(FILE *fp, int case_sensitive)
{
    CharStats *ptr = cstats_init(case_sensitive);
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c < ASCII_N) {
            ptr->counts[ptr->csens==0 ? (int) toupper(c) : (int) c]++;
        }
    }
    return ptr;
}


/** @brief Frees the memory allocated for a CharStats object.
 *
 * This function frees the memory allocated for a CharStats object, including
 * its counts array. The `ptr` parameter should be a pointer to the CharStats
 * object to be freed.
 *
 * @param ptr A pointer to the CharStats object to be freed.
 */
static void cstats_free(CharStats *ptr)
{
    free(ptr->counts);
    free(ptr);
}

/** @brief Prints the counts of all characters in a CharStats object.
 *
 * Prints the counts of all characters in a CharStats object's counts array.
 * The `ptr` parameter should be a pointer to an initialized CharStats object.
 * The counts are printed in hexadecimal format.
 *
 * @param ptr A pointer to the CharStats object to print the counts of.
 */
static void printa(CharStats *ptr)
{
    for (u_char c = 0; c < ASCII_N; c++) {
        printf("'%c'(%02hhx @%p): %d\n", c, c, ptr->counts + c*sizeof(int), ptr->counts[c]);
    }
}


/** @brief Calculates the total count of all characters in a CharStats object.
 *
 * This function calculates the total count of all characters in a CharStats
 * object's counts array, and returns the result. The `ptr` parameter should
 * be an initialized CharStats pointer. The function iterates over all ASCII
 * characters and sums the corresponding counts in the counts array. The
 * function returns the total count.
 *
 * @param ptr The pointer to the CharStats object to calculate the total count for.
 * @return The total count of all characters in the object's counts array.
 */
static int total(CharStats *ptr)
{
    int total = 0;
    for (int i = 0; i < ASCII_N; i++) {
        total += ptr->counts[i];
    };
    return total;
}
/** @brief Calculates the sum of the counts for a given set of characters.
 *
 * This function takes a CharStats pointer and an array of characters, and
 * calculates the sum of the counts for each character in the array. The `ptr`
 * parameter should be a CharStats pointer that has already been initialized.
 * The `collection` parameter should be an array of characters to sum the counts for.
 * The `char_n` parameter should be the number of characters in the array. The
 * function returns the sum of the counts.
 *
 * @param ptr The pointer to the CharStats object to calculate the sum for.
 * @param collection An array of characters to sum the counts for.
 * @param char_n The number of characters in the array.
 * @return The sum of the counts for the given characters.
 */
static int sum(CharStats *ptr, const char* collection, int char_n)
{
    int sum = 0;
    for (int i = 0; i < char_n; i++) {
        sum += ptr->counts[(int) collection[i]];
    }
    ptr->_sum = sum;
    return sum;
}


/** @brief Gets the count for a character in a CharStats object.
 *
 * Returns the count for a given character in a CharStats object's counts array.
 * If the object is case-insensitive, the character is converted to uppercase
 * before looking up the count. The `ptr` parameter should be an initialized
 * CharStats pointer. The `c` parameter is the character to get the count for.
 *
 * @param ptr The pointer to the CharStats object to get the count from.
 * @param c The character to get the count for.
 * @return The count for the given character.
 */
static int get_count(CharStats *ptr, char c)
{
    return ptr->counts[ptr->csens==0 ? (int) toupper(c) : (int) c];
}
/** @brief Gets the frequency for a character in a CharStats object.
 *
 * Returns the frequency for a given character in a CharStats object's counts
 * array, as a float between 0 and 1. If the object is case-insensitive, the
 * character is converted to uppercase before looking up the count. The `ptr`
 * parameter should be an initialized CharStats pointer. The `c` parameter is
 * the character to get the frequency for. If the `total` parameter is 0 (the
 * default), the function uses the total count of all uppercase alphabetical
 * characters.
 *
 * @param ptr The pointer to the CharStats object to get the frequency from.
 * @param c The character to get the frequency for.
 * @return The frequency for the given character, as a float between 0 and 1.
 */
static float get_freq(CharStats *ptr, char c)
{
    if (ptr->_sum == 0)  ptr->sum(ptr, ALPHABET, ALPHABET_N);
    return (float) ptr->counts[ptr->csens==0 ? (u_char) toupper(c) : (u_char) c] / ptr->_sum;
}

/** @brief Compares the counts of two characters in a CharStats object. */
static int countcmp(const void *a, const void *b, CharStats *ptr)
{
    return ptr->counts[*(u_char *) b] - ptr->counts[*(u_char *) a];
}
/** @brief Returns a sorted array of all ASCII characters based on their counts in a CharStats object.
 *
 * Returns a sorted array of all ASCII characters based on their counts in a CharStats object's counts array.
 * The `ptr` parameter should be a pointer to an initialized CharStats object.
 * The returned array is sorted in ascending order of character counts.
 *
 * @param ptr A pointer to the CharStats object to sort the characters by.
 * @return A pointer to the sorted array of characters.
 */
static char *get_sorted(CharStats *ptr)
{
    char *sorted = malloc((ASCII_N)*sizeof(char));
    memcpy(sorted, ASCII, ASCII_N);
    qsort_r(sorted, ASCII_N, sizeof(char), countcmp, ptr);
    return sorted;
}

/** @brief Calculates the total count of all characters in a CharStats object.
 *
 * Calculates the total count of all characters in a CharStats object's counts array.
 * The `ptr` parameter should be a pointer to an initialized CharStats object.
 *
 * @param ptr A pointer to the CharStats object to calculate the total count of.
 * @return The total count of all characters in the CharStats object.
 */
static char *get_top_n(CharStats *ptr, int n)
{
    char *sorted = ptr->get_sorted(ptr);
    char *top_n = strslice(sorted, 0, n);
    free(sorted);
    return top_n;
}

/**
 * @brief Returns the top n characters from a given collection in sorted order.
 *
 * This function takes a pointer to a `CharStats` struct, an integer `n`, a string `collection`,
 * and an integer `char_n`. It returns a string containing the top `n` characters from the given
 * `collection`, sorted in descending order. The `CharStats` struct must have a `get_sorted`
 * function that returns a string containing all the characters in the struct, sorted in
 * descending order of frequency.
 *
 * @param ptr Pointer to a `CharStats` struct.
 * @param n Number of characters to return.
 * @param collection String containing the characters to filter.
 * @param char_n Length of the `collection` string.
 * @return String containing the top `n` characters from the given `collection`, sorted in
 *         descending order.
 */
static char *get_top_n_coll(CharStats *ptr, int n, const char *collection, int char_n)
{
    char *sorted = ptr->get_sorted(ptr);
    char *filtered = calloc(char_n+1, sizeof(char));
    int j = 0;
    for (int i=0; j < n && i < ASCII_N; i++) {
        if (strchr(collection, sorted[i]) != NULL)  filtered[j++] = sorted[i];
    }
    filtered[j] = '\0';
    free(sorted);
    return filtered;
}
