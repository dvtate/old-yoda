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

#include "process_line.hpp"

// don't we all love pretty colors :)))
#include "terminal_colors.hpp"


extern char* metaName;


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

	UserVar* first_node = new UserVar(NULL, " ", 0.0);
  	first_node->first = first_node;

	bool elseStatement = false;
	char *rpnln = (char*) malloc(256), *rpnln_head = rpnln;
	size_t lineLen = 256;


	// for each line in the programFile...
	for (;;) {

	  	// used for line numbers in errors
		line++;

		if (getline(&rpnln, &lineLen, program) == -1) {
			// prevent memory leaks...
			delete first_node;
			fclose(program);

			return; // EOF
		}
		rpnln_head = rpnln;

		// I need a copy of it to call free() on later.
		char *errorToken = NULL;
		// process the line
		if ((errorToken =
			processLine(mainStack, first_node,errorReporting, rpnln, elseStatement, program))
			&& errorReporting
		) {

		  	// file name and
		  	setTermEffect(TERM_EFF_BOLD);
			std::cerr <<programFile <<":" <<line <<':' <<errorToken - rpnln_head <<":\n";
			setTermEffect();

			// print the problem statement
			color_fprintf(stderr, 255, 0, 0, "\t%s\t", getLineFromFile(programFile, line));

		  	// point to the problem area
			for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
				std::cerr <<' ';

			color_fputs(stderr, "^\n", 255, 0, 0);

			delete first_node;
			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif
		  	// you're dead :P
			exit(EXIT_FAILURE);

		}

	}

}

bool runFile(FILE* prog_file, UserVar* first_node, bool& errorReporting,
	      std::stack<CalcValue>& mainStack, bool& elseStatement
) {

	if (!prog_file)
		return true;

	size_t local_line = 0;

	char* rpnln = NULL, *rpnln_head = rpnln;
	size_t lineLen = 256;

	// for each line in the programFile...
	for (;;) {

	  	// used for line numbers in errors
		local_line++;

		if (getline(&rpnln, &lineLen, prog_file) == -1) {
			//free(rpnln);
			return false; // EOF
		}
		rpnln_head = rpnln;
		// I need a copy of it to call free() on later.
		char* errorToken = NULL;
		// process the line
		if ((errorToken =
			processLine(mainStack, first_node,errorReporting, rpnln, elseStatement, prog_file))
			&& errorReporting
		) {

		  	// file name and
		  	setTermEffect(TERM_EFF_BOLD);
			std::cerr <<progName <<":" <<line - linesToEnd(prog_file)
					  <<':' <<errorToken - rpnln_head <<":\n";
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

	// prevent memory leaks...
	free(rpnln_head);
	return false;

}


// a NULL CalcValue
CalcValue ans;

void runShell(UserVar* first_node, bool& errorReporting,
	      std::stack<CalcValue>& mainStack, bool& elseStatement
){

	std::cout <<line++ <<">>> ";

	char* rpnln = (char*) malloc(256);
	size_t lineLen = 256;

	if (getline(&rpnln, &lineLen, stdin) == -1) {
		std::cerr <<"\aERROR: Input failed... email toast27@gmail.com if this persists\n\n";
		return;
	}

	// I need a copy of it to call free() on later.
	char* rpnln_head = rpnln;


	// process the line
	bool errors = processLine(mainStack, first_node, errorReporting, rpnln, elseStatement, stdin);

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

// this is a shitty function
void runStringStack(StrStack& code, bool& errorReporting){


	// the most important component of the program :)
	std::stack<CalcValue> mainStack;

	UserVar* first_node = new UserVar(NULL, " ", 0.0);
  	first_node->first = first_node;

	static CalcValue ans(0.0); // here `0` could be a pointer

	bool elseStatement = false;

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
			processLine(mainStack, first_node, errorReporting, rpnln, elseStatement, stdin)) // note: stdin is a bad file for this purpose..
			&& errorReporting
		) {



		  	// file name and
		  	setTermEffect(TERM_EFF_BOLD);
			std::cerr <<progName <<":" <<line <<':' <<errorToken - rpnln_head <<":\n";
			setTermEffect();

			// print the problem statement
			color_fprintf(stderr, 255, 0, 0, "\t%s\t", getLineFromFile(progName, line));

		  	// point to the problem area
		  	for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
				std::cerr <<' ';
			color_fputs(stderr, "^\n", 255, 0, 0);

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

		  	// you're dead :P
			exit(EXIT_FAILURE);


		}
	}
}


/* this will no longer get used
// this is part of a ghetto solution to make getStrStack() work in nested-blocks
struct current_block_data_t {
	char** stackHead;
	ssize_t linesLeft;
} curStrStack = { NULL, 0 };


/// this is still pretty ghetto...
bool runStringStack(
	StrStack& code, bool& errorReporting, std::stack<CalcValue>& mainStack,
	UserVar* first_node
){

	bool elseStatement = false;
	char	*rpnln_head;

	//static CalcValue ans(0.0); // here `0` could be a pointer


  	// used for line numbers in errors (plus previosly mentioned kludge)
	curStrStack.linesLeft = code.stackDepth;

	curStrStack.stackHead = code.stackHead;

	// 500 chars/line is a reasonable max
	char* rpnln = rpnln_head= (char*) malloc(512);

	// for each line in the string stack...
	for (; curStrStack.linesLeft > 0; curStrStack.linesLeft--) {

		// copy line to prevent corruption
		rpnln = (char*) realloc(rpnln_head, strlen(*curStrStack.stackHead) + 1);
		strcpy(rpnln, *(curStrStack.stackHead++));


		rpnln_head = rpnln;

		// I need to delete it later.
		char *errorToken = NULL;

		// process the line
		if ((errorToken =
			processLine(mainStack, first_node, errorReporting, rpnln, elseStatement, stdin))
			&& errorReporting)
		{
			// why the fuck doesn't this get run ???????
		  	setTermEffect(TERM_EFF_BOLD);
			std::cerr <<progName <<": block : " <<code.stackDepth - curStrStack.linesLeft
					  <<" : " <<rpnln - rpnln_head << ":\n";

			setTermEffect();

			// print the problem statement
			//color_fprintf(stderr, 255, 0, 0, "\t%s\t", getLineFromFile(progName, line));
			color_fprintf(stderr, 255, 0, 0, "\t%s\t", trimStr(*(curStrStack.stackHead - 1)));
		  	// point to the problem area
		  	for (size_t i = rpnln - rpnln_head; i ; i-- )
				std::cerr <<' ';
			color_fputs(stderr, "^\n", 255, 0, 0);

			// windows sucks :P
			#ifdef _WIN32
				std::cin.ignore();
			#endif

			curStrStack = { NULL, 0 };
			free(rpnln_head);

			// stop execution on error
			return true;

		}

		//if ()
	}

	curStrStack = { NULL, 0 };
	free(rpnln_head);
	return false;
}
*/


#endif
