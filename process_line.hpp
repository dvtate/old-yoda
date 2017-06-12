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



extern char* progName;

// error's if the stack is empty
#define ASSERT_NOT_EMPTY(OPERATOR)\
	if (mainStack.empty()) {\
		if (showErrors)\
			std::cerr <<"\aERROR: not enough data for `" <<OPERATOR <<"`.\n";\
		return p;\
	}

// closes with an error
#define PASS_ERROR(MSG) \
	if (showErrors)	\
		std::cerr <<MSG;\
	return p;

#define GET_LIST_INDEX(MAINSTACK, VAR_NODES, ASSIGN_TO)\
	if (MAINSTACK.top().type == CalcValue::INX) {\
		while (!MAINSTACK.empty() && mainStack.top().type == CalcValue::INX) {\
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
            PASS_ERROR("\aERROR: List index out of bounds");\
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
		fputs(buff, statement);\
		rewind(statement);\
		free(buff);\
\
		/* add layer to scope*/\
		UserVar first_node(NULL, " ", 0.0);\
		first_node.first = &first_node;\
		var_nodes.push_back(first_node);\
\
		/* run the temp file */\
		if (runFile(statement, var_nodes, showErrors, (STACK), elseStatement)) {\
			PASS_ERROR("\aERROR: @ (exec operator) failed\n");\
		}\
		fclose(statement);\
\
		/* variables go out of scope */\
		UserVar* node = &var_nodes[var_nodes.size() - 1];\
		vars::wipeAll(node);\
		var_nodes.pop_back();\
	}


extern bool runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
                    std::stack<CalcValue>& mainStack, bool& elseStatement
);

/// returns: location/source of error or NULL
/// params: environment/operation variables
/// this function runs the user's code, most essential part of the interpreter
char* processLine(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                  bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed
){

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
		    || !strcmp(p, "&&") || !strcmp(p, "||") // logical operators
		    || !strcmp(p, "<=") || !strcmp(p, ">=")
		    || !strcmp(p, "logBase") || !strcmp(p, "logbase")
		    || !strcmp(p, "pow") // for those who dont like "**"
				) {

			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy operator `" <<p <<"`.\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operator `" <<p <<"`. (expected two numbers)\n");
			}

			double b = getNextValue(mainStack).getNum();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operator `" <<p <<"`. (expected two numbers)\n");
			}

			double a = getNextValue(mainStack).getNum();

			switch (*p) {
				case '*':
					if (*(p + 1) == '*')
						mainStack.push(pow(a, b));
					else
						mainStack.push(a * b);
					break;

				case '/': mainStack.push(a / b); break;
				case '-': mainStack.push(a - b); break;
				case '%': mainStack.push((int) a % (int) b); break;
				case '^': mainStack.push((int) a ^ (int) b); break;
				case '|':
					if (*(p + 1) == '\0') // bitwise
						mainStack.push((int) a | (int) b);
					else // logical
						mainStack.push(a || b);
					break;

				case '&':
					if (*(p + 1) != '\0')
						mainStack.push(a && b);
					else
						mainStack.push((int) a & (int) b);
					break;

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
			if (a.isEmpty() != b.isEmpty()) { // val + null
				if (a.isEmpty())
					mainStack.push(b);
				else
					mainStack.push(a);

				// get next token
				p = qtok(rpnln, &rpnln);

				continue;

			} else if (a.isEmpty() && b.isEmpty()) { // null + null
				mainStack.push(a);

				// get next token
				p = qtok(rpnln, &rpnln);

				continue;

			}

			// branching out all 4 permutations of string and num
			if (a.type == CalcValue::STR) {
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
			}
		} else if (strcmp(p, "==") == 0) {
			mainStack.push((getNextValue(mainStack) == getNextValue(mainStack)));

			// not equal to
		} else if (strcmp(p, "!=") == 0)
			mainStack.push(!(getNextValue(mainStack) == getNextValue(mainStack)));

			// logical not operator
		else if (*p == '!' && *(p + 1) == '\0')
			mainStack.push((getNextValue(mainStack).getNum() == 0));


			//trig functions
		else if (strcmp(p, "sin") == 0) {
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
					for (char ch : str) {
						char chr[2] = {ch, '\0'};
						mainStack.push(chr);
					}

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

					char *pch = strtok(str, delims);
					while (pch) {
						mainStack.push(pch);
						pch = strtok(NULL, delims);
					}
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

				int i = mainStack.top().number;

				if (abs(i) > strlen(mainStack.top().string) || i == strlen(mainStack.top().string)) {
					PASS_ERROR("\aERROR: char_at: index `" << i << "` out of bounds\n");
				}
				mainStack.pop();

				char chr[2];

				// negative index starts from back
				chr[0] = tmp[i >= 0 ? i : strlen(tmp) + i];
				chr[1] = '\0';
				mainStack.push(chr);

			} else if (mainStack.top().type == CalcValue::NUM) {

				int i = mainStack.top().number;
				mainStack.pop();
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);

				if (mainStack.top().type != CalcValue::STR) {
					PASS_ERROR("\aERROR: char_at expected a string and a numerical index\n");
				}

				char chr[2];
				if (abs(i) > strlen(mainStack.top().string) || i == strlen(mainStack.top().string)) {
					PASS_ERROR("\aERROR: char_at: index `" << i << "` out of bounds\n");
				}
				chr[0] = mainStack.top().string[i >= 0 ? i : strlen(mainStack.top().string) + i];
				mainStack.pop();
				mainStack.push(chr);

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
				if (abs(i) > strlen(tmp) || i == strlen(tmp)) {
					PASS_ERROR("\aERROR: del_char: index `" << i << "` out of bounds\n");
				}

				// delete char at index
				deleteChar(i >= 0 ? tmp + i  : tmp + strlen(tmp) + i);
				mainStack.push(tmp);


			} else if (mainStack.top().type == CalcValue::NUM) {
				// get index
				int i = (int) mainStack.top().number;
				mainStack.pop();

				// check range of index
				if (abs(i) > strlen(mainStack.top().string) || i == strlen(mainStack.top().string)) {
					PASS_ERROR("\aERROR: del_char: index `" << i << "` out of bounds\n");
				}

				// verify string
				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);
				if (mainStack.top().type != CalcValue::STR) {
					PASS_ERROR("\aERROR: del_char expected a string and a numerical index");
				}

				char* str = mainStack.top().string;
				deleteChar(i >= 0 ? i + str : str + strlen(str) + i);

			}

			// line-comments
		} else if (*p == '#')
			break; // ignore rest of line

			// pi
		else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI); // defined in math.h
			// e
		else if (*p == 'e' && *(p + 1) == '\0')
			mainStack.push(M_E); // defined in math.h
			// literals
		else if (strcmp(p, "null") == 0)
			mainStack.push(CalcValue());
		else if (strcmp(p, "true") == 0)
			mainStack.push(1.0);
		else if (strcmp(p, "false") == 0)
			mainStack.push(0.0);

		else if (strcmp(p, "range") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			int end = (int) mainStack.top().getNum();
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: range expected 2 numbers, a start and end\n");
			}
			int start = (int) mainStack.top().getNum();
			mainStack.pop();

			if (start > end)
				for (; start > end; start--)
					mainStack.push(start);
			else if (start < end)
				for (; start < end; start++)
					mainStack.push(start);
			else mainStack.push(start);


			// print to terminal
		} else if (strcmp(p, "print") == 0) {
			ASSERT_NOT_EMPTY(p);

			CONVERT_INDEX(mainStack, var_nodes);
			if (printCalcValueRAW(mainStack.top(), var_nodes))
				return p;


			mainStack.pop();

			// print and end with a newline
		} else if (strcmp(p, "println") == 0) {
			CONVERT_INDEX(mainStack, var_nodes);
			ASSERT_NOT_EMPTY(p);

			CONVERT_INDEX(mainStack, var_nodes);
			if (printCalcValueRAW(mainStack.top(), var_nodes))
				return p;

			mainStack.pop();

			std::cout <<std::endl;


		} else if (strcmp(p, "print_blk") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::BLK) {
				PASS_ERROR("\aERROR: print_blk expected a macro.");
			}
			size_t len = 50;
			char* str = (char*) malloc(len);
			mainStack.top().block->toString(&str, &len);
			printf("len= %lu \n \"%s\"", len, str);
			free(str);

			// prints in color
		} else if (strcmp(p, "color_print") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: not enough data for function `print_color`. (takes 2 arguments)\n\n");
			}

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue val = getNextValue(mainStack);
			if (!val.isStr()) {
				PASS_ERROR("\aERROR: print_color expected a string containing a valid HTML color\n\n");
			}

			setFgColor(val.string);

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue msg = getNextValue(mainStack);
			if (printCalcValueRAW(msg, var_nodes)) {
				setFgColor();
				return p;
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
			size_t lineLen = 256;

			if (getline(&input, &lineLen, stdin) == -1) {
				PASS_ERROR("\aERROR: input could not getline()\n");
			}

			if (input[strlen(input) - 1] == '\n')
				input[strlen(input) - 1] = '\0';

			mainStack.push(input);

			free(input);

			// get a single character from stdin
		} else if (strcmp(p, "getchar") == 0) {
			char input[2] = { (char) getc(stdin), '\0' };
			mainStack.push(input);

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


			// load the contents of a string to a file
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
				CalcValue* tmp = mainStack.top().valAtRef(var_nodes);

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

				CalcValue* list;

				if (mainStack.top().type == CalcValue::REF) {
					CalcValue* tmp = mainStack.top().valAtRef(var_nodes);
					if (!tmp) {
						PASS_ERROR("\aERROR: $"<<mainStack.top().string <<" undefined or inaccessable.\n");
					}
					list = tmp->getListElem(index);
					mainStack.pop();

				} else if (mainStack.top().type == CalcValue::ARR)
					list = mainStack.top().getListElem(index);

				if (!list) {
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
		// this puts into a macro
		} else if (strcmp(p, "stk") == 0) { // TODO: change me

			std::stack<CalcValue> tmpStack;

			while (!mainStack.empty()) {
				tmpStack.push(mainStack.top());
				mainStack.pop();
			}

			StrStack block;
			while (!tmpStack.empty()) {
				switch (tmpStack.top().type) {
					case CalcValue::STR:
						if (tmpStack.top().string) {
							// escape double-quotes
							char *tmp = str_replace(tmpStack.top().string, "\"", "\\\"");

							// allocate enough space for the string
							size_t len = strlen(tmp);
							char str[len + 4 + 1];

							// put string in quotes and end line
							str[0] = '"';
							strcpy(str + 1, tmp);
							str[len + 1] = '"';
							str[len + 2] = '\n';
							str[len + 3] = 0;
							block.push(str);

							// str_replace has blind allocation
							free(tmp);
						}
						break;

					case CalcValue::BLK:
						for (ssize_t i = tmpStack.top().block->stackDepth - 1; i >= 0; i--) {
							block.push(tmpStack.top().block->at(i));
						}
						break;

					case CalcValue::NUM:
						char str[40];
						snprintf(str, 39, "%*.*g\n", 18, 18, tmpStack.top().getNum());
						block.push(str);
						break;

					case CalcValue::REF:
						break;

				}

				tmpStack.pop();
			}

			mainStack.push(block);

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

		} else if (strcmp(p, "list_size") == 0) {
			auto tmp = mainStack.top().list->size();
			mainStack.pop();
			mainStack.push((double) tmp);

			// initialize a list
		} else if (*p == '(') {

			char *newLine = NULL, *p_cpy = ++p;
			printf("p=\"%s\"\n",p);
			while (*p_cpy) {
				p_cpy++;
			}
			if (lineLen - (p_cpy - pInit) > 2) {
				*p_cpy = ' ';
			}
			printf("p_cpy=\"%s\"\n",p_cpy);

			/*
			if (lineLen - (p - pInit) > 1) { // ( more code....
				if (*++p == '\0')
					p++;
				//while (*p) {
				//	p++;
				//}
				//if (lineLen - (p - pInit) > 1) { // (code, more code
				//	p++;
				//}
				newLine = NULL;

			} else { // (\n
				newLine = (char*) malloc(256);
				size_t lineLen = 256;

				if (getline(&newLine, &lineLen, codeFeed) == -1) {
					PASS_ERROR("\aERROR: `(` invalid list, possible missing `)`\n");
				} else
					line++;

				p = newLine;

			}*/

			std::string listBody = list::getList(p, codeFeed);
			if (listBody == "(") {
				PASS_ERROR("\aERROR: `(` invalid list, possible missing `)`\n");
			}

			std::vector<std::string> elems = list::split(listBody);

			std::stack<CalcValue> tmpStack;
			std::vector<CalcValue> tmp;
			std::vector<CalcValue> arr;
			char *str, *str_head;
			for (std::string elem : elems) {
				str_head = str = new char[elem.size() + 1];
				strcpy(str, elem.c_str());
				char *err = processLine(tmpStack, var_nodes, showErrors, str, elseStatement, codeFeed);
				if (err) {
					PASS_ERROR("\aERROR in block near `" << err << "`. in list:\n");
				}
				arr.push_back(tmpStack.empty() ? CalcValue() : tmpStack.top());
				emptyStack(tmpStack);
				delete[] str_head;
			}

			mainStack.push(arr);

			free(newLine);

			//printf("lst_p = \"%s\"\n", p);
			if (p && *p && strlen(p)) {
				if (!strlen(p)) {
					break;
				}

				//continue;
			}

		// initialize a strStack
		} else if (*p == '{') {

			char* newLine = NULL, * tmp = ++p;

			while (*tmp) {
				tmp++;
			}
			if (lineLen - (tmp - pInit) > 2) {
				*tmp = ' ';
			}
			//printf("tmp=\"%s\"\n", tmp);
			//printf("p=\"%s\"\n", p);

			/*if (lineLen - (p - pInit) > 2) { // { more code....
				char* tmp = p;

				while (*tmp)
					tmp++;

				if (lineLen > tmp - pInit) {
					if (*tmp)
						printf("tmp=\"%s\"\n", tmp);
					else
						*tmp = ' ';
				}

				newLine = NULL;

			} else { // {\n
				newLine = (char*) malloc(256);
				size_t lineLen = 256;

				if (getline(&newLine, &lineLen, codeFeed) == -1) {
					PASS_ERROR("\aERROR: `{` could not getline(). Possible missing `}`\n");
				} else
					line++;

				p = newLine;

			}*/
			StrStack* execArr = strstk::getStrStack(p, codeFeed);

			//free's mem allocated for line
			free(newLine);

			if (execArr) {
				mainStack.push(CalcValue(*execArr));
				delete execArr;
			} else {
				PASS_ERROR("\aERROR: `{` could not getline(). Possible missing `}`\n");
				strcpy(pInit, p);
				free(p);
				return pInit;
			}

			rpnln = p; // this is a memory leak..
			//free(p);
		} else if (*p == '}') {
			printCalcValueRAW(mainStack.top(), var_nodes);
			PASS_ERROR("\aERROR: `}` without previous `{`\n\n");


			// eval operator
		} else if ((*p == '@' && *(p + 1) == '\0') || strcmp(p, "eval") == 0) {
			ASSERT_NOT_EMPTY(p);

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			CalcValue top = CalcValue(mainStack.top());
			mainStack.pop();

			if (top.type == CalcValue::BLK) {
				RUN_STR_STK(*top.block, mainStack);

			} else if (top.type == CalcValue::STR) {
				char* err = processLine(mainStack, var_nodes, showErrors, top.string, elseStatement, codeFeed);
				if (err) {
					PASS_ERROR("\aERROR in block near `" <<err <<"`. Called here:\n");
				}
			} else {
				PASS_ERROR("\aERROR: @ (execution operator) only accepts strings and executable arrays\n");
			}

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
			newElseClause.push("{");

			bool condition = mainStack.top().getNum() != 0;
			mainStack.pop();

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (elseStatement) {
				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs anonymous subroutines to function");
				}
				StrStack elseifBlock = *mainStack.top().block;
				mainStack.pop();

				CONVERT_INDEX(mainStack, var_nodes);
				CONVERT_REFS(mainStack, var_nodes);

				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs anonymous subroutines to function");
				}
				StrStack elseBlock = *mainStack.top().block;
				mainStack.pop();

				StrStack::appendToStack(newElseClause, elseBlock);
				newElseClause.push("} else {");
				StrStack::appendToStack(newElseClause, elseifBlock);
				if (condition) {
					newElseClause.push("} true if");
				} else {
					newElseClause.push("} false if ");
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

			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);
			if (mainStack.top().type != CalcValue::BLK) {
				PASS_ERROR("\aERROR: while: condition must be a block/subroutine\n");
			}

			StrStack condBlock = *mainStack.top().block;
			mainStack.pop();

			std::stack<CalcValue> condStack;
			CalcValue top = mainStack.top();

			// main-loop
			for (;;) {
				RUN_STR_STK(condBlock, condStack);
				if (!condStack.top().getNum())
					break;

				// run process
				if (top.type == CalcValue::BLK) {
					RUN_STR_STK(*top.block, mainStack);
				} else
					mainStack.push(top);
			}

		} else if (strcmp(p, "for-each") == 0) {
			// we have at least 3 things... the first is a reference
			if (mainStack.size() < 3 || mainStack.top().type != CalcValue::REF) {
				PASS_ERROR("\aERROR: malformed for-each statement \n correct format: `{ process } { list } $var for-each`\n" <<std::endl);
			}

			// if it hasn't been assigned yet..
			/*UserVar* var = vars::findVar(var_nodes, mainStack.top().string); // container variable
			if (var == NULL) {
				var = new UserVar(first_node, mainStack.top().string, CalcValue());
				vars::lastVar(first_node)->next = var;
			}*/

			vars::assignVar(&var_nodes[var_nodes.size() - 1], mainStack.top().string, CalcValue());
			UserVar* var = vars::findVar(var_nodes, mainStack.top().string);

			// get list
			mainStack.pop();
			CONVERT_INDEX(mainStack, var_nodes);
			CONVERT_REFS(mainStack, var_nodes);

			if (mainStack.top().type != CalcValue::BLK && mainStack.top().type != CalcValue::ARR) {
				PASS_ERROR("\aERROR: malformed for-each statement \n correct format: `{ process } $list $var for-each`\n" <<std::endl);
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
					PASS_ERROR("\aERROR: malformed for-each statement \n correct format: `{ process } { list } $var for-each`\n");
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
					PASS_ERROR("\aERROR: malformed for-each statement \n correct format: `{ process } (list) $var for-each`\n");
				}

				StrStack process(*mainStack.top().block);
				mainStack.pop();

				for (CalcValue& cv : vec) {
					var->val = cv;
					RUN_STR_STK(process, mainStack);
				}

			}
		// exit the program
		} else if ((*p == 'q' && *(p + 1) == '\0')
		           || !strcmp(p, "exit") || !strcmp(p, "quit")
				)
			exit(EXIT_SUCCESS); // exit the program

		// show help
		else if (strcmp(p, "help") == 0) {
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
						std::cout << "[ARR] @ " << var <<": $" <<var->name <<' ';
						printCalcValueRAW(var->val, var_nodes);
						std::cout <<" =\n";
					}
					var = var->next;
				}

			}

		// print the contents of the stack
		} else if (strcmp(p, "stack") == 0)
			commands::debugStack(mainStack, var_nodes);

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

			CalcValue rhs = getNextValue(mainStack),
					  lhs = getNextValue(mainStack);

			if (lhs.type == CalcValue::REF ) {
				UserVar *var = vars::findVar(var_nodes, lhs.string);

				while (var && var->val.type == CalcValue::REF)
					var = vars::findVar(var_nodes, var->val.string);

				if (var == NULL) {
					var = new UserVar(&var_nodes[var_nodes.size() - 1], lhs.string, rhs);
					var->val.type = rhs.type;
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
					var->val.type = lhs.type;
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
		} else if (strcmp(p, "errors-off") == 0)
			showErrors = false;
		else if (strcmp(p, "errors-on") == 0)
			showErrors = true;

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

			// retarded users...
		} else if (*p == '\'') {
			PASS_ERROR("\aERROR: strings are enclosed in double-quotes (\")\n");

			// user has given a string :D
		} else if (*p == '\"')
			mainStack.push(p + 1);

			// reference
		else if (*p == '$' && *(p + 1) != '\0') // user must use '$' prefix to access the variables
			mainStack.push(CalcValue().setRef(p + 1)); // beautiful hack, eh?
			// let's try and figure out what this could be...
		else {
			// parse input
			double number = atof(p);

			// the user is an asshole :T
			if (number == 0 && *p != '0' && (*p != '-' && *(p + 1) != '0')) {
				PASS_ERROR("\aSYNTAX ERROR: near `" <<p <<"`" <<std::endl);

				// the user has given us a number :D
			} else
				mainStack.push(number);
		}

		// get next token
		p = qtok(rpnln, &rpnln);

	}


	return NULL;

}


#endif