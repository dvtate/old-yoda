#include <iostream>
#include <stack>
#include <queue>
#include <cstring>

#ifdef _WIN32
	#include "fuck_windows.h"
#endif

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


int argc_cpy;
char** argv_cpy;


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
		bool showErrors = true;
		runFile(argv[1], showErrors);


		// windows sucks :P
		#ifdef _WIN32
			std::cin.ignore();
		#endif


	}

}
