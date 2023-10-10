#ifndef STRUTILS_H
#define STRUTILS_H

#define ASCII_N 128
#define ALPHABET_N 26

extern const char ASCII[ASCII_N+1];
extern const char ALPHABET[ALPHABET_N+1];


char *strupper(const char *str);
char *strlower(const char *str);

char *strslice(const char *str, int start, int end);


#endif