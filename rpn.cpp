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
uint8_t nestedIf = 0;

#include "process_line.h"
#include "core.h"

char* metaName;

int main(int argc, char** argv){

	metaName = *argv;

	// give 'em da shell
	if (argc == 1) {



		UserVar* first_node = new UserVar(NULL, " ", 0.0);
		first_node->first = first_node;

		bool showErrors = true;


		std::stack<CalcValue> mainStack;

		// used for storing the name for user variables on a line by line basis
		std::queue<char*> varNames;

		for (;;)
			runShell(first_node, showErrors, mainStack, varNames);


	// hit 'em up wit dat version info
	} else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version")  == 0) {
		std::cout <<"Stack Based Scripting Language  - v0.0.2-alpha\n\
Copyright (C) 2016 Dustin Van Tate Testa <toast27@gmail.com>\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation.\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n\
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License\n\
along with this program. If not, see <http://www.gnu.org/licenses/>\n";

	// help a brotha out
	} else if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0) {
		std::cout <<"Usage: a.out [ option | file ] ...\nOptions:"
					"   -h\t\t: display's this help message (also --help)\n"
					"   -V\t\t: display's version information (also --version)\n\n";

		displayHelp();

	// run a file
	} else {

		metaName = *argv;

		bool showErrors = true;
		runFile(argv[1], showErrors);


		// windows sucks :P
		#ifdef _WIN32
			std::cin.ignore();
		#endif


	}

	return 0;

}
