#include <iostream>
#include <stack>
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

unsigned int line = 1;

#include "core.hpp"

#include "process_line.hpp"

#include "lolcat_version.h"

char* metaName;


int main(int argc, char** argv){

	metaName = *argv;

	// having errors should be the default
	bool showErrors = true;

	// shell
	if (argc == 1) {

		// set up a namespace for variables
		UserVar first_node(NULL, " ", 0.0);
		first_node.first = &first_node;

		std::vector<UserVar> var_nodes;
		var_nodes.push_back(first_node);

		bool elseStatement = false;

		// the most important component to the language
		std::stack<CalcValue> mainStack;

		// process commands as they come in
		for (;;)
			runShell(var_nodes, showErrors, mainStack, elseStatement);



		// version info
	} else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version")  == 0) {
		printVersionInfo();

		// help
	} else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
		std::cout <<"Usage: yoda [ option | file ] ...\nOptions:"
				"   -h,\t--help\t: display's this help message (also --help)\n"
				"   -V,\t--version\t: display's version information (also --version)\n\n";

		displayHelp();

		// file
	} else {

		runFile(argv[1], showErrors);

		// windows sucks :P
#ifdef _WIN32
		std::cin.ignore();
#endif

	}

}