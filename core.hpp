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
#include "calc_value.hpp"

// user defined variables
#include "user_variables.hpp"

// some useful functions
#include "utils.hpp"

#include "string_stack.hpp"

#include "process_line.hpp"

// don't we all love pretty colors :)))
#include "terminal_colors.h"


extern char* metaName;


FILE* program = stdin;
char* progName = NULL;


void runFile(char* programFile, bool& errorReporting){

	progName = programFile;

	program = fopen(programFile, "r");

  	// file error
	if (program == NULL) {

		textEffect(TERM_CLR_BRIGHT);
	  	std::cerr <<metaName <<": " <<COLOR_RED <<"error: " <<COLOR_RESET;
	  	std::cerr <<": could not open file \'" <<programFile <<'\'' <<std::endl;
		exit(EXIT_FAILURE);
	}


	// the most important component of the program :)
	std::stack<CalcValue> mainStack;

	// used for storing the name for user variables on a line by line basis
	std::queue<char*> varNames;


	UserVar* first_node = new UserVar(NULL, " ", 0.0);
  	first_node->first = first_node;

	static CalcValue ans(0.0); // here `0` could be a pointer

	// for each line in the programFile...
	for (;;) {

	  	// used for line numbers in errors
		line++;


		char* rpnln = (char*) malloc(256);
		size_t lineLen = 256;


		if (getline(&rpnln, &lineLen, program) == -1)
			return; // EOF

		// I need a copy of it to call free() on later.
		char	*rpnln_head = rpnln,
				*errorToken = NULL;
		// process the line
		if ((errorToken =
			processLine(mainStack, first_node,errorReporting, rpnln))
			&& errorReporting
		) {

		  	// file name and
		  	textEffect(TERM_CLR_BRIGHT);
			std::cerr <<programFile <<":" <<line <<':' <<errorToken - rpnln_head <<':';
			textEffect();

			// print the problem statement
			std::cerr <<'\t' <<COLOR_RED <<getLineFromFile(programFile, line) <<'\t';

		  	// point to the problem area
		  	while (rpnln_head++ != rpnln)
				std::cout <<' ';
			std::cout <<'^' <<COLOR_RESET <<std::endl;

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

		  	// you're dead :P
			exit(EXIT_FAILURE);

		}

		// prevent memory leaks...
		free(rpnln_head);
	}

	// windows sucks :P
	#ifdef _WIN32
		std::cin.ignore();
	#endif

}



// a NULL CalcValue
CalcValue ans;


void runShell(UserVar* first_node, bool& errorReporting,
	      std::stack<CalcValue>& mainStack
){



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
	bool errors = processLine(mainStack, first_node, errorReporting, rpnln);

	if (errors)
		emptyStack(mainStack);


	// prevent memory leaks...
	free(rpnln_head);

	if (!mainStack.empty()) {
		ans = mainStack.top();
		//ans.print(first_node);
		if (!printCalcValue(ans, first_node))
			std::cout <<'\n';
	}

   	// this allows the ans keyword to function
	if (!mainStack.empty())
		ans = mainStack.top();

	std::cout <<std::endl;

}

void runStringStack(StrStack& code, bool& errorReporting){


	// the most important component of the program :)
	std::stack<CalcValue> mainStack;

	// used for storing the name for user variables on a line by line basis
	std::queue<char*> varNames;

	UserVar* first_node = new UserVar(NULL, " ", 0.0);
  	first_node->first = first_node;

	static CalcValue ans(0.0); // here `0` could be a pointer


	char** stackHead = code.stackHead;

	// for each line in the string stack...
	for (size_t i = 0; i < code.stackDepth; i++) {

	  	// used for line numbers in errors
		line++;



		char* rpnln = *(stackHead++);

		// I need a copy of it to call free() on later.
		char	*rpnln_head = rpnln,
				*errorToken = NULL;


		// process the line
		if ((errorToken =
			processLine(mainStack, first_node, errorReporting, rpnln))
			&& errorReporting
		) {

		  	// file name and
		  	textEffect(TERM_CLR_BRIGHT);
			std::cerr <<progName <<":" <<line <<':' <<errorToken - rpnln_head <<':';
			textEffect();

			// print the problem statement
			std::cerr <<'\t' <<COLOR_RED <<getLineFromFile(progName, line) <<'\t';

		  	// point to the problem area
		  	while (rpnln_head++ != rpnln)
				std::cout <<' ';
			std::cout <<'^' <<COLOR_RESET <<std::endl;

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

		  	// you're dead :P
			exit(EXIT_FAILURE);


		}


	}

}

void runStringStack(
	StrStack& code, bool& errorReporting, std::stack<CalcValue>& mainStack,
	UserVar* first_node
){

	static CalcValue ans(0.0); // here `0` could be a pointer


	char** stackHead = code.stackHead;

	// for each line in the string stack...
	for (size_t i = 0; i < code.stackDepth; i++) {

	  	// used for line numbers in errors
		line++;



		char* rpnln = *(stackHead++);

		// I need a copy of it to call free() on later.
		char	*rpnln_head = rpnln,
				*errorToken = NULL;


		// process the line
		if ((errorToken =
			processLine(mainStack, first_node, errorReporting, rpnln))
			&& errorReporting)
		{

		  	// file name and
		  	textEffect(TERM_CLR_BRIGHT);
			std::cerr <<progName <<":" <<line <<':' <<errorToken - rpnln_head <<':';
			textEffect();

			// print the problem statement
			std::cerr <<'\t' <<COLOR_RED <<getLineFromFile(progName, line) <<'\t';

		  	// point to the problem area
		  	while (rpnln_head++ != rpnln)
				std::cout <<' ';
			std::cout <<'^' <<COLOR_RESET <<std::endl;

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

		  	// you're dead :P
			exit(EXIT_FAILURE);

		}

	}

}


#endif
