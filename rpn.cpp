#include <iostream>
#include <stack>
#include <queue>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>


#include "string_stack.h"

// this is the class used in our stack
#include "calc_value.h"

// user defined variables
#include "user_variables.h"


// some useful functions
#include "utils.h"

unsigned int line = 0;

// this will get removed in the rewrite...
uint16_t nestedIf = 0;

#include "process_line.h"
#include "core.h"


/*This file is about to go through the biggest rewrite I've ever done:
* - main will take in cmdline options for the name of the file to run
* - if none are given then it will go to the interactive shell (currently the only option)
*/

int argc_cpy;
char** argv_cpy;

CalcValue ans = 0.0;

int main(int argc, char** argv){

	argc_cpy = argc;
	argv_cpy = argv;

	// give 'em da shell
	if (argc == 1) {

		UserVar* first_node = new UserVar(" ", 0.0);
		bool showErrors = true;


		std::stack<CalcValue> mainStack;

		// used for storing the name for user variables on a line by line basis
		std::queue<char*> varNames;

		for (;;)
			runShell(first_node, showErrors, mainStack, varNames);


	// hit 'em up wit dat version info
	} else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version")  == 0) {
		std::cout <<"Scripting language - v0.0.1" <<std::endl
				  <<"Copyright (C) 2016 Dustin Van Tate Testa <toast27@gmail.com>\n"
				  <<"This is free software; see the source for copying conditions.\n\
There is NO warranty; not even MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";
		return 0;

	// help a brotha out
	} else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
		std::cout <<"Usage: a.out [ option | file ] ...\nOptions:"
				  <<"-h\t: display's this help message (also --help)\n"
				  <<"-V\t: display's version information (also --version)\n\n";

		displayHelp();
		return 0;
	} else {
		FILE* program = fopen(argv[1],"r");
		if (program == NULL) {
			std::cerr <<*argv <<": could not open file \'" <<argv[1] <<'\'' <<std::endl;
			return 1;
		} else {
			bool showErrors = true;
			runFile(program, showErrors);
		}

	}

}
