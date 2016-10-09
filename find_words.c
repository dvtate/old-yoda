#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "find_words.h"



extern uint16_t rows, cols;
extern char** game, **solutions;
extern char* words;
extern size_t words_len;



// diagonal find
char d_upRight(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row - i < 0 || col + i >= cols)
			return 0;
		else if (CHAR_AT(game, row - i, col + i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row - i, col + i) = 1;

	return 1;
}
char d_downRight(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row + i >= rows || col + i >= cols)
			return 0;
		else if (CHAR_AT(game, row + i, col + i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row + i, col + i) = 1;

	return 1;

}
char d_upLeft(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row - i < 0 || col - i < 0)
			return 0;
		else if (CHAR_AT(game, row - i, col - i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row - i, col - i) = 1;

	return 1;
}
char d_downLeft(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row + i >= rows || col - i < 0)
			return 0;
		else if (CHAR_AT(game, row + i, col - i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row + i, col - i) = 1;

	return 1;
}


// horizontal find
char h_up(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row - i < 0)
			return 0;
		else if (CHAR_AT(game, row - i, col) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row - i, col) = 1;


	return 1;
}
char h_down(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (row + i >= rows)
			return 0;
		else if (CHAR_AT(game, row + i, col) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row + i, col) = 1;


	return 1;
}
char h_left(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (col - i < 0)
			return 0;
		else if (CHAR_AT(game, row, col - i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row, col - i) = 1;


	return 1;
}
char h_right(const char* word, const uint16_t row, const uint16_t col){
	// check if word is there
	for (uint16_t i = 0; i < strlen(word); i++)
		if (col + i >= cols)
			return 0;
		else if (CHAR_AT(game, row, col + i) != *(word + i))
			return 0;

	// paint word on board
	for (uint16_t i = 0; i < strlen(word); i++)
		CHAR_AT(solutions, row, col + i) = 1;


	return 1;
}




void findWords(void){

	char* word = strtok(words, " ,");
	while (word && *word) {

		//std::cout <<"Finding word - \"" <<word <<"\"\n";



		for (uint16_t r = 0; r < rows; r++)
			for (uint16_t c = 0; c < cols; c++) {
				// if the first letter of the word is found
				if (CHAR_AT(game, r, c) == *word) {

					// see if the rest of the word is there
					if (d_upRight(word, r, c))
						goto next_word;
					else if (d_downRight(word, r, c))
						goto next_word;
					else if (d_upLeft(word, r, c))
						goto next_word;
					else if (d_downLeft(word, r, c))
						goto next_word;

					else if (h_up(word, r, c))
						goto next_word;
					else if (h_down(word, r, c))
						goto next_word;
					else if (h_left(word, r, c))
						goto next_word;
					else if (h_right(word, r, c))
						goto next_word;


				}

			}

		printf("\aWarning: couldn\'t find word \"%s\"\n", word);

next_word:
		word = strtok(NULL, " ,");


	}
}

