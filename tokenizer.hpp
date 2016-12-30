#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>



// removes char from string from it's pointer
// this is extremely inefficient...
static inline void deleteChar(char* toDelete)
	{ memmove(toDelete, toDelete + 1, strlen(toDelete)); }
static inline void deleteChars(char* toDelete, const size_t numChars)
	{ memmove(toDelete, toDelete + numChars, strlen(toDelete)); }

// Returns the start of the token/string-constant
// manages escape-sequences within string-constants
char* qtok(char* str, char** next){
	if (str == NULL)
		return NULL;
	char *current = str, *start = str;

    // skip beginning whitespace.
    while (*current != '\0' && isspace(*current))
        current++;

    start = current;

    if (*current == '"') {
        // Quoted token
        start = current;
        current++; // Skip the beginning quote.

		bool lastWasSlash = false;

        for (;;) {
            // Go until we find a quote or the end of string.
            while (*current != '\0'  && *current != '"') {
                lastWasSlash = false;

                if (*current == '\\') {
                    if (*(current + 1) == 'n') {
                        *current = '\n';
                        *(current + 1) = '\r'; // it's not that I want to support windows...
					    					   // this is just more efficienct than deleteChar()
											   // also, this doesn't do the same thing as '\n'..
											   // so I probably should find a better solution...
                    } else if (*(current + 1) == 'r') {
                        *current = *(current + 1) = '\r'; // nice hack :)
                        current++;
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'a') {
                        *current = '\a';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'b') {
                        *current = '\b';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'f') {
                        *current = '\f';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'v') {
                        *current = '\v';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == '\\') {
                        deleteChar(current);
                        lastWasSlash = true;
                    } else if (isdigit(*(current + 1))) {
						// escape sequence can be a maximum of 3 chars long
                    	char str[3] = {
                    		*(current + 1),
                    		*(current + 2),
                    		*(current + 3)
                    	};
                    	char* ptr;
						// convert the octal literal to binary
						*current = (char) strtol(str, &ptr, 8);
						deleteChars(current + 1, ptr - str);

                    } else if (*(current + 1) == 'x' && isdigit(*(current + 2))) {
						// escape sequence can be a maximum of 2 chars long
                    	char str[2] = { *(current + 2), *(current + 3) };
                    	char* ptr;
						// convert the octal literal to binary
						*current = (char) strtol(str, &ptr, 16);
						deleteChars(current + 1, ptr - str);

                    } else
						deleteChar(current);

					//current++;
                }
                current++;

            }

            //current++;


            if (*current == '\0') // Reached the end of the string.
                goto finalize;

            if (*(current - 1) == '\\' && !lastWasSlash) {
                // Escaped quote (keep going)
                current++;
                continue;
            }

            // Reached the ending quote.
            goto finalize;
        }
    }


    // Not quoted so run till we see a space.
    while (*current && !isspace(*current))
        current++;

finalize:

    if (*current) {
        // Close token if not closed already.
        *current = '\0';
        current++;
        // Eat trailing whitespace.
        while (*current && isspace(*current))
            current++;
    }

    *next = current;

    //return unescapeToken(start);
    return start;

}


#endif
