#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdlib>
#include <cstring>
#include <ctype.h>



// removes char from string from it's pointer
// this is extremely inefficient...
static inline void deleteChar(char *toDelete)
	{ memmove(toDelete, toDelete + 1, strlen(toDelete)); }

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

                    } else if (*(current + 1) == 'r') {
                        *current = *(current + 1) = '\r';
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
                    } else if (*(current + 1) == '\'') {
                        *current = '\'';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == '\"') {
                        *current = '\"';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == '\\') {
                        *current = '\\';
                        deleteChar(current + 1);
                        lastWasSlash = true;
                    }
                    //current++;
                }
                current++;

            }

            //current++;


            if (*current == '\0') // Reached the end of the string.
                goto finalize;

            if (*(current - 1) == '\\' && lastWasSlash == false) {
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
