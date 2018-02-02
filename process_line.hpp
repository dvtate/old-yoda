#ifndef PROCESS_LINE_H
#define PROCESS_LINE_H

#include <iostream>
#include <stack>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <unistd.h>

// this is the class used in our stack
#include "calc_value.hpp"

// custom types
#include "user_types.hpp"

// user defined variables
#include "user_variables.hpp"

// some useful functions
#include "utils.hpp"

// version info
#include "lolcat_version.h"

// lists
#include "list.hpp"

// macros
#include "string_stack.hpp"

// terminal stuff
#include "termio.hpp"

// Returns the start of the token/string-constant
// manages escape-sequences within string-constants
extern char* qtok(char* str, char** next);
extern int line;
extern char* progName;
extern FILE* program;

// will be the error code given when a function return successfully
// this content is set so that if they use it out of context they are still told so
extern const char* lambda_finish;


// this gets returned from runMacro() on error
extern const char* macro_error;


extern macro::ret_t runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
                            std::stack<CalcValue>& mainStack, bool& elseStatement);

// converts from full to relative path
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// closes with an error
#define PASS_ERROR(MSG) \
	if (showErrors)	\
		std::cerr <<'(' <<__FILENAME__ <<'#'<<__LINE__ <<") " <<MSG;\
	return p;


// error's if the stack is empty
#define ASSERT_NOT_EMPTY(OPERATOR)\
	if (mainStack.empty()) {\
		PASS_ERROR("\aERROR: not enough data for `" <<OPERATOR <<"`.\n")\
	}


// makes the top() value an actual Value as opposed to a reference
#define CONVERT_REFS(MAINSTACK, VAR_NODES)\
	if (MAINSTACK.top().type == CalcValue::REF) {\
		CalcValue* val = MAINSTACK.top().valAtRef(VAR_NODES);\
\
		if (val != NULL)\
			MAINSTACK.top().setValue(*val);\
		else {\
			PASS_ERROR("\aERROR: broken reference to $" <<MAINSTACK.top().string <<'\n');\
		}\
	}


#define GET_LIST_INDEX(MAINSTACK, VAR_NODES, ASSIGN_TO)\
	if (MAINSTACK.top().type == CalcValue::INX) {\
		while (!MAINSTACK.empty() && MAINSTACK.top().type == CalcValue::INX) {\
			ASSIGN_TO.push_back(MAINSTACK.top().index);\
			MAINSTACK.pop();\
		}\
		if (MAINSTACK.empty()) {\
			PASS_ERROR("\aERROR: index without list\n");\
		}\
	}

// set top() = the value at the given index
#define CONVERT_INDEX(MAINSTACK, VAR_NODES)\
	if (MAINSTACK.top().type == CalcValue::INX) {\
        std::vector<ssize_t> indx;\
        GET_LIST_INDEX(MAINSTACK, VAR_NODES, indx);\
        CONVERT_REFS(MAINSTACK, VAR_NODES);\
        if (MAINSTACK.top().type != CalcValue::ARR) {\
            PASS_ERROR("\aERROR: index without array\n");\
        }\
		CalcValue* cv = MAINSTACK.top().getListElem(indx);\
		if (!cv) {\
            PASS_ERROR("\aERROR: List index out of bounds\n");\
        }\
		MAINSTACK.top().setValue(*cv);\
	}

// unfortunately I don't think perfomance is better with this system...
#define CONVERT_TOP(MAINSTACK, VAR_NODES, FREEABLE)\
	if (!MAINSTACK.empty()) {\
		unsigned fsize = FREEABLE.size();\
		CalcValue* tmp = conv_top(MAINSTACK, VAR_NODES, showErrors, FREEABLE);\
		if (!tmp) {\
			PASS_ERROR("\aERROR: error in lazy evaluation\n");\
		}\
		MAINSTACK.push(*tmp);\
		if (fsize != FREEABLE.size()) {\
            free(FREEABLE[FREEABLE.size() - 1]);\
            FREEABLE.pop_back();\
        }\
	}


#define GET_REQUEST(MAINSTACK, VAR_NODES, ASSIGN_TO)\
	if (MAINSTACK.top().type == CalcValue::INX) {\
		while (!MAINSTACK.empty() && MAINSTACK.top().type == CalcValue::INX) {\
			ASSIGN_TO.push_back(MAINSTACK.top().index);\
			MAINSTACK.pop();\
		}\
		if (MAINSTACK.empty()) {\
			PASS_ERROR("\aERROR: index without list\n");\
		}\
	}



// evals a block
#define RUN_STR_STK(STRSTK, STACK) {\
		/* put the statement in a string */\
		size_t buff_size = 500;\
		char* buff = (char*) malloc(buff_size);\
		(STRSTK).toString(&buff, &buff_size);\
\
		/* put the string in a temp file*/\
		FILE* statement = tmpfile();\
		if (!statement) { PASS_ERROR("\aERROR: tmpfile() returned null. This isn't your fault. :/\n"); }\
		fputs(buff, statement);\
		rewind(statement);\
		freeable.push_back(buff);\
\
		/* add layer to scope*/\
		UserVar first_node(NULL, " ", 0.0);\
		var_nodes.push_back(first_node);\
\
		/* run the temp file */\
		macro::ret_t ret = runFile(statement, var_nodes, showErrors, (STACK), elseStatement);\
		if (ret == macro::ERROR) {\
			PASS_ERROR("\aERROR: macro execution failed\n");\
		} else if (ret == macro::RETURN || ret == macro::BREAK) {\
			/* variables go out of scope */\
			vars::clearScope(var_nodes);\
			fclose(statement);\
			return (char*) lambda_finish;\
		}\
		\
\
		/* variables go out of scope */\
		vars::clearScope(var_nodes);\
		fclose(statement);\
	}


#define CLEAR_STACK(STACK) while (!STACK.empty()) STACK.pop();

#include "top_modifiers.hpp"


/// returns: location/source of error or NULL
/// params: environment/operation variables
/// this function runs the user's code, most essential part of the interpreter
/// go ahead and hate on the fact its over 200 lines long... but it works tho :)
char* processLine(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                  bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                  std::vector<void*>& freeable
);

#endif