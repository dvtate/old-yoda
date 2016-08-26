#ifndef CONDITIONALS_H
#define CONDITIONALS_H


#include <iostream>
#include <stdlib.h>
#include <stack>
#include <queue>
#include <inttypes.h>
#include <string.h>


#include "calc_value.h"

#include "process_line.h"


/*	WARNING:
* This code is terribly broken. I wrote this without any sleep and I'm suprised
* I even got it to compile. It might be best to do a total rewrite. Please feel
* free to fix my mistakes. I've done a pretty shitty job with this... I'm going
* to sleep now.
*/


extern FILE* program;

extern char* processLine(
	std::stack<CalcValue>& mainStack, UserVar* first_node,
	bool& showErrors, char*& rpnln
);

extern void runStringStack(
	StrStack& code, bool& errorReporting, std::stack<CalcValue>& mainStack,
	UserVar* first_node
);



char* endif(char*& string){
	if (string && *string) {
		do {
			if (*string == ':') {
				if (*++string == '?')
					return ++string;

			} else if (*string == '#')
				return NULL;
		} while (*++string != '\0');
	}
	return NULL;
}

// if elseif else
char* conditional(char* str,
				  std::stack<CalcValue>& mainStack,
				  UserVar* first_node,
				  bool& showErrors
){
	bool triggered = false;

	bool condition = !mainStack.empty() && mainStack.top().getNum();
	if (!mainStack.empty())
		mainStack.pop();

	// copy the string into a new stack
	size_t lineLen = strlen(str) + 1;
	char* string = (char*) malloc(lineLen); // make a copy of this for modification to prevent problems
	strcpy(string, str);

	char* string_head = string;


process_condition:


	// run condition and ignore the others
	if (!triggered && condition) {

		triggered = true;

		// holds the if statement code
		StrStack stk;
		//std::cout <<"triggered XDDDDDDDD\n";

		while (*string != '#' && *string != '\0' &&
			   !(*string == ':' && *(string + 1) == '?'))
			string++;

		if (*string == ':') {
			*string = '\0';
			processLine(mainStack, first_node, showErrors, string_head);
			*string += 3;
		} else {
			*string = '\0';
			stk.push(string_head);

			for (;;) {
				string = string_head;

				if (getline(&string, &lineLen, program) == -1) { // EOF
					std::cerr <<"\aERROR: unterminated conditional; missing `:?`\n";
					return 0;
				} else if (endif(string) != NULL) {
					stk.push(string);
					break;
				}

			}

			runStringStack(stk, showErrors, mainStack, first_node);


		}

	} else { // skip this one

		for (;;) {
			while (*string != '#' && *string != '\0' &&
				   !(*string == ':' && *(string + 1) == '?'))
				string++;


			if (*string == ':') {
				string += 2;
				break;
			// another line
			} else {
				string = string_head;
				if (getline(&string, &lineLen, program) == -1) { // EOF
					std::cerr <<"\aERROR: unterminated conditional; missing `:?`\n";
					return 0;
				}


			}
		}

	}

	char* string_cpy = string;

	while (*string_cpy != '\0' && *string_cpy != '#'
			&& *string_cpy != '?' && *(string_cpy + 1) != ':')
		string_cpy++;

	if (*string_cpy == '\0')
		return string_cpy;
	if (*string_cpy == '#')
		return 0;

	uint32_t condition_len = string_cpy - string;

	char* strHolder = (char*) malloc(condition_len + 1);

	// copy the string
	strncpy(strHolder, string, condition_len);

	// add NULL-terminator
	*(strHolder + condition_len) = '\0';



	processLine(mainStack, first_node, showErrors, strHolder);

	condition = !mainStack.empty() && mainStack.top().getNum();
	if (!mainStack.empty())
		mainStack.pop();


	goto process_condition;

	free(strHolder);
	//free(string_head);
	return NULL;

}




#endif
