#include "strutils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char ALPHABET[ALPHABET_N+1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char ASCII[ASCII_N+1] = "\x00\x01\x02\x03\x04\x05\x06\a""\b""\t""\n""\b""\f""\r""\x0e\x0f"
                              "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
                              " !'#$%&'()*+,-./0123456789:;<=>?"
                              "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                              "`abcdefghijklmnopqrstuvwxyz{|}~\x7f";


/**
 * @brief Converts a string to uppercase.
 *
 * This function takes a string as input and returns a new string with all
 * characters converted to uppercase. The original string is not modified.
 *
 * @param str The string to convert to uppercase.
 * @return A new string with all characters converted to uppercase.
 * @note The returned string must be freed by the caller.
 */
char *strupper(const char *str)
{
    char *upper = strdup(str);
    for (int i=0; str[i] != '\0'; i++) {
        upper[i] = toupper(str[i]);
    }
    return upper;
}

/**
 * @brief Converts a string to lowercase.
 *
 * This function takes a string as input and returns a new string with all
 * characters converted to lowercase. The original string is not modified.
 *
 * @param str The string to convert to lowercase.
 * @return A new string with all characters converted to lowercase.
 * @note The returned string must be freed by the caller.
 */
char *strlower(const char *str)
{
    char *lower = strdup(str);
    for (int i=0; str[i] != '\0'; i++) {
        lower[i] = tolower(str[i]);
    }
    return lower;
}


/**
 * @brief Returns a slice of a string.
 *
 * This function takes a string as input and returns a new string that is a
 * slice of the input string, starting at the specified start index (inclusive)
 * and ending at the specified end index (exclusive). The original string is
 * not modified.
 *
 * @param source The source string to slice.
 * @param start The start index of the slice (inclusive).
 * @param end The end index of the slice (exclusive).
 * @return A new string that is a slice of the input string.
 * @note The returned string must be freed by the caller.
 */
char *strslice(const char *source, int start, int end)
{
    char *slice = malloc(sizeof(char) * (end - start + 1));
    for (int i=0; i < end-start; i++) {
        slice[i] = source[i+start];
    }
    slice[end - start] = '\0';
    return slice;
}
