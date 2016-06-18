#ifndef RPN_UTILS_H
#define RPN_UTILS_H

#include "calc_value.h"


#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stack>


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

// Strips backslashes from quotes
char *unescapeToken(char *token){
    char *in = token;
    char *out = token;

    while (*in) {
        //assert(in >= out);

        if (in[0] == '\\' && (in[1] == '"' || in[1] == '\'')) {
            *out = in[1];
            out++;
            in += 2;
        } else {
            *out = *in;
            out++;
            in++;
        }
    }
    *out = 0;
    return token;
}

// Returns the end of the token, without changing it.
char *qtok(char *str, char **next, bool& isQuoted){
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
        for (;;){
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


// Returns the end of the token, without changing it.
char *qtok(char *str, char **next){
    char *current = str;
    char *start = str;

  	bool isQuoted = false;

    // Eat beginning whitespace.
    while (*current && isspace(*current))
        current++;

    start = current;

    if (*current == '"') {

        isQuoted = true;

        // Quoted token
        current++; // Skip the beginning quote.
        start = current;
        for (;;){
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


#endif
