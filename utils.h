#ifndef RPN_UTILS_H
#define RPN_UTILS_H

#include "calc_value.h"


#include <iostream>
#include <cstdio>
#include <ctype.h>
#include <stack>
#include <cstring>

extern int main();



inline void displayHelp(){
    std::cout <<"\n\t\tRPN Calculator\nPlace the operator after its two operands. \
Here is an example:\n > 1 1 + \n ans = 2\n\nTo use the previous answer \
replace one number with `ans` as in the following example:\n \
> ans\n ans = 2\n\nWhen you are finished, type `q` or `exit` to exit the program.\n"
              <<std::endl;
}


CalcValue getNextValue(std::stack<CalcValue>& valStack){

  	if (!valStack.empty()) {
		CalcValue topVal = valStack.top();
        valStack.pop();
        return topVal;
    } else {
        std::cerr <<"\aERROR: not enough data to satisfy operator.\n" <<std::endl;
        return main();
    }
}



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
                        *(current + 1) = '\r';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'r') {
                        *current = *(current + 1) = '\r';
                        current++;
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'a') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
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

template <class T>
inline void emptyStack(std::stack<T>& stk){
	while (!stk.empty())
		stk.pop();
}

inline char* skipSpaces(char* p){
	while (isspace(*p))
		p++;

	return p;
}

#endif
