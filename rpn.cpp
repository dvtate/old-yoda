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


#include "string_stack.hpp"

// this is the class used in our stack
#include "calc_value.hpp"

// user defined variables
#include "user_variables.hpp"


// some useful functions
#include "utils.hpp"

unsigned int line = 0;

#include "process_line.hpp"
#include "core.hpp"

#include "lolcat_version.h"

char* metaName;

int main(int argc, char** argv){

	metaName = *argv;

	// give 'em da shell
	if (argc == 1) {



		UserVar* first_node = new UserVar(NULL, " ", 0.0);
		first_node->first = first_node;

		bool showErrors = true;

		// the most important component to the language
		std::stack<CalcValue> mainStack;

		for (;;)
			runShell(first_node, showErrors, mainStack);


	// hit 'em up wit dat version info
	} else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version")  == 0) {
		printVersionInfo();

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
