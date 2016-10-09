#ifndef WORDFINDER_UTILS_H
#define WORDFINDER_UTILS_H

#include <string.h>
#include <inttypes.h>

extern uint16_t rows, cols;
extern char** game;



#define CHAR_AT(MAT, ROW, COL)\
		*(*((MAT) + (ROW)) + (COL))


// removes the trailing newline character at the end of the string by replacing
// it with a nulll-terminator ('\0')
inline void deleteNewline(char* str){

	// there is probably a more efficient solution...
	for (uint16_t i = 0; i < strlen(str); i++, str++);
	if (*str == '\n')
		*str = '\0';
}


#endif
