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


extern char* progName;
extern FILE* program;


extern macro::ret_t runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
                            std::stack<CalcValue>& mainStack, bool& elseStatement, std::vector<void*>& freeable);


// error's if the stack is empty
#define ASSERT_NOT_EMPTY(OPERATOR)\
	if (mainStack.empty()) {\
		PASS_ERROR("\aERROR: not enough data for `" <<OPERATOR <<"`.\n")\
	}

// closes with an error
#define PASS_ERROR(MSG) \
	if (showErrors)	\
		std::cerr <<"(#" <<__LINE__ <<") " <<MSG;\
	return p;

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
		macro::ret_t ret = runFile(statement, var_nodes, showErrors, (STACK), elseStatement, freeable);\
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


/// returns: location/source of error or NULL
/// params: environment/operation variables
/// this function runs the user's code, most essential part of the interpreter
/// go ahead and hate on the fact its over 200 lines long... but it works tho :)
char* processLine(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                  bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                  std::vector<void*>& freeable
){

	//std::cout <<"\nCurrent Line: " << rpnln <<"\n";

	// probably won't even use these 2 vars but its good to have them...
	size_t lineLen = strlen(rpnln);
	char* pInit = rpnln;

	// get first token from the input
	char* p = qtok(rpnln, &rpnln);

	// empty string/whitespace input
	if (p == NULL)
		return p;

	// decipher token
	while (p != NULL && *p != '\0') {

		//printf("p=\"%s\"\n",p);

		// char is a binary operator
		if (((*p == '-' || *p == '*' || *p == '/' || *p == '%'
		      || *p == '|' || *p == '&' || *p == '^' || *p == '>' || *p == '<')
		     && *(p + 1) == '\0')
		    || !strcmp(p, "<<") || !strcmp(p, ">>") || !strcmp(p, "**")
		    || !strcmp(p, "<=") || !strcmp(p, ">=") || !strcmp(p, "logbase")
		    || !strcmp(p, "pow") // for those who dont like "**"
				) {

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy operator `" <<p <<"`.\n");
			}
			//printf("\nb="); printCalcValue(mainStack.top(), var_nodes);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operator `" <<p <<"`. (expected two numbers)\n");
			}

			double b = mainStack.top().getNum();
			//printf("\nb=(%s) ", CVtypename(mainStack.top())); printCalcValue(mainStack.top(), var_nodes);
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operator `" <<p <<"`. (expected two numbers)\n");
			}

			double a = mainStack.top().getNum();
			//printf("\na="); printCalcValue(mainStack.top(), var_nodes);
			mainStack.pop();
			switch (*p) {
				case '*':
					if (*(p + 1) == '*')
						mainStack.push(pow(a, b));
					else
						mainStack.push(a * b);
					break;

				case '/': mainStack.push(a / b); break;
				case '-': mainStack.push(a - b); break;
				case '%': mainStack.push((long) a % (long) b); break;
				case '^': mainStack.push((long) a ^ (long) b); break;
				case '|': mainStack.push((long) a | (long) b); break;
				case '&': mainStack.push((long) a & (long) b); break;

				case '<':
					switch (*(p + 1)) {
						case '<': mainStack.push((int) a << (int) b); break;
						case '=': mainStack.push(a <= b); break;
						case '\0': mainStack.push(a < b); break;
					}
					break;

				case '>':
					switch (*(p + 1)) {
						case '>': mainStack.push((int) a << (int) b); break;
						case '=': mainStack.push(a >= b); break;
						case '\0': mainStack.push(a > b); break;
					}
					break;

				case 'l': mainStack.push(log10(b) / log10(a)); break;
				case 'p': mainStack.push(pow(a, b)); break;
			}

		} else if (*p == '+' && *(p + 1) == '\0') {

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy `+` operator." <<std::endl);
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue b = getNextValue(mainStack);

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue a = getNextValue(mainStack);


			// handling null values
			if (a.isEmpty() != b.isEmpty()) // val + null
				mainStack.push(b.isEmpty() ? a : b);

			else if (a.isEmpty() && b.isEmpty()) // null + null
				mainStack.push(a);

				// branching out all 4 permutations of string and num
			else if (a.type == CalcValue::STR) {
				if (b.type == CalcValue::STR) {
					// allocate enough memory for both strings and a null terminator
					char combined[strlen(a.getStr()) + strlen(b.getStr()) + 1];

					// combine the strings
					strcpy(combined, a.getStr());
					strcpy(combined + strlen(a.getStr()), b.getStr());

					mainStack.push(combined);

				} else if (b.type == CalcValue::NUM) { // b is a number

					// convert the double to a string
					char str[26];
					snprintf(str, 26, "%*.*g", 10, 16, b.getNum());
					char* trimmedStr = trimStr(str);

					// allocate memory
					char combined[strlen(a.getStr()) + strlen(trimmedStr) + 1];

					// combine them
					strcpy(combined, a.getStr());
					strcpy(&combined[strlen(a.getStr())], trimmedStr);

					mainStack.push(combined);
				}
			} else if (a.type == CalcValue::NUM) {
				if (b.type == CalcValue::STR) {

					// convert the double to a string
					char str[26];

					snprintf(str, 26, "%*.*g", 10, 16, a.getNum());
					char* trimmedStr = trimStr(str);

					// allocate memory
					char combined[strlen(trimmedStr) + strlen(b.getStr()) + 1];
					// combine them
					strcpy(combined, trimmedStr);
					strcpy(&combined[strlen(trimmedStr)], b.getStr());

					mainStack.push(combined);

				} else if (b.type == CalcValue::NUM)
					mainStack.push(a.getNum() + b.getNum());
			} else {
				PASS_ERROR("\aERROR: operator `+` doesn't work on the provided types")
			}
		} else if (strcmp(p, "==") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `" <<p <<"` expected 2 values to compare\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue a = mainStack.top();
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.top().setValue(CalcValue(mainStack.top() == a));

			// not equal to
		} else if (strcmp(p, "!=") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `" <<p <<"` expected 2 values to compare\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue a = mainStack.top();
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.top() = !(mainStack.top() == a);

			// logical not operator
		} else if (*p == '!' && *(p + 1) == '\0') {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			mainStack.push((getNextValue(mainStack).getNum() == 0));

			// short-circuit &&
		} else if (strcmp(p, "&&") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `" <<p <<"` (short-circuit and) expected 2 boolean expressions/macros\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue v2 = mainStack.top();
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue v1 = mainStack.top();
			mainStack.pop();

			if (v1.type == CalcValue::NUM) {
				if (v1.number == 0)
					mainStack.push(0.0);
				else {
					if (v2.type == CalcValue::NUM) {
						if (v2.number == 0)
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else if (v2.type == CalcValue::BLK) {
						std::stack<CalcValue> condStack;

						RUN_STR_STK(*v2.block, condStack);
						// if condition evaluates to false (or doesnt give a value)
						if (condStack.empty() || !condStack.top().getNum())
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else {
						PASS_ERROR("\aERROR: `" <<p <<"` (short-circuit and) expected a boolean expression/macro\n");
					}
				}

			} else if (v1.type == CalcValue::BLK) {

				std::stack<CalcValue> condStack;

				RUN_STR_STK(*v1.block, condStack);
				// if condition evaluates to false (or doesnt give a value)
				if (condStack.empty() || !condStack.top().getNum())
					mainStack.push(0.0);
				else {
					if (v2.type == CalcValue::NUM) {
						if (v2.number == 0)
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else if (v2.type == CalcValue::BLK) {
						std::stack<CalcValue> condStack;

						RUN_STR_STK(*v2.block, condStack);
						// if condition evaluates to false (or doesnt give a value)
						if (condStack.empty() || !condStack.top().getNum())
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else {
						PASS_ERROR("\aERROR: `" <<p <<"` (short-circuit and) expected a boolean expression/macro\n");
					}
				}



			} else {
				PASS_ERROR("\aERROR: `" <<p <<"` (short-circuit or) expected a boolean expression/macro\n");
			}



			// short-circuit ||
		} else if (strcmp(p, "||") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `" << p << "` (short-circuit or) expected 2 boolean expressions/macros\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue v2 = mainStack.top();
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue v1 = mainStack.top();
			mainStack.pop();

			if (v1.type == CalcValue::NUM) {
				if (v1.number != 0)
					mainStack.push(1.0);
				else {
					if (v2.type == CalcValue::NUM) {
						if (v2.number == 0)
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else if (v2.type == CalcValue::BLK) {
						std::stack<CalcValue> condStack;

						RUN_STR_STK(*v2.block, condStack);
						// if condition evaluates to false (or doesnt give a value)
						if (condStack.empty() || !condStack.top().getNum())
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else {
						PASS_ERROR("\aERROR: `" << p << "` (short-circuit or) expected a boolean expression/macro\n");
					}
				}

			} else if (v1.type == CalcValue::BLK) {

				std::stack<CalcValue> condStack;

				RUN_STR_STK(*v1.block, condStack);
				// if condition evaluates to false (or doesnt give a value)
				if (!condStack.empty() && condStack.top().getNum())
					mainStack.push(1.0);
				else {
					if (v2.type == CalcValue::NUM) {
						if (v2.number == 0)
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else if (v2.type == CalcValue::BLK) {
						std::stack<CalcValue> condStack;

						RUN_STR_STK(*v2.block, condStack);
						// if condition evaluates to false (or doesnt give a value)
						if (condStack.empty() || !condStack.top().getNum())
							mainStack.push(0.0);
						else
							mainStack.push(1.0);
					} else {
						PASS_ERROR("\aERROR: `" << p << "` (short-circuit and) expected a boolean expression/macro\n");
					}
				}


			} else {
				PASS_ERROR("\aERROR: `" << p << "` (short-circuit and) expected a boolean expression/macro\n");
			}

			// ++ | --
		} else if (strlen(p) == 2 && (*p == '+' || *p == '-') && *(p + 1) == *p) {
			ASSERT_NOT_EMPTY(p);

			CalcValue* var_val;
			if (mainStack.top().type == CalcValue::REF) {
				//PASS_ERROR("\aERROR: increment/decrement operator expected a reference\n");
				var_val = mainStack.top().valAtRef(var_nodes);
				mainStack.pop();
			} else if (mainStack.top().type == CalcValue::INX) {
				std::vector<ssize_t> index;
				GET_LIST_INDEX(mainStack, var_nodes, index);

				CalcValue list = mainStack.top();
				mainStack.pop();

				if (list.type == CalcValue::REF) {
					var_val = list.valAtRef(var_nodes)->getListElem(index);
					if (!var_val) {
						PASS_ERROR("\aERROR: $"<< list.string <<" undefined or inaccessable.\n");
					}
				} else {
					PASS_ERROR("\aERROR: invalid use of list assignment\n");
				}
			}
			if (!var_val) {
				PASS_ERROR("\aERRORL broken reference to $" <<mainStack.top().string <<std::endl);
			}
			if (var_val->type != CalcValue::NUM) {
				PASS_ERROR("\aIncrement/decrememnt expected a numerical variable\n");
			}
			var_val->number += *p == '+' ? 1 : -1;


			// modified assignment '*'= += *= /= -= %=
		} else if ((strlen(p) == 2 && (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%') && *(p + 1) == '=')
				   || (strlen(p) == 3 && (*p == '<' || *p == '>') && *p == *(p + 1) && *(p + 2) == '=')) {
			// not enough data
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: not enough data for modified assignment")
			}
			bool last_index = false;
			if (mainStack.top().type == CalcValue::INX)
				last_index = true;
			CONVERT_INDEX(mainStack,var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			CalcValue v2 = mainStack.top();
			mainStack.pop();
			CalcValue v1 = mainStack.top();
			mainStack.pop();

			CalcValue* target;
			CalcValue* val;

			if (v1.type == CalcValue::INX) {
				mainStack.push(v1);

				std::vector<ssize_t> index;

				GET_LIST_INDEX(mainStack, var_nodes, index);

				CalcValue list = mainStack.top();
				mainStack.pop();

				if (list.type == CalcValue::REF) {
					target = list.valAtRef(var_nodes)->getListElem(index);
					if (!target) {
						PASS_ERROR("\aERROR: $"<< list.string <<" undefined or inaccessable.\n");
					}
				} else {
					PASS_ERROR("\aERROR: invalid use of list assignment\n");
				}
				val = v2.valAtRef(var_nodes);

			} else if (v1.type == CalcValue::REF) {
				target = v1.valAtRef(var_nodes);
				val = v2.valAtRef(var_nodes);
			} else if (v2.type == CalcValue::REF) {
				target = v2.valAtRef(var_nodes);
				val = v1.valAtRef(var_nodes);
			} else if (last_index) {
				PASS_ERROR("\aERROR: modified assignment expected the index on the left and the value on the right\n");
			} else {
				PASS_ERROR("\aERROR: modified assignment expected a reference\n");
			}

			if (!target || !val) {
				PASS_ERROR("\aERRORL broken reference to $" <<mainStack.top().string <<std::endl);
			}
			if (target->type != CalcValue::NUM && *p != '+') {
				PASS_ERROR("\aERROR: Modified assignment expected a reference to a numerical value\n");
			}
			if (val->type != CalcValue::NUM && *p != '+') {
				PASS_ERROR("\aERROR: Modified assignment expected a value to modify.")
			}

			switch (*p) {
				case '+': // addition is an overloaded operator

					// handling null values
					if (target->isEmpty() != val->isEmpty()) // val + null
						target->number = target->isEmpty() ?  val->number : target->number;

						// branching out all 4 permutations of string and num
					else if (target->type == CalcValue::STR) {
						if (val->type == CalcValue::STR) {
							// allocate enough memory for both strings and a null terminator
							char combined[strlen(target->getStr()) + strlen(val->getStr()) + 1];

							// combine the strings
							strcpy(combined, target->getStr());
							strcpy(combined + strlen(target->getStr()), val->getStr());

							target->setValue(combined);

						} else if (val->type == CalcValue::NUM) { // b is a number

							// convert the double to a string
							char str[26];
							snprintf(str, 26, "%*.*g", 10, 16, val->getNum());
							char* trimmedStr = trimStr(str);

							// allocate memory
							char combined[strlen(target->getStr()) + strlen(trimmedStr) + 1];

							// combine them
							strcpy(combined, target->getStr());
							strcpy(combined + strlen(target->getStr()), trimmedStr);

							target->setValue(combined);
						}
					} else if (target->type == CalcValue::NUM) {
						if (val->type == CalcValue::STR) {

							// convert the double to a string
							char str[26];

							snprintf(str, 26, "%*.*g", 10, 16, target->getNum());
							char* trimmedStr = trimStr(str);

							// allocate memory
							char combined[strlen(trimmedStr) + strlen(val->getStr()) + 1];
							// combine them
							strcpy(combined, trimmedStr);
							strcpy(&combined[strlen(trimmedStr)], val->getStr());

							target->setValue(combined);

						} else if (val->type == CalcValue::NUM)
							target->number = target->number + val->number;
					}
					break;
				case '-':
					target->number = target->number - val->number;
					break;
				case '*':
					target->number = target->number * val->number;
					break;
				case '/':
					target->number = target->number / val->number;
					break;
				case '<':
					target->number = (double) ((long) target->number << (long) val->number);
					break;
				case '>':
					target->number = (double) ((long) target->number >> (long) val->number);
					break;
				case '%':
					target->number = (double) ((long) target->number % (long) val->number);
					break;
			}

			//trig functions
		} else if (strcmp(p, "sin") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(sin(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "cos") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(cos(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "tan") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(tan(getNextValue(mainStack).getNum()));

			// inverse trig functions
		} else if (strcmp(p, "asin") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(asin(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "acos") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(acos(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "atan") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(atan(getNextValue(mainStack).getNum()));

			// hyperbolic trig functions
		} else if (strcmp(p, "sinh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(sinh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "cosh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(cosh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "tanh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(tanh(getNextValue(mainStack).getNum()));


			// inverse hyperbolic trig functions
		} else if (strcmp(p, "asinh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(asinh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "acosh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(acosh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "atanh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(atanh(getNextValue(mainStack).getNum()));

			// more unary math functions
		} else if (strcmp(p, "log") == 0 || strcmp(p, "log10") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(log10(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "ln") == 0) { // natural log
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(log(getNextValue(mainStack).getNum()));

		} else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0) { // square root
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(sqrt(getNextValue(mainStack).getNum()));

		} else if (strcmp(p, "abs") == 0) { // absolute value
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			mainStack.push(std::abs(getNextValue(mainStack).getNum()));

			// find length of a string
		} else if (strcmp(p, "strlen") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type == CalcValue::STR)
				mainStack.top().setValue((double)strlen(mainStack.top().string));
			else {
				PASS_ERROR("\aERROR: strlen expected a string.\n");
			}

			// find first occurance of substring in a string (strstr())
		} else if (strcmp(p, "strstr") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: strstr expected 2 strings, a haystack and a needle to find\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: strstr expected 2 strings, a haystack and a needle to find\n");
			}

			char needle[strlen(mainStack.top().string)];
			strcpy(needle, mainStack.top().string);
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: strstr expected 2 strings, a haystack and a needle to find\n");
			}
			char haystack[strlen(mainStack.top().string)];
			strcpy(haystack, mainStack.top().string);
			mainStack.pop();

			mainStack.push(strstr(haystack, needle));

			// case-insensitive strstr
		} else if (strcmp(p, "stristr") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}

			char needle[strlen(mainStack.top().string)];
			strcpy(needle, mainStack.top().string);
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}

			char haystack[strlen(mainStack.top().string)];
			strcpy(haystack, mainStack.top().string);
			mainStack.pop();

			mainStack.push(strstr(haystack, needle));

			// remove leading and triling
		} else if (strcmp(p, "trim") == 0) {

			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: trim expected a string\n");
			}

			char str[strlen(mainStack.top().string) + 1];
			strcpy(str, mainStack.top().string);
			mainStack.pop();
			mainStack.push(trimStr(str));

		} else if (strcmp(p, "split") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			// split elems of list
			if (mainStack.top().type == CalcValue::ARR) {
				// TODO: fix this
				std::vector<CalcValue> tmp = *mainStack.top().list;
				mainStack.pop();

				for (CalcValue elem : tmp)
					mainStack.push(CalcValue(elem));

				// spliting a string
			} else {

				if (mainStack.size() < 2) {
					PASS_ERROR("\aERROR: split expected 2 strings, a base-string and delimiters\n");
				}

				// get delims
				if (mainStack.top().type != CalcValue::STR) {
					PASS_ERROR("\aERROR: split expected 2 strings, a base-string and delimiters\n");
				}
				// exploding string into constituent characters
				if (!strlen(mainStack.top().string)) {
					mainStack.pop();
					// get str
					CONVERT_INDEX(mainStack, var_nodes);
					CONVERT_REFS(mainStack, var_nodes);
					if (mainStack.top().type != CalcValue::STR) {
						PASS_ERROR("\aERROR: split expected 2 strings, a base-string and delimiters\n");
					}
					char str[strlen(mainStack.top().string)]; // no room for '\0'
					strcpy(str, mainStack.top().string);
					mainStack.pop();

					// push each character onto the stack
					std::vector<CalcValue> list;
					for (char ch : str) {
						char chr[2] = {ch, '\0'};
						list.push_back(chr);
					}
					mainStack.push(list);
					// split by delimiter
				} else {
					// copy delimiters
					char delims[strlen(mainStack.top().string) + 1];
					strcpy(delims, mainStack.top().string);
					mainStack.pop();

					// get str
					CONVERT_INDEX(mainStack, var_nodes);
					CONVERT_REFS(mainStack, var_nodes);
					if (!mainStack.top().isStr()) {
						PASS_ERROR("\aERROR: split expected 2 strings, a base-string and delimiters\n");
					}
					char str[strlen(mainStack.top().string) + 1];
					strcpy(str, mainStack.top().string);
					mainStack.pop();

					std::vector<CalcValue> list;
					char *pch = strtok(str, delims);
					while (pch) {
						list.push_back(pch);
						pch = strtok(NULL, delims);
					}
					mainStack.push(list);
				}
			}
			// replace substring
		} else if (strcmp(p, "str_replace") == 0) {
			if (mainStack.size() < 3) {
				PASS_ERROR("\aERROR: str_replace expected 3 strings: base_string, old_substr, new_substr");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: str_replace expected 3 strings: base_string, old_substr, new_substr");
			}
			char with[strlen(mainStack.top().string) + 1];
			strcpy(with, mainStack.top().string);
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: str_replace expected 3 strings: base_string, old_substr, new_substr\n");
			}
			char repl[strlen(mainStack.top().string) + 1];
			strcpy(repl, mainStack.top().string);
			mainStack.pop();

			char *tmp = str_replace(mainStack.top().string, repl, with);
			mainStack.push(tmp);
			free(tmp);

		} else if (strcmp(p, "char_at") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: char_at expected a string and a numerical index\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type == CalcValue::STR && mainStack.top().string) {
				char tmp[strlen(mainStack.top().string)];
				strcpy(tmp, mainStack.top().string);
				mainStack.pop();

				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::NUM) {
					PASS_ERROR("\aERROR: char_at expected a string and a numerical index\n");
				}

				ssize_t i = mainStack.top().number;
				mainStack.pop();

				if ((size_t) abs(i) > strlen(tmp) || i == (ssize_t) strlen(tmp)) {
					//PASS_ERROR("\aERROR: char_at: index `" << i << "` out of bounds\n");
					mainStack.push(CalcValue());
				} else {
					char chr[2];

					// negative index starts from back
					chr[0] = tmp[i >= 0 ? i : strlen(tmp) + i];
					chr[1] = '\0';
					mainStack.push(chr);
				}
			} else if (mainStack.top().type == CalcValue::NUM) {

				ssize_t i = (ssize_t) mainStack.top().number;

				mainStack.pop();
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);

				if (mainStack.top().type != CalcValue::STR) {
					PASS_ERROR("\aERROR: char_at expected a string and a numerical index\n");
				}

				char chr[2];

				if ((size_t) abs(i) > strlen(mainStack.top().string) || i == (ssize_t) strlen(mainStack.top().string)) {
					//PASS_ERROR("\aERROR: char_at: index `" << i << "` out of bounds\n");
					mainStack.pop();
					mainStack.push(CalcValue()); // returns null on error
				} else {

					chr[0] = mainStack.top().string[i >= 0 ? i : strlen(mainStack.top().string) + i];
					chr[1] = '\0';
					mainStack.pop();
					mainStack.push(chr);
				}
			}

			// delete a char from a string at an index
		} else if (strcmp(p, "del_char") == 0) {

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: del_char expected a string and a numerical index\n");
			}
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type == CalcValue::STR && mainStack.top().string) {

				// copy top string
				char tmp[strlen(mainStack.top().string)];
				strcpy(tmp, mainStack.top().string);
				mainStack.pop();

				// get index
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::NUM) {
					PASS_ERROR("\aERROR: del_char expected a string and a numerical index\n");
				}
				int i = (int) mainStack.top().number;
				mainStack.pop();

				// assert index in range
				if (abs(i) > (ssize_t) strlen(tmp) || i == (ssize_t) strlen(tmp)) {
					PASS_ERROR("\aERROR: del_char: index `" << i << "` out of bounds\n");
				}

				// delete char at index
				deleteChar(i >= 0 ? tmp + i  : tmp + strlen(tmp) + i);
				mainStack.push(tmp);


			} else if (mainStack.top().type == CalcValue::NUM)  {
				// get index
				ssize_t i = (ssize_t) mainStack.top().number;
				mainStack.pop();

				// check range of index
				if (abs(i) > (ssize_t) strlen(mainStack.top().string) || i == (ssize_t) strlen(mainStack.top().string)) {
					PASS_ERROR("\aERROR: del_char: index `" << i << "` out of bounds\n");
				}

				// verify string
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::STR) {
					PASS_ERROR("\aERROR: del_char expected a string and a numerical index");
				}

				// calls delete char on the CV in ms.top(), if index is negative then start from reverse
				char* str = mainStack.top().string;
				deleteChar(i >= 0 ? i + str : str + strlen(str) + i);

			}

			// line-comments
		} else if (*p == '#')
			break; // ignore rest of line

			// constants
		else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI); // defined in math.h
		else if (*p == 'e' && *(p + 1) == '\0')
			mainStack.push(M_E); // defined in math.h
		else if (strcmp(p, "null") == 0)
			mainStack.push(CalcValue());
		else if (strcmp(p, "true") == 0)
			mainStack.push(1.0);
		else if (strcmp(p, "false") == 0)
			mainStack.push(0.0);

			// generates a list with a range of numbers from * to * ~~
		else if (strcmp(p, "range") == 0) {
			// assert stklen
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			// get end
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			int end = (int) mainStack.top().getNum();
			mainStack.pop();


			// get start
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			int start = (int) mainStack.top().getNum();
			mainStack.pop();

			// generate list
			std::vector<CalcValue> list;
			if (start > end) { // [start, end] 3,2,1
				for (; start >= end; start--)
					list.push_back((double) start);
				mainStack.push(list);
			} else if (start < end) { // [start, end) 1,2,3
				for (; start < end; start++)
					list.push_back((double) start);
				mainStack.push(list);
			} else { // start == end
				list.push_back(start);
				mainStack.push(list);
			}


			// print value terminal
		} else if (strcmp(p, "print") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (printCalcValueRAW(mainStack.top(), var_nodes)) {
				PASS_ERROR("\aERROR: print has failed.\n");

			}
			mainStack.pop();

			// print and end with a newline
		} else if (strcmp(p, "println") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (printCalcValueRAW(mainStack.top(), var_nodes)) {
				PASS_ERROR("\aERROR: printlln has failed.\n");
			}
			mainStack.pop();

			std::cout <<std::endl;

		} else if (strcmp(p, "color_print") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: not enough data for function `" <<p <<"`. (expects message and color)\n\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: "<<p <<" expected a message and a string containing a valid HTML color\n\n");
			}

			setFgColor(mainStack.top().string);
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue msg = getNextValue(mainStack);
			if (printCalcValueRAW(msg, var_nodes)) {
				setFgColor();
				PASS_ERROR("ERROR: color_print has failed")
			}

			setFgColor();

			// changes the terminal background color for text
		} else if (strcmp(p, "setBgColor") == 0) {

			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: setBgColor expected a string containing a valid HTML color.\n\n");
			}

			setBgColor(mainStack.top().string);
			mainStack.pop();

			// changes the terminal foreground color for text
		} else if (strcmp(p, "setFgColor") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: setFgColor expected a string containing a valid HTML color.\n\n");
			}

			setFgColor(mainStack.top().string);
			mainStack.pop();

		} else if (strcmp(p, "reset_color") == 0)
			resetANSI();
		else if (strcmp(p, "resetFgColor") == 0)
			setFgColor();
		else if (strcmp(p, "resetBgColor") == 0)
			setBgColor();



			// user input
		else if (strcmp(p, "input") == 0 || strcmp(p, "getline") == 0) {
			char* input = (char*) malloc(256);
			size_t line_len = 256;

			if (getline(&input, &line_len, stdin) == -1) {
				PASS_ERROR("\aERROR: input could not getline()\n");
			}

			if (input[strlen(input) - 1] == '\n')
				input[strlen(input) - 1] = '\0';

			mainStack.push(input);

			free(input);

			// get a single character from stdin
		} else if (strcmp(p, "getchar") == 0) {
			char input[2] = { (char) getc(stdin), '\0'};
			mainStack.push(input);
			// silently get character without needing newline
		} else if (strcmp(p, "getch") == 0) {
			char tmp[] {
#ifdef _WIN32
					getch()
#else
					termio::getch()
#endif
					, '\0'};
			mainStack.push(tmp);
			// get character without newline
		} else if (strcmp(p, "getche") == 0) {
			char tmp[] {
#ifdef _WIN32
					getche()
#else
					termio::getche()
#endif
					, '\0'};
			mainStack.push(tmp);
		} else if (strcmp(p, "get_pass") == 0) {
			mainStack.push(getpass(""));
			// load the contents of a file into a string
		} else if (strcmp(p, "file_get_contents") == 0) {

			ASSERT_NOT_EMPTY(p);

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			// didn't recieve an string...
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the file name.\n\n");
			}

			char *file_contents;
			size_t input_file_size;

			// open the file
			FILE *input_file = fopen(mainStack.top().string, "rb");
			mainStack.pop();
			if (!input_file) {
				// if file not found pass a null calc_value object
				//PASS_ERROR("\aERROR: file_get_contents: file `" <<mainStack.top().string <<"` not found.";
				mainStack.push(NULL_CALCVAL_OBJECT);
			} else {

				// get size of file
				fseek(input_file, 0, SEEK_END);
				input_file_size = ftell(input_file);
				rewind(input_file);

				// allocate memory for the string
				file_contents = (char*) malloc(input_file_size + 1);
				fread(file_contents, sizeof(char), input_file_size, input_file);

				// push the string to the stack
				file_contents[input_file_size] = '\0';
				mainStack.push(file_contents);

				// cleanup
				fclose(input_file);
				free(file_contents);

			}


			// load the contents of a string into a file
		} else if (strcmp(p, "file_put_contents") == 0) {
			// takes a string and a filename
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: file_put_contents expected 2 strings contents and file name\n\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			// takes a string for the filename
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the file name.\n\n");
			}

			FILE *output_file = fopen(mainStack.top().string, "w");
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			// takes a string for the contents
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the contents to write.\n\n");
			}

			fwrite(mainStack.top().string, 1, strlen(mainStack.top().string) + 1, output_file);
			mainStack.pop();
			fclose(output_file);

			// functionally equivalent to #include but without a preprocessor
		} else if (strcmp(p, "insert") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type != CalcValue::STR || mainStack.top().isEmpty()) {
				PASS_ERROR("\aERROR: `" <<p <<"` expected a string for the file name")
			}
			FILE* statement = fopen(mainStack.top().string, "r");
			if (!statement) {
				PASS_ERROR("\aERROR: \""<<mainStack.top().string <<"\" couldn't be found.\n");
			}
			mainStack.pop();

			// run the file
			macro::ret_t ret = runFile(statement, var_nodes, showErrors, mainStack, elseStatement, freeable);
			if (ret == macro::ERROR) {
				PASS_ERROR("\aERROR: error in file added with `insert`\n");
			}

			// we're now done with the file
			fclose(statement);

			// get the value at the specific index of an array
		} else if (strcmp(p, "get") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `get` expected a list and a numerical index\n\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type == CalcValue::NUM) {
				size_t index = (size_t) round(abs(mainStack.top().getNum()));
				mainStack.pop();
				if (mainStack.top().type == CalcValue::ARR) {
					CalcValue val = (*mainStack.top().list)[index];
					mainStack.top() = val;
				} else {
					PASS_ERROR("\aERROR: `get` expected a list and a numerical index\n\n");
				}
			} else if (mainStack.top().type == CalcValue::ARR) {
				std::vector<CalcValue> list = *mainStack.top().list;
				mainStack.pop();
				if (mainStack.top().type == CalcValue::NUM) {
					CalcValue val = list[(size_t) round(abs(mainStack.top().getNum()))];
					mainStack.top() = val;
				} else {
					PASS_ERROR("\aERROR: `get` expected a list and a numerical index\n\n");
				}
			} else if (mainStack.top().type == CalcValue::INX) {
				CONVERT_INDEX(mainStack, var_nodes);
			} else {
				PASS_ERROR("\aERROR: `get` expected a list and a numerical index\n\n");
			}

			// index of a list (bracket operator)
		} else if (*p == ']' && *(p + 1) == '\0') {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes); // is this right?
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: non-numerical index\n");
			}
			CalcValue tmp;
			tmp.type = CalcValue::INX;
			tmp.index = (ssize_t) mainStack.top().getNum();
			mainStack.pop();
			mainStack.push(tmp);

			// push a value onto a list
		} else if (strcmp(p, "push_back") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: `push_back` expected a list and a numerical index\n\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);

			if (mainStack.top().type == CalcValue::REF) {
				CalcValue *tmp = mainStack.top().valAtRef(var_nodes);

				if (!tmp) {
					goto push_into_arr;
				}
				if (tmp->type == CalcValue::ARR)
					tmp->list->push_back(val);
				else {
					std::vector<CalcValue> tmpVec;
					tmpVec.push_back(*tmp);
					tmp->setValue(tmpVec);
					tmp->list->push_back(val);
				}
				mainStack.pop();
			} else if (mainStack.top().type == CalcValue::INX) {

				std::vector<ssize_t> index;
				GET_LIST_INDEX(mainStack, var_nodes, index);

				CalcValue *list;

				if (mainStack.top().type == CalcValue::REF) {
					CalcValue *tmp = mainStack.top().valAtRef(var_nodes);
					if (!tmp) {
						PASS_ERROR("\aERROR: $" << mainStack.top().string << " undefined or inaccessable.\n");
					}
					list = tmp->getListElem(index);
					mainStack.pop();

				} else if (mainStack.top().type == CalcValue::ARR)
					list = mainStack.top().getListElem(index);
				else {
					PASS_ERROR("\aERROR: invalid list index\n");
				}

				if (list->type == CalcValue::ARR) {
					list->list->push_back(val);
				} else {
					std::vector<CalcValue> tmpVec;
					tmpVec.push_back(*list);
					list->setValue(tmpVec);
					list->list->push_back(val);
				}
			} else if (mainStack.top().type == CalcValue::ARR) {
				mainStack.top().list->push_back(val);
			} else {
				push_into_arr:
				std::vector<CalcValue> tmpVec;
				tmpVec.push_back(mainStack.top());
				mainStack.top().setValue(tmpVec);
				mainStack.top().list->push_back(val);
			}

			// random number on [0,1]
		} else if (strcmp(p, "random") == 0) {
			mainStack.push((double) rand() / RAND_MAX);

			// convert to string
		} else if (strcmp(p, "str") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);

			if (val.type == CalcValue::STR)
				mainStack.push(val.getStr());
			else if (val.type == CalcValue::NUM) {
				char str[27];
				snprintf(str, 26, "%*.*g", 10, 16, val.getNum());
				mainStack.push(trimStr(str));

				// c++ solutions don't always work as desired :P
				//mainStack.push(std::to_string(val.getNum()).c_str());
			}
			// convert to list
		} else if (strcmp(p, "list") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);


			std::vector<CalcValue> tmp;
			while (!mainStack.empty()) {
				CONVERT_INDEX(mainStack, var_nodes);
				tmp.push_back(mainStack.top());
				mainStack.pop();
			}
			mainStack.push(tmp);

			// convert to number
		} else if (strcmp(p, "num") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
				mainStack.push(0.0);
			else if (val.type == CalcValue::NUM)
				mainStack.push(val.getNum());
			else if (val.type == CalcValue::STR)
				mainStack.push(atof(val.getStr()));

			// convert to an integer
		} else if (strcmp(p, "int") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
				mainStack.push(0.0);
			else if (val.type == CalcValue::NUM)
				mainStack.push(round(val.getNum()));
			else if (val.type == CalcValue::STR)
				mainStack.push( round( atof( val.getStr() ) ) );

			// truncate number
		} else if (strcmp(p, "floor") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
				mainStack.push(0.0);
			else if (val.type == CalcValue::NUM)
				mainStack.push(floor(val.getNum()));
			else if (val.type == CalcValue::STR)
				mainStack.push(atoi(val.getStr()));
			else {
				PASS_ERROR("\aERROR: floor expected a number\n");
			}

			// returns size of list at top of stak
		} else if (strcmp(p, "list_size") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			auto tmp = mainStack.top().list->size();
			mainStack.pop();
			mainStack.push((double) tmp);

			// initialize a list
		} else if (*p == '(') {

			char* newLine = NULL, * p_tmp = ++p;

			while (*p_tmp) {
				p_tmp++;
			}
			if (lineLen - (p_tmp - pInit) > 2) {
				*p_tmp = ' ';
			}

			std::string listBody = list::getList(p, codeFeed, freeable);
			//std::cout <<"listbody=\"" <<listBody <<"\"\n";
			if (listBody == "(") {
				PASS_ERROR("\aERROR: `(` invalid list, possible missing `)`\n");
			}

			std::vector<std::string> elems = list::split(listBody);

			/*
			for (int i = 0; i < elems.size(); i++) {
				std::cout <<'[' <<i <<"] : \"" <<elems[i] <<"\"\n";
			}*/

			std::stack<CalcValue> tmpStack;
			std::vector<CalcValue> tmp;
			std::vector<CalcValue> arr;
			char *str, *str_head;
			for (std::string elem : elems) {
				str_head = str = new char[elem.size() + 1];
				strcpy(str, elem.c_str());
				char *err = processLine(tmpStack, var_nodes, showErrors, str, elseStatement, codeFeed, freeable);
				if (err) {
					PASS_ERROR("\aERROR in element near `" << err << "`. in list:\n");
				}
				if (!tmpStack.empty()) {
					CONVERT_INDEX(tmpStack, var_nodes);
				}
				arr.push_back(tmpStack.empty() ? CalcValue() : tmpStack.top());
				emptyStack(tmpStack);
				delete[] str_head;
			}

			// empty lists actually contain no elems and not a null value
			if (arr.size() == 1 && arr[0].isNull())
				mainStack.push(std::vector<CalcValue>());
			else
				mainStack.push(arr);

			free(newLine);

			rpnln = p;

			// initialize a strStack
		} else if (*p == '{') {

			char* newLine = NULL, * tmp = ++p;

			while (*tmp) {
				tmp++;
			}
			if (lineLen - (tmp - pInit) > 2) {
				*tmp = ' ';
			}

			char* heap_str;
			StrStack* execArr = macro::getMacro(p, codeFeed, heap_str);
			freeable.push_back((void*) heap_str);
			//free(heap_str);

			//free's mem allocated for line
			free(newLine);

			if (execArr) {
				mainStack.push(*execArr);
				delete execArr;
			} else {
				PASS_ERROR("\aERROR: `{` could not getline(). Possible missing `}`\n");
				//strcpy(pInit, p);
				//free(p);
				//return pInit;
			}

			rpnln = p;


			// ummm hopefully the user actually fucked up and it's not my fault...
		} else if (*p == '}') {
			PASS_ERROR("\aERROR: `}` without previous `{`\n\n");

			// making a lambda/anonymous function
		} else if (strcmp(p, "lambda") == 0 || strcmp(p, "lam") == 0) {
			if (mainStack.size() < 2 || (elseStatement && mainStack.size() < 3)) {
				PASS_ERROR("\aERROR: lambda expected a body and a list of parameters\n" <<std::endl);
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::ARR) {
				PASS_ERROR("\aERROR: lambda expected a body and a list of parameters\n" <<std::endl);
			}
			Lambda lam;

			lam.params.reserve(mainStack.top().list->size());

			for (CalcValue val : *mainStack.top().list) {
				// normal parameter
				if (val.type == CalcValue::REF)
					lam.params.push_back(val.isNull() ? "" : val.string);
				else if (val.isNull()) {
					PASS_ERROR("\aERROR: null parameter");
				}

				else if (val.type == CalcValue::ARR) {
					// va_args
					if (val.list->size() == 1) {
						if (val.list->at(0).type != CalcValue::REF) {
							PASS_ERROR("\aERROR: lambda: invalid parameter: list containing only a variable means va_args, \
									list containing a variable and a macro handles optional parameters\n");
						}
						std::string v = " ";
						lam.params.push_back(v + val.list->at(0).string); // " a"

						// optional parameter
					} else if (val.list->size() == 2) {
						if (val.list->at(0).type != CalcValue::REF || val.list->at(1).type != CalcValue::BLK) {
							PASS_ERROR("\aERROR: lambda: invalid parameter: list containing only a variable means va_args, \
									list containing a variable and a macro handles optional parameters\n");
						}

						std::string space = " ";

						size_t sze = 100;
						char* s_tmp = (char*) malloc(sze);
						val.list->at(1).block->toString(&s_tmp, &sze);
						lam.params.push_back(space + val.list->at(0).string + space + s_tmp); // " a a 5 ="
						free(s_tmp);

					} else {
						PASS_ERROR("\aERROR: lambda: invalid parameter: list containing only a variable means va_args, \
									list containing a variable and a macro handles optional parameters\n");
					}
				} else {
					PASS_ERROR("\aERROR: lambda: invalid parameter: list containing only a variable means va_args, \
									list containing a variable and a macro handles optional parameters\n");
				}
			}

			int paramErr = lam.validParams();
			if (paramErr >= 0) {
				PASS_ERROR("\aERROR: lambda: parameter: invalid or misplaced parameter number " <<paramErr + 1\
				           <<". special parameters must be at the end\n")
			}

			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type != CalcValue::BLK) {
				PASS_ERROR("\aERROR: lambda expected a body and a list of parameters\n" <<std::endl);
			}

			lam.src = *mainStack.top().block;

			mainStack.pop();

			mainStack.push(lam);

		// eval operator
		} else if ((*p == '@' && *(p + 1) == '\0') || strcmp(p, "eval") == 0) {
			ASSERT_NOT_EMPTY(p);

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue top = CalcValue(mainStack.top());
			mainStack.pop();

			bool elseStatement_cpy = elseStatement;
			elseStatement = false;
			if (top.type == CalcValue::BLK) {
				RUN_STR_STK(*top.block, mainStack);
			} else if (top.type == CalcValue::STR) {
				char* err = processLine(mainStack, var_nodes, showErrors, top.string, elseStatement, codeFeed, freeable);
				if (err) {
					PASS_ERROR("\aERROR in block near `" <<err <<"`. Called here:\n");
				}

			// note: after adding va_args and missing handlers, lambda execution performance has decreased significantly
			}  else if (top.type == CalcValue::LAM) {

				//ASSERT_NOT_EMPTY(p);
				if (!mainStack.empty()) {
					CONVERT_INDEX(mainStack, var_nodes);
					//CONVERT_REFS(mainStack, var_nodes); // could cause problems later on
				}
				// no arguments provided
				if (mainStack.empty() || mainStack.top().type != CalcValue::ARR)
					mainStack.push(std::vector<CalcValue>());

				// verify we dont have too many arguments
				long max_args = top.lambda->max_args(); // -1 = no max
				if (max_args != -1 && (long) mainStack.top().list->size() > max_args) {
					PASS_ERROR("\aERROR: too many arguments for the given lambda expression");
				}

				// which parameter gets assigned which argument(s)
				std::vector<int16_t> paramBindings;
				paramBindings = top.lambda->bindArgs(mainStack.top().list->size());

				/*std::cout <<"params=";
				for (std::string& i : top.lambda->params)
					std::cout <<i <<",";
				std::cout <<"\n";

				std::cout <<"parambindings=";
				for (int16_t i : paramBindings)
					std::cout <<i <<", ";
				std::cout <<"\n";
				*/

				// not enough args :(
				bool hasArgs = top.lambda->params.size() && paramBindings.size();
				if (paramBindings.size() && paramBindings[0] == -1) {
					if (top.lambda->requiredArgs() < 0) {
						PASS_ERROR("\aERROR: lambda expected at least "<< (int) -top.lambda->requiredArgs() <<" arguments\n");
					} else {
						PASS_ERROR("\aERROR: lambda expected " << (int) top.lambda->requiredArgs() <<" arguments\n");
					}
				}

				// empty args list won't get popped later as hasArgs == false
				if (!mainStack.empty() && !mainStack.top().list->size()) {
					mainStack.pop();
				}

				// add layer to scope
				var_nodes.push_back(UserVar(NULL, " ", 0.0));

				// this gonna get rly complicated
				if (hasArgs) {
					uint16_t i;

					// for each argument
					for (i = 0; i < paramBindings.size(); i++) {

						// if coresponding parameter is a va_args and the end of the arg list
						// we fill it with the remaining args
						if (i + 1U < paramBindings.size() && paramBindings[i] == paramBindings[i + 1]) { // va_args
							//std::cout <<"filling va_arg1\n";
							std::vector<CalcValue> args;
							for (; i < paramBindings.size(); i++)
								args.push_back(mainStack.top().list->at(i));
							i--;


							//std::cout <<"Assigning ["<<i<<":" <<paramBindings[i] <<"] $" <<top.lambda->params[paramBindings[i]].c_str() + 1<<"\n";
							vars::assignNewVar(var_nodes,
							                   top.lambda->params[paramBindings[i]].c_str() + 1,
							                   CalcValue(args));


						// if coresponding parameter is a va_args we fill it with the remaining args
						} else if (top.lambda->countSpaces(paramBindings[i]) == 1) { // va_arg
							//std::cout <<"filling va_arg2\n";
							std::vector<CalcValue> arg_s;
							arg_s.push_back(mainStack.top().list->at(i));

							vars::assignNewVar(var_nodes,
							                   top.lambda->params[paramBindings[i]].c_str() + 1,
							                   CalcValue(arg_s));


						// the parameter has been provided and thus we ignore the missing handler
						} else if (top.lambda->countSpaces(paramBindings[i]) == 2) { // var with default handler
							// copy param into tmp string
							char var_name[top.lambda->params[paramBindings[i]].length()];
							strcpy(var_name, top.lambda->params[paramBindings[i]].c_str() + 1);

							// delete handler from cpy
							char* tmp = var_name;
							while (*tmp && *tmp != ' ')
								tmp++;
							if (*tmp)
								*tmp = '\0';

							// prevent cyclic references from occuring by mutilateing reference names
							if (mainStack.top().list->at(i).type == CalcValue::REF) {
								// { } ($a) lam $fxn =
								// $a 6 =
								// ($a) $fxn @
								// assign var: "$ a" val_at($a)
								// set $a = ref"$ a"
								// set ($a) = ref"$ a"



								UserVar* end = vars::findVar(var_nodes, mainStack.top().list->at(i).string);
								if (!end) {
									mainStack.push(mainStack.top().list->at(i));
									CONVERT_REFS(mainStack, var_nodes); // this will error and exit
								}
								// find last variable argument references
								//UserVar* end = vars::lastVarInRefChain(var_nodes, mainStack.top().list->at(i).string);
								// broken reference
								//if (!end) {
								//	mainStack.push(mainStack.top().list->at(i));
								//	CONVERT_REFS(mainStack, var_nodes); // this will error and exit
								//}

								char* ref_name = end->name;

								// var->val isn't escaped
								if (end->name[0] != ' ') {
									// set it to a reference to an identical and safer variable
									char* v_name = mutilate::mutilateVarName(end->name);
									vars::assignNewVar(end->first, v_name, end->val);
									end->val.setRef(v_name);
									free(v_name);
									ref_name = end->val.string;
								}

								vars::assignNewVar(var_nodes,
								                   var_name,
								                   CalcValue().setRef(ref_name));
							} else
								vars::assignNewVar(var_nodes,
								                   var_name,
								                   mainStack.top().list->at(i));

						// this really should be the first condition...
						} else if (top.lambda->countSpaces(paramBindings[i]) == 0) { // normal var
							// prevent cyclic references from occuring by mutilateing reference names
							if (mainStack.top().list->at(i).type == CalcValue::REF) {
								// { } ($a) lam $fxn =
								// $a 6 =
								// ($a) $fxn @
								// assign var: "$ a" val_at($a)
								// set $a = ref"$ a"
								// set ($a) = ref"$ a"


								UserVar* end = vars::findVar(var_nodes, mainStack.top().list->at(i).string);
								if (!end) {
									mainStack.push(mainStack.top().list->at(i));
									CONVERT_REFS(mainStack, var_nodes); // this will error and exit
								}
								// find last variable argument references
								//UserVar* end = vars::lastVarInRefChain(var_nodes, mainStack.top().list->at(i).string);
								// broken reference
								/*if (!end) {
									mainStack.push(mainStack.top().list->at(i));
									CONVERT_REFS(mainStack, var_nodes); // this will error and exit
								}*/

								char* ref_name = end->name;

								// var->val isn't escaped
								if (end->name[0] != ' ') {
									// set it to a reference to an identical and safer variable
									char* var_name = mutilate::mutilateVarName(end->name);
									vars::assignNewVar(end->first, var_name, end->val);
									end->val.setRef(var_name);
									free(var_name);
									ref_name = end->val.string;
								}

								vars::assignNewVar(var_nodes,
								                   top.lambda->params[paramBindings[i]].c_str(),
								                   CalcValue().setRef(ref_name));
							} else
								vars::assignNewVar(var_nodes,
								                   top.lambda->params[paramBindings[i]].c_str(),
								                   mainStack.top().list->at(i));
						}
						/*
						//vars::assignVar(var_nodes, top.lambda->params[i].c_str(), mainStack.top().list->at(i)); y u no work :/ ?
						UserVar *var = new UserVar(&first_node,
						                           top.lambda->params[i].c_str(),
						                           mainStack.top().list->at(i));
						//var->val.type = lhs.type;
						vars::lastVar(&var_nodes[var_nodes.size() - 1])->next = var;
						*/
					}

					// optional variables weren't defined
					if (i < top.lambda->params.size()) {

						// handle each undefined variable
						for (; i < top.lambda->params.size(); i++) {
							//std::cout <<"ignoring missing handler `" <<top.lambda->params.at(i) <<"` cs=" <<top.lambda->countSpaces(i) <<std::endl;
							// handle undefined optional params
							if (top.lambda->countSpaces(i) == 2) {
								// variable doesnt get defined here but handler gets run

								char* tmp = (char*) malloc(strlen(top.lambda->params[i].c_str()));
								strcpy(tmp, top.lambda->params[i].c_str() + 1);

								unsigned int c = 0;
								while (tmp[c] != ' ') c++;
								tmp[c] = '\0';

								char* tmp2 = tmp + c + 1;
								char* err = processLine(mainStack, var_nodes, showErrors, tmp2, elseStatement, codeFeed, freeable);
								if (err) {
									PASS_ERROR("\aERROR: in optional variable handler `" <<err <<"`. Called here:\n");
								}
								free(tmp);

							// empty va_args list
							} else if (top.lambda->countSpaces(i) == 1) {
								vars::assignNewVar(var_nodes,
								                   top.lambda->params[i].c_str() + 1,
								                   std::vector<CalcValue>());
							}
						}
					}
					// done with args list
					mainStack.pop();

				// no args provided but we still might need to fill in params
				} else if (top.lambda->params.size()) {
					// handle each undefined variable
					for (unsigned int i = 0; i < top.lambda->params.size(); i++) {
						//std::cout <<"Handling missing param `" <<top.lambda->params.at(i) <<"` cs=" <<top.lambda->countSpaces(i) <<std::endl;
						// handle undefined optional params (call missing handler)
						if (top.lambda->countSpaces(i) == 2) {
							// variable doesnt get defined here but handler gets run

							char* tmp = (char*) malloc(strlen(top.lambda->params[i].c_str()));
							strcpy(tmp, top.lambda->params[i].c_str() + 1);

							unsigned int c = 0;
							while (tmp[c] != ' ') c++;
							tmp[c] = '\0';

							char* tmp2 = tmp + c + 1;
							char* err = processLine(mainStack, var_nodes, showErrors, tmp2, elseStatement, codeFeed, freeable);
							if (err) {
								PASS_ERROR("\aERROR: in optional variable handler `" <<err <<"`. Called here:\n");
							}
							free(tmp);

						// empty va_args list
						} else if (top.lambda->countSpaces(i) == 1) {
							vars::assignNewVar(var_nodes,
							                   top.lambda->params[i].c_str() + 1,
							                   std::vector<CalcValue>());
						}
					}


				}


				// put the statement in a string
				size_t buff_size = 500;
				char* buff = (char*) malloc(buff_size);
				top.lambda->src.toString(&buff, &buff_size);

				// put the string in a temp file
				FILE* statement = tmpfile();
				fputs(buff, statement);
				rewind(statement);
				free(buff);

				std::stack<CalcValue> fxnStack;

				// run the lambda/tmpfile
				macro::ret_t ret = runFile(statement, var_nodes, showErrors, fxnStack, elseStatement, freeable);

				// check return value (why did the lambda finish?)
				if (ret == macro::ERROR) {
					PASS_ERROR("\aERROR: @ (exec operator) failed\n");
				}
				fclose(statement);

				// variables go out of scope
				vars::clearScope(var_nodes);

				// reverse order of stack
				std::stack<CalcValue> tmpStack;
				while (!fxnStack.empty()) {
					if (fxnStack.top().type == CalcValue::INX) {
						CONVERT_INDEX(fxnStack, var_nodes);
					}
					tmpStack.push(fxnStack.top());
					fxnStack.pop();
				}

				// add it to mainStack
				while (!tmpStack.empty()) {
					mainStack.push(tmpStack.top());
					tmpStack.pop();
				}


			} else {
				PASS_ERROR("\aERROR: @ (execution operator) only accepts strings and executable arrays\n");
			}
			elseStatement = elseStatement_cpy;

			// conditionals::else
		} else if (strcmp(p, "else") == 0) {
			/// sets else flag true so we know that we need to deal with it later
			ASSERT_NOT_EMPTY(p);
			//CONVERT_INDEX(mainStack, var_nodes);
			//CONVERT_REFS(mainStack, var_nodes);
			elseStatement = true;

			// conditionals::elseif
		} else if (strcmp(p, "elseif") == 0) {
			/// works by combining else and elseif statement into a single else
			/// statement with an if-else system inside
			/// multi-statment if elseif statments will have multiple layers

			if (mainStack.size() < 2 || (elseStatement && mainStack.size() < 3)) {
				PASS_ERROR("\aERROR: elseif expected a condition and a block of code (takes 2 arguments)\n" <<std::endl);
			}

			StrStack newElseClause;
			newElseClause.push("\n{\n");

			bool condition = mainStack.top().getNum() != 0;
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (elseStatement) {
				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs macros to function");
				}
				StrStack elseifBlock = *mainStack.top().block;
				mainStack.pop();

				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);

				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs macros to function");
				}
				StrStack elseBlock = *mainStack.top().block;
				mainStack.pop();

				StrStack::appendToStack(newElseClause, elseBlock);
				newElseClause.push("\n} else {");
				StrStack::appendToStack(newElseClause, elseifBlock);
				if (condition) {
					newElseClause.push("\n} 1 if");
				} else {
					newElseClause.push("\n} 0 if");
				}

				mainStack.push(newElseClause);
			} else {
				elseStatement = true;
			}


			// comditionals::if
		} else if (strcmp(p, "if") == 0) {
			// verify we have enough data for the operator
			if (elseStatement && mainStack.size() < 3) {
				PASS_ERROR("\aERROR: if-else statement requires a condition and 2 blocks of code\n");
			} else if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: if expected a condition and a block of code (takes 2 arguments)\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			bool condition = mainStack.top().getNum() != 0;
			mainStack.pop();

			// branching for else statements and the condition
			if (elseStatement) // { } else { } condition if
				if (condition) {
					CalcValue runTrue = mainStack.top();
					mainStack.pop(); mainStack.pop();
					if (runTrue.type == CalcValue::BLK) {
						RUN_STR_STK(*runTrue.block, mainStack);
					} else
						mainStack.push(runTrue);

				} else {
					mainStack.pop();
					if (mainStack.top().type == CalcValue::BLK) {
						CalcValue top = mainStack.top();
						mainStack.pop();
						if (top.type == CalcValue::BLK) {
							RUN_STR_STK(*top.block, mainStack);
						} else
							mainStack.push(top);

					} // else, it's a value that should stay at the top of the stack
				}

			else // { code } condition if
			if (condition) {
				if (mainStack.top().type == CalcValue::BLK) {
					CalcValue top = mainStack.top();
					mainStack.pop();
					if (top.type == CalcValue::BLK) {
						RUN_STR_STK(*top.block, mainStack);
					} else
						mainStack.push(top);

				} // else, it's a value that should stay at the top of the stack
			} // else, don't do anything as there isn't an else clause

			// errs if stack top is false
		} else if (strcmp(p, "assert") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (!mainStack.top().getNum()) {
				PASS_ERROR("\aERROR: assertion test failed\n");
			}
			mainStack.pop();

			// runs the same block of code a given number of times
		} else if (strcmp(p, "repeat") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: repeat loop needs a number of times to execute and a block.\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			size_t timesToRepeat = (size_t) abs(mainStack.top().getNum());
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			StrStack block;
			if (mainStack.top().type == CalcValue::BLK)
				block = (*mainStack.top().block);
			else {
				PASS_ERROR("\aERROR: repeat loop expected a block of code to execute.\n")
			}
			mainStack.pop();

			for (; timesToRepeat > 0; timesToRepeat--) {
				RUN_STR_STK(block, mainStack);
			}

			// while loop
		} else if (strcmp(p, "while") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: while loop needs two blocks - a condition and a process.\n");
			}

			// verify we have a runnable expression for our condition
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::BLK) {
				PASS_ERROR("\aERROR: while: condition must be a block/subroutine\n");
			}

			// extract condition
			StrStack condBlock = *mainStack.top().block;
			mainStack.pop();
			std::stack<CalcValue> condStack;

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue top = mainStack.top();

			// main-loop
			for (;;) {
				RUN_STR_STK(condBlock, condStack);
				// if condition evaluates to false (or doesnt give a value)
				if (condStack.empty() || !condStack.top().getNum())
					break;

				// run process
				if (top.type == CalcValue::BLK) {
					RUN_STR_STK(*top.block, mainStack);
				} else
					mainStack.push(top);
			}

			// iterate through a list or a macro
		} else if (strcmp(p, "for-each") == 0 || strcmp(p, "foreach") == 0) {
			// we have at least 3 things... the first is a reference
			if (mainStack.size() < 3 || mainStack.top().type != CalcValue::REF) {
				PASS_ERROR("\aERROR: malformed for-each statement \n correct format: `{ process } list $var for-each`\n"
						           << std::endl);
			}

			// if it hasn't been assigned yet..
			/*UserVar* var = vars::findVar(var_nodes, mainStack.top().string); // container variable
			if (var == NULL) {
				var = new UserVar(first_node, mainStack.top().string, CalcValue());
				vars::lastVar(first_node)->next = var;
			}*/

			UserVar iterator_scope(NULL, " ", CalcValue());
			var_nodes.push_back(iterator_scope);
			UserVar *var = vars::assignVar(var_nodes, mainStack.top().string, CalcValue());

			// get list
			mainStack.pop();
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type != CalcValue::BLK && mainStack.top().type != CalcValue::ARR) {
				PASS_ERROR(
						"\aERROR: malformed for-each statement \n correct format: `{ process } $list $var for-each`\n"
								<< std::endl);
			}

			if (mainStack.top().type == CalcValue::BLK) {
				// get list
				std::stack<CalcValue> tmpStack;
				RUN_STR_STK(*mainStack.top().block, tmpStack);
				mainStack.pop();

				// get process
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR(
							"\aERROR: malformed for-each statement \n correct format: `{ process } $list $var for-each`\n");
				}

				StrStack process(*mainStack.top().block);
				mainStack.pop();

				while (!tmpStack.empty()) {
					var->val = tmpStack.top();
					RUN_STR_STK(process, mainStack);
					tmpStack.pop();
				}
			} else {
				// get list
				std::vector<CalcValue> vec = *mainStack.top().list;
				mainStack.pop();

				// get process
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR(
							"\aERROR: malformed for-each statement \n correct format: `{ process } (list) $var for-each`\n");
				}

				StrStack process(*mainStack.top().block);
				mainStack.pop();

				for (CalcValue &cv : vec) {
					var->val = cv;
					RUN_STR_STK(process, mainStack);
				}

			}

			// clear the iterator scope
			vars::clearScope(var_nodes);

			// sleep fxn
		} else if (strcmp(p, "sleep") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);


			/* perhaps add back after adding multi-threading support?
			#include <chrono>
			#include <thread>
			std::this_thread::sleep_for(std::chrono::microseconds((unsigned) (1000 * mainStack.top().number)));
			 */

			// accepts miliseconds but has microsecond accuracy
			usleep((unsigned long)(mainStack.top().number * 1000));
			mainStack.pop();

			// exit the program
		} else if ((*p == 'q' && *(p + 1) == '\0')
		           || !strcmp(p, "exit") || !strcmp(p, "quit")
				) {
			exit(EXIT_SUCCESS);

			// exit the current macro and leave all the values currently on the stack
		} else if (strcmp(p, "break") == 0) {
			return (char *) lambda_finish;
		} else if (strcmp(p, "pass") == 0) {
			rpnln = (char*) lambda_finish;
			return (char *) lambda_finish;
			// exit the lambda and only leave the top on the stack
		} else if (strcmp(p, "return") == 0) {
			if (!mainStack.empty()) {
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				CalcValue top = mainStack.top();
				emptyStack(mainStack);
				mainStack.push(top);
			}
			rpnln = (char*) lambda_finish;
			return (char*) lambda_finish;

			// show help
		} else if (strcmp(p, "help") == 0) {
			displayHelp();

			// clear screen
		} else if (strcmp(p, "clear") == 0 || strcmp(p, "cls") == 0) {
#ifdef _WIN32
			system("cls");
#else
			system("clear");
#endif

			// essentially restarts the program (don't display help)
		} else if (strcmp(p, "reset") == 0 ) { //
			emptyStack(mainStack);
			for (UserVar node : var_nodes)
				vars::wipeAll(&node);

			// useful for debugging
		} else if (strcmp(p, "vars") == 0 || strcmp(p, "ls_vars") == 0) {
			std::cout <<"Defined Variables:\nNote: variables with spaces after the `$` and a crunch-code at\nthe end are defined by the interpreter for safety.\n";
			// for each scope
			for (size_t i = 0; i < var_nodes.size(); i++) {

				std::cout <<"Scope<" <<i <<"> ====\n";
				UserVar *var = var_nodes[i].next;

				// for each variable in scope
				while (var != NULL) {
					if (var->val.type == CalcValue::NUM)
						std::cout << "[NUM] @ " << var << ": $" << var->name << ' '
						          << var->val.getNum() << " =\n";
						// var is null (print null)
					else if (var->val.type == CalcValue::STR && var->val.isNull())
						std::cout << "[NIL] @ " << var << ": $" << var->name << ' '
						          << "null =\n";

						// var is a string (print contents)
					else if (var->val.type == CalcValue::STR)
						std::cout << "[STR] @ " << var << ": $" << var->name << " \""
						          << var->val.getStr() << "\" =\n";

						// var is a reference (show what it points to)
					else if (var->val.type == CalcValue::REF)
						std::cout << "[REF] @ " << var << ": $" << var->name << " $"
						          << var->val.getRef() << " =\n";

						// var is a macro (give details)
					else if (var->val.type == CalcValue::BLK)
						std::cout << "[BLK] @ " << var << ": $" << var->name << " has "
						          << var->val.block->stackDepth
						          << ((var->val.block->stackDepth == 1) ? " line\n" : " lines\n");

						// variable is a list (print contents of list)
					else if (var->val.type == CalcValue::ARR) {
						std::cout << "[ARR] @ " << var <<": $" <<var->name <<" has "
						          <<var->val.list->size() <<(var->val.list->size() == 1 ? "element" : " elements\n");
						//printCalcValue(var->val, var_nodes);
						std::cout <<" =\n";
					}

					else if (var->val.type == CalcValue::LAM) {
						std::cout << "[LAM] @ " <<var <<": (";
						bool hasParams =var->val.lambda->params.size() && var->val.lambda->params[0] != "";
						if (hasParams) {
							std::cout <<"$" <<var->val.lambda->params[0];
							for (uint16_t param = 1; param < var->val.lambda->params.size(); param++) {
								std::cout <<", $" <<var->val.lambda->params[param];
							}
						}
						std::cout <<") $"<< var->name <<std::endl;
					}
					var = var->next;
				}

			}

			// print the contents of the stack
		} else if (strcmp(p, "stack") == 0)
			commands::debugStack(mainStack, var_nodes);

			// prints the name of the main file being run
		else if (strcmp(p, "__file") == 0)
			mainStack.push(progName);

			// typeof function
		else if (strcmp(p, "typeof") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);

			if (mainStack.top().type == CalcValue::REF) {
				UserVar* var = vars::findVar(var_nodes, mainStack.top().string);
				if (var)
					mainStack.top().setValue(var->val);
			}

			CalcValue val = mainStack.top();
			mainStack.pop();

			if (val.isNull()) // NULL string pointer
				mainStack.push("NULL_VAL");

			else if (val.type == CalcValue::STR) // string-type
				mainStack.push("string"); // STR

			else if (val.type == CalcValue::NUM) // number-type
				mainStack.push("number/boolean"); // NUM/BLN

			else if (val.type == CalcValue::REF) // variable reference
				mainStack.push("reference");

			else if (val.type == CalcValue::BLK) // string_stack
				mainStack.push("executable array");

			else if (val.type == CalcValue::ARR) // list
				mainStack.push("list");

			else if (val.type == CalcValue::LAM) // lambda
				mainStack.push("lambda");

			// system call (problem: this conflicts with the current strategy for handling if statements.....)
		} else if (strcmp(p, "sys") == 0 || strcmp(p, "system") == 0) {

			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().isStr())
				system(mainStack.top().getStr()); // gets run in BASH/CMD

			else if (mainStack.top().type == CalcValue::NUM) {
				PASS_ERROR("\aERROR: cannot make a system call with a number...\n" <<std::endl);
			}
			mainStack.pop();

			// assignment operator
		} else if (*p == '=' && *(p + 1) == '\0') { // variable assignment

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: not enough data for assignment. (takes 2 arguments)\n" <<std::endl);
			}

			if (mainStack.top().type == CalcValue::INX) {
				CONVERT_INDEX(mainStack, var_nodes);
			}

			CalcValue rhs = getNextValue(mainStack);

			CalcValue lhs = getNextValue(mainStack);

			if (lhs.type == CalcValue::REF ) {
				UserVar *var = vars::findVar(var_nodes, lhs.string);

				while (var && var->val.type == CalcValue::REF)
					var = vars::findVar(var_nodes, var->val.string);

				if (var == NULL) {
					var = new UserVar(&var_nodes[var_nodes.size() - 1], lhs.string, rhs);
					//var->val.type = rhs.type;
					vars::lastVar(&var_nodes[var_nodes.size() - 1])->next = var;
					//vars::assignVar(var_nodes, lhs.string, rhs);
					// changing the variable's value
				} else
					var->setValue(rhs);

			} else if (lhs.type == CalcValue::INX) {
				mainStack.push(lhs);

				std::vector<ssize_t> index;
				GET_LIST_INDEX(mainStack, var_nodes, index);

				CalcValue list = mainStack.top();
				mainStack.pop();

				if (list.type == CalcValue::REF) {
					CalcValue* tmp = list.valAtRef(var_nodes);
					if (!tmp) {
						PASS_ERROR("\aERROR: $"<< list.string <<" undefined or inaccessable.\n");
					}
					tmp->assignElem(index, rhs);
				} else {
					PASS_ERROR("\aERROR: invalid use of list assignment");
				}

			} else if (rhs.type == CalcValue::REF) {
				UserVar* var = vars::findVar(var_nodes, rhs.string);

				while (var && var->val.type == CalcValue::REF)
					var = vars::findVar(var_nodes, var->val.string);

				if (var == NULL) {
					var = new UserVar(&var_nodes[var_nodes.size() - 1], rhs.string, lhs);
					//var->val.type = lhs.type;
					vars::lastVar(&var_nodes[var_nodes.size() - 1])->next = var;
					//vars::assignVar(var_nodes, rhs.string, lhs);

					// changing the variable's value
				} else
					var->setValue(lhs);

				// nothing that can hold data
			} else {
				PASS_ERROR("\aERROR: inappropriate use of assignment operator. (no assignable term given)\n" <<std::endl);
			}


			// is the given variable/reference defined?
		} else if (strcmp(p, "is_defined") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::REF) {
				PASS_ERROR("\aERROR: is_defined expected a reference\n");
			}

			mainStack.push(vars::varExists(var_nodes, getNextValue(mainStack).string));

			// copy operator
		} else if (*p == '~' && *(p + 1) == '\0') {
			if (mainStack.empty()){
				PASS_ERROR("\aERROR: not enough data for copy operator (`~`)\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			// error reporting can get annoying on final programs
			// toggle errors
		} else if (strcmp(p, "__errors-off") == 0)
			showErrors = false;
		else if (strcmp(p, "__errors-on") == 0)
			showErrors = true;
		else if (strcmp(p, "__errors") == 0)
			mainStack.push(showErrors);


			// show version info
		else if (strcmp(p, "version") == 0)
			printVersionInfo();

			// delete a variable
		else if (strcmp(p, "delete") == 0) {
			ASSERT_NOT_EMPTY(p);
			// deleting an element of a list
			if (mainStack.top().type == CalcValue::INX) {
				std::vector<ssize_t> indx;
				GET_LIST_INDEX(mainStack, var_nodes, indx);
				if (mainStack.top().type == CalcValue::ARR) {
					mainStack.top().deleteListElem(indx);
				} else if (mainStack.top().type == CalcValue::REF) {
					CalcValue* cv = mainStack.top().valAtRef(var_nodes);
					if (!cv) {
						PASS_ERROR("\aERROR: broken reference to $" <<mainStack.top().string <<"\n");
					}
					if (cv->type != CalcValue::ARR) {
						PASS_ERROR("\aERROR: index without list\n");
					}
					cv->deleteListElem(indx);
					mainStack.pop();
				}

				// deleting a variable
			} else if (mainStack.top().type == CalcValue::REF) {
				vars::removeVar(vars::findVar(var_nodes, mainStack.top().string)->first, mainStack.top().string);
				mainStack.pop();
			} else {
				PASS_ERROR("\aERROR: delete expected a variable/reference or a list index to delete\n");
			}

			// clear the stack
		}else if  (strcmp(p, "...") == 0)
			emptyStack(mainStack);

			// pop the top of the stack
		else if  (*p == ';' && *(p + 1) == '\0') {
			if (!mainStack.empty())
				mainStack.pop();

			// pushes the stack size to the stack
		} else if (strcmp(p, "stklen") == 0)
			mainStack.push(mainStack.size());

			// swap the top 2 elements in the stack
		else if (strcmp(p, "swap") == 0) {
			CONVERT_INDEX(mainStack, var_nodes);

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy `" <<p <<"` operator.\n");
			}
			// take the top 2 elements from the top
			CalcValue val1 = getNextValue(mainStack);
			CalcValue val2 = getNextValue(mainStack);

			// second val could be part of an index
			if (val2.type == CalcValue::INX) {
				mainStack.push(val2);
				CONVERT_INDEX(mainStack, var_nodes);
				val2 = getNextValue(mainStack);
			}

			// push them back in reverse order
			mainStack.push(val1);
			mainStack.push(val2);

			// duplicate the top of the stack
		} else if (strcmp(p, "dup") == 0) {
			ASSERT_NOT_EMPTY(p);
			mainStack.push(mainStack.top());

			// duplicate the top elements a set number of times
		} else if (strcmp(p, "dupx") == 0 || strcmp(p, "dupn") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy `" <<p <<"` operator." <<std::endl);
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: operator `" <<p <<"` expected a number\n");
			}

			double copies = mainStack.top().getNum();
			mainStack.pop();

			while (copies-- > 1)
				mainStack.push(mainStack.top());

			// reverses the order of the stack
		} else if (strcmp(p, "reverse_stack") == 0) {
			std::stack<CalcValue> tmpStack;

			while (!mainStack.empty()) {
				if (mainStack.top().type == CalcValue::INX) {
					CONVERT_INDEX(mainStack, var_nodes);
				}
				tmpStack.push(mainStack.top());
				mainStack.pop();
			}

			mainStack = tmpStack;

		} else if (strcmp(p, "print_macro") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::BLK)
			{ PASS_ERROR("\aERROR: remove `print_macro` from ur code..."); }
			size_t size = 512;
			char* macro = (char*) malloc(size);
			mainStack.top().block->toString(&macro, &size);
			printf("%s", macro);
			free(macro);

			// retarded users...
		} else if (*p == '\'') {
			PASS_ERROR("\aERROR: strings are enclosed in double-quotes (\")\n");

			// user has given a string :D
		} else if (*p == '\"')
			mainStack.push(p + 1);

			// reference
		else if (*p == '$' && *(p + 1) != '\0') // user must use '$' prefix to access the variables
			mainStack.push(CalcValue().setRef(p + 1)); // simplest line in this file?

			// let's try and figure out what this could be...
		else {
			// parse input
			double number = atof(p);

			// the user is still learning
			if (number == 0 && *p != '0' && (*p != '-' && *(p + 1) != '0')) {
				PASS_ERROR("\aSYNTAX ERROR: near `" <<p <<"`\nfirst token = `" <<pInit <<"`\nstklen = "
				                                    <<mainStack.size() <<"\nrpnln=\"" <<rpnln <<"\"\n");

				// the user has given us a number
			} else
				mainStack.push(number);
		}

		// get next token

		p = qtok(rpnln, &rpnln);

	}


	return NULL;

}


#endif
