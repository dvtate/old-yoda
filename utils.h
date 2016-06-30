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
    CalcValue topVal;
    if (!valStack.empty()) {
        topVal = valStack.top();
        valStack.pop();
        return topVal;
    } else {
        std::cerr <<"\aERROR: not enough data to satisfy operator.\n" <<std::endl;
        return main();
    }
}


// removes char from string from it's pointer
static inline void deleteChar(char *word, char *toDelete){
    memmove(toDelete, toDelete + 1, strlen(toDelete));
}

// Returns the end of the token, without changing it.
char* qtok(char* str, char** next){
    char *current = str, *start = str;

    bool isQuoted = false;
    bool lastWasSlash = false;

    // Eat beginning whitespace.
    while (*current && isspace(*current))
        current++;

    start = current;

    if (*current == '"') {

        isQuoted = true;

        // Quoted token
        start = current;
        current++; // Skip the beginning quote.


        for (;;) {
            // Go until we find a quote or the end of string.
            while (*current != '\0'  && *current != '"') {
                lastWasSlash = false;
                
                // this if statement contains code that might not be portable...
                if (*current == '\\') {
                    if (*(current + 1) == 'n') {
                        *current = '\n';
                        *(current + 1) = '\r';
                        deleteChar(str, current + 1);
                    } else if (*(current + 1) == 'r') {
                        *current = *(current + 1) = '\r';
                        current++;
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
                        deleteChar(str, current + 1);
                    } else if (*(current + 1) == '\\') {
                        *current = '\\';
                        deleteChar(str, current + 1);
                        lastWasSlash = true;
                    }
                    //current ++;
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

/*
// Returns the end of the token, without changing it.
char* qtok(char* str, char** next, bool& isQuoted){
    if (strlen(str) == 0)
        return NULL;

    char *current = str;
    char *start = str;

    isQuoted = false;

    // Eat beginning whitespace.
    while (*current && isspace(*current))
        current++;

    start = current;

    if (*current == '"') {

        isQuoted = true;

        // Quoted token
        current++; // Skip the beginning quote.
        start = current;

        for (;;) {
            // Go until we find a quote or the end of string.
            while (*current && (*current != '"'))
                current++;
            if (!*current) // Reached the end of the string.
                goto finalize;

            if (*(current - 1) == '\\') {
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
        *current = 0;
        current++;
        // Eat trailing whitespace.
        while (*current && isspace(*current))
            current++;
    }

    *next = current;

    return unescapeToken(start);

}

*/

#endif
