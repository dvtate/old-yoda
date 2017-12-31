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


extern char* metaName;
extern int line;

extern FILE* program;
extern char* progName;


// will be the error code given when a function return successfully
// this content is set so that if they use it out of context they are still told so
extern const char* lambda_finish;

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





void runFile(char* programFile, bool& errorReporting);




/* from string_stack.hpp
namespace macro {
	typedef enum {
		MACRO, LAMBDA, OTHER
	} exec_t;

	typedef enum {
		SUCCESS, RETURN, ERROR, BREAK
	} ret_t;
}
*/
macro::ret_t runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
			 std::stack<CalcValue>& mainStack, bool& elseStatement
);


void runShell(std::vector<UserVar>& var_nodes, bool& errorReporting,
			  std::stack<CalcValue>& mainStack, bool& elseStatement, std::vector<void*> freeable
);



char* runMacro(StrStack* macro, std::stack<CalcValue>& mainStack, std::vector<UserVar> var_nodes, std::vector<void*>& freeable, bool showErrors, bool elseStatement);
char* runMacroKeepScope(StrStack* macro, std::stack<CalcValue>& mainStack, std::vector<UserVar> var_nodes, std::vector<void*>& freeable, bool showErrors, bool elseStatement);


// Returns the start of the token/string-constant
// manages escape-sequences within string-constants
char* qtok(char* str, char** next);



#endif