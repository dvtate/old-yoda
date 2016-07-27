#ifndef RPN_CORE_H
#define RPN_CORE_H

#include <iostream>
#include <stack>
#include <queue>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>

// this is the class used in our stack
#include "calc_value.h"

// user defined variables
#include "user_variables.h"

// some useful functions
#include "utils.h"


#include "process_line.h"

void runFile(FILE*);

/*
void runStringStack(StrStack&);
*/

void runShell(UserVar* first_node, bool& errorReporting){


	static CalcValue ans(0.0); // here `0` could be a pointer


	std::stack<CalcValue> mainStack;

	// used for storing the name for user variables on a line by line basis
	std::queue<char*> varNames;

	//if (!nestedIf)
	std::cout <<line++ <<">>> ";

	char* rpnln = (char*) malloc(256);
	size_t lineLen = 256;

	if (getline(&rpnln, &lineLen, stdin) == -1) {
		std::cerr <<"\aERROR: Input failed...\n" <<std::endl;
		return;
	}

	// I need a copy of it to call free() on later.
	char* rpnln_head = rpnln;


	// process the line
	processLine(mainStack, first_node, varNames, errorReporting, rpnln, lineLen);


	// prevent memory leaks...
	free(rpnln_head);


	if (!mainStack.empty()) {
		ans = mainStack.top();
		if (ans.type == CalcValue::NUM)
			std::cout <<"ans " <<ans.getNum() <<" =\n";
		else
			std::cout <<"ans \"" <<ans.getStr() <<"\" =\n";
	}



}



#endif
