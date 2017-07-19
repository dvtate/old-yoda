#ifndef RPN_CORE_H
#define RPN_CORE_H

#include <iostream>
#include <stack>
#include <queue>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <inttypes.h>

// this is the class used in our stack
#include "calc_value.hpp"

// user defined variables
#include "user_variables.hpp"

// some useful functions
#include "utils.hpp"

#include "string_stack.hpp"

// will be the error code given when a function return successfully
// this content is set so that if they use it out of context they are still told so
const char* lambda_finish = "\aERROR: use of `return` or `break` out of context :/";


#include "process_line.hpp"

// don't we all love pretty colors :)))
#include "terminal_colors.hpp"


extern char* metaName;
extern int line;

FILE* program = stdin;
char* progName = NULL;


void runFile(char* programFile, bool& errorReporting){

	progName = programFile;

	program = fopen(programFile, "r");

	// file error
	if (program == NULL) {

		setTermEffect(TERM_EFF_BOLD);

		std::cerr <<metaName <<": ";
		color_fputs(stderr, "error: ", 255, 0, 0);
		std::cerr <<": could not open file \'" <<programFile <<"\'\n";

		setTermEffect();

		exit(EXIT_FAILURE);

	}


	// the most important component of the program :)
	std::stack<CalcValue> mainStack;

	UserVar first_node(NULL, " ", 0.0);
	first_node.first = &first_node;

	std::vector<UserVar> var_nodes;
	var_nodes.push_back(first_node);

	bool elseStatement = false;

	std::vector<void*> freeable;



	char* rpnln = NULL;
	char* rpnln_head = rpnln;
	size_t lineLen = 0;



	// for each line in the programFile...
	for (;;) {

		// used for line numbers in errors
		line++;

		rpnln = rpnln_head;

		// here there be bugs
		if (getline(&rpnln, &lineLen, program) == -1) { // EOF
			return;
		}

		// I need a copy of it to call free() on later.
		rpnln_head = rpnln;

		char *errorToken = NULL;
		// process the line
		if ((errorToken =
					 processLine(mainStack, var_nodes,errorReporting, rpnln, elseStatement, program, freeable))
			&& errorReporting
				) {


			// file name and
			setTermEffect(TERM_EFF_BOLD);
			std::cerr <<"^^ in " <<programFile <<":" <<line <<':' <<errorToken - rpnln_head <<"\n";
			setTermEffect();

			// print the problem statement
			color_fprintf(stderr, 255, 0, 0, "\t%s", getLineFromFile(programFile, line));

			// point to the problem area
			//for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
			//	std::cerr <<' ';

			//color_fputs(stderr, "^\n", 255, 0, 0);

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

			// you're dead :P
			exit(EXIT_FAILURE);

		}

	}

}

bool runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
			 std::stack<CalcValue>& mainStack, bool& elseStatement, std::vector<void*>& freeable
) {

	if (!prog_file)
		return true;


	size_t local_line = 0;

	char* rpnln = NULL, *rpnln_head = rpnln;
	size_t lineLen = 0;

	// for each line in the programFile...
	for (;;) {

		// used for line numbers in errors
		local_line++;

		rpnln = rpnln_head;
		if (getline(&rpnln, &lineLen, prog_file) == -1) {
			//free(rpnln);
			return false; // EOF
		}

		rpnln_head = rpnln;
		// I need a copy of it to call free() on later.
		char* errorToken = NULL;
		// process the line
		if ((errorToken =
					 processLine(mainStack, var_nodes, errorReporting, rpnln, elseStatement, prog_file, freeable))
			&& errorReporting
				) {

			if (errorToken == lambda_finish)
				goto cleanup_end;

			// file name and
			setTermEffect(TERM_EFF_BOLD);
			std::cerr <<"^^ in " <<progName <<":local_line=" <<local_line
			          <<":col=" <<errorToken - rpnln_head <<"?\n";

			setTermEffect();

			// print the problem statement
			rewind(prog_file);
			color_fprintf(stderr, 255, 0, 0, "\t%s\t",
						  getLineFromFile(prog_file, local_line));

/*
			// point to the problem area
			for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
				std::cerr <<' ';

			color_fputs(stderr, "^\n", 255, 0, 0);
*/


			// prevent memory leaks...
			free(rpnln_head);
			return true;
		}
		free(rpnln_head);
		rpnln = rpnln_head = NULL;
	}

cleanup_end:
	// prevent memory leaks...
	free(rpnln_head);
	return false;

}


void runShell(std::vector<UserVar>& var_nodes, bool& errorReporting,
			  std::stack<CalcValue>& mainStack, bool& elseStatement, std::vector<void*> freeable
){

	std::cout <<line++ <<">>> ";

	char* rpnln = (char*) malloc(256);
	size_t lineLen = 256;

	if (getline(&rpnln, &lineLen, stdin) == -1) {
		//std::cerr <<"\aERROR: Input failed... email toast27@gmail.com if this persists\n\n";
		std::cout <<"\nGood-bye!\n";
		exit(EXIT_SUCCESS);
		return;
	}

	// I need a copy of it to call free() on later.
	char* rpnln_head = rpnln;

	// process the line
	bool errors = processLine(mainStack, var_nodes, errorReporting, rpnln, elseStatement, stdin, freeable);

	if (errors)
		emptyStack(mainStack);

	// prevent memory leaks...
	free(rpnln_head);

	// this fails...
	if (!mainStack.empty()) {
		if (!printCalcValue(mainStack.top(), var_nodes))
			std::cout <<'\n';
	}

	std::cout <<std::endl;

}


#endif
