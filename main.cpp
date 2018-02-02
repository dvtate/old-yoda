#include <iostream>
#include <stack>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

// re-implemented getline()
#include "win_supp.h"

#include "string_stack.hpp"

// this is the class used in our stack
#include "calc_value.hpp"

// user defined variables
#include "user_variables.hpp"


// some useful functions
#include "utils.hpp"

int line = 0;

#include "core.hpp"


char* metaName;
extern char* progName;


void handle_sigint_file(int);
void handle_sigint_shell(int);


int main(int argc, char** argv){

	metaName = *argv;

	// having errors should be the default
	bool showErrors = true;

	// shell
	if (argc == 1) {
		line = 1; // required for alignment

		// seed random
		srand(time(NULL));

		// set up a namespace for variables
		UserVar first_node(NULL, " ", 0.0);
		first_node.first = &first_node;

		std::vector<UserVar> var_nodes;
		var_nodes.reserve(2000); // overkill but it will prevent addresses from changing
		var_nodes.push_back(first_node);

		bool elseStatement = false;

		std::vector<void*> freeable;

		// the most important component to the language
		std::stack<CalcValue> mainStack;

		/*
		shell_vars.nodes = & var_nodes;
		shell_vars.elseStat = &elseStatement;
		shell_vars.showErrors = &showErrors;
		shell_vars.ms = &mainStack;
		*/

		// this handles Ctrl+C
		signal(SIGINT, handle_sigint_shell);

		// process commands as they come in
		for (;;)
			runShell(var_nodes, showErrors, mainStack, elseStatement, freeable);

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

		// seed random
		srand (time(NULL));

		// this handles Ctrl+C
		signal(SIGINT, handle_sigint_file);

		// start file
		runFile(argv[1], showErrors);


		// trash OS
#ifdef _WIN32
		std::cin.ignore();
#endif

	}
}


void handle_sigint_file(int){
	printf("In file: %s On line: %d\n", progName, --line);

	char* ln = fileutils::getLineFromFile(progName, line);
	if (ln)
		color_fprintf(stderr, 255, 0, 0, ln);
	else
		printf("\aERROR: getLineFromFile failed\n");

	exit(EXIT_SUCCESS);
}

void handle_sigint_shell(int){
	printf("\r");
	//runShell(*shell_vars.nodes, *shell_vars.showErrors, *shell_vars.ms, *shell_vars.elseStat);
}
