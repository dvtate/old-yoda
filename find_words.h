#ifndef FIND_WORDS_H
#define FIND_WORDS_H

#include <string.h>

#include "utils.h"

extern uint16_t rows, cols;
extern char** game, **solutions;
extern char* words;
extern size_t words_len;


char d_upRight(const char* word, const uint16_t row, const uint16_t col);
char d_downRight(const char* word, const uint16_t row, const uint16_t col);
char d_upLeft(const char* word, const uint16_t row, const uint16_t col);
char d_downLeft(const char* word, const uint16_t row, const uint16_t col);

char h_up(const char* word, const uint16_t row, const uint16_t col);
char h_down(const char* word, const uint16_t row, const uint16_t col);
char h_left(const char* word, const uint16_t row, const uint16_t col);
char h_right(const char* word, const uint16_t row, const uint16_t col);


void findWords(void);

#endif
