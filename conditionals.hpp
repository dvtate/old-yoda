#ifndef CONDITIONALS_H
#define CONDITIONALS_H


#include <iostream>
#include <stdlib.h>
#include <stack>
#include <queue>
#include <inttypes.h>
#include <string.h>


#include "calc_value.hpp"
#include "string_stack.hpp"
#include "process_line.hpp"


/*	WARNING:
* This code is terribly broken. but I think I'm going to be able to fix it now :)))
*/


extern FILE* program;

extern unsigned int line;

extern char* processLine(
	std::stack<CalcValue>& mainStack, UserVar* first_node,
	bool& showErrors, char*& rpnln
);

extern void runStringStack(
	StrStack& code, bool& errorReporting, std::stack<CalcValue>& mainStack,
	UserVar* first_node
);




inline char* ignoreConditional(char*& str) {
	char* p;
	uint8_t depth;
	size_t lineLen = 256; // same value as in core.h

	p = str;
	while (*p && depth != 0) {
		if (*p == '?' && *(p + 1) == ':') {
			depth++;
			p += 2;
		} else if (*p == ':' && *(p + 1) == '?') {
			depth--;
			p += 2;
		} else
			p++;
	}

	while (depth) {
		str = (char*) realloc(str, lineLen);
		if (getline(&str, &lineLen, program) == -1) {
			std::cerr <<"\aERROR: unterminated conditional; missing `:?`\n";
			return str;
		}

		p = str;
		while (*p && depth != 0) {
			if (*p == '?' && *(p + 1) == ':') {
				depth++;
				p += 2;
			} else if (*p == ':' && *(p + 1) == '?') {
				depth--;
				p += 2;
			} else
				p++;
		}
	}
	str = p;
	return NULL;
}


inline char* pushConditional(char*& str, StrStack& block){
	uint8_t depth = 1;
	size_t lineLen = strlen(str) + 1; // same value as in core.h

	char* p = (char*) malloc(lineLen);
	strcpy(p, str);

	while (*p && depth != 0) { // for each character
		if (*p == '?' && *(p + 1) == ':') {
			depth++;
			p += 2;
		} else if (*p == ':' && *(p + 1) == '?') {
			depth--;
			p += 2;
		} else
			p++;

	}

	while (depth) { // for each line
		line++;
		str = (char*) malloc(lineLen);
		if (getline(&str, &lineLen, program) == -1) {
			std::cerr <<"\aERROR: unterminated conditional; missing `:?`\n";
			return str;
		}

		p = str;
		while (*p && depth != 0) { // for each character
			if (*p == '?' && *(p + 1) == ':') {
				depth++;
				p += 2;
			} else if (*p == ':' && *(p + 1) == '?') {
				depth--;
				p += 2;
			} else
				p++;

		}


	}

	// copy the portion of the line inside the if-block
	if (depth == 0 && *p) {
		// copy to a container string
		size_t len = (p - 2) - str;
		char lineInsideIf[len + 1];
		strncpy(lineInsideIf, str, len);
		lineInsideIf[len] = 0;

		block.push(lineInsideIf);
	} else
		block.push(str);



	str = p;

	return NULL;

}

char* conditional(char*& str,
				  std::stack<CalcValue>& mainStack,
				  UserVar* first_node,
				  bool& showErrors
){

	StrStack toRun;
	bool canRun;

	// process the first condition
	if (!mainStack.top().getNum()) {
		mainStack.pop();
		canRun = true;
		char* err = pushConditional(str, toRun);
		if (err != NULL)
			return err;

		// single block if statement
		if (!*str)
			goto conditional_endif;
	} else {
		mainStack.pop();
		char* err = ignoreConditional(str);
		if (err != NULL)
			return err;
		canRun = false;
	}



	// process the others
	while (*str) {

		// find the length of the condition
		char* p = str;
		while (*p && (*p != '?' && *(p + 1) != ':')) {
			p++;
		}

		if (!*p) // endif if there is no elseif
			goto conditional_endif;

		// assign the condition to a string
		size_t len = (p - 1) - str;
		char cond[len + 1];
		strncpy(cond, str, len);
		cond[len] = 0;
		char* condCpy = cond;


		// push the condition to the stack
		char* err = processLine(mainStack, first_node, showErrors, condCpy);
		if (err != NULL)
			return err;

		str = p;

		// process the first condition
		if (!canRun && !mainStack.top().getNum()) {
			mainStack.pop();
			canRun = true;
			char* err = pushConditional(str, toRun);
			if (err != NULL)
				return err;

			// single block if statement
			if (!*str)
				goto conditional_endif;
		} else {
			mainStack.pop();
			char* err = ignoreConditional(str);
			if (err != NULL)
				return err;
		}

	}



// end of function/statement
conditional_endif:

	if (canRun)
		runStringStack(toRun, showErrors, mainStack, first_node);
	return NULL;

}

#endif
