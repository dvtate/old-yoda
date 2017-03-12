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

extern CalcValue ans;


#define ASSERT_NOT_EMPTY(OPERATOR)\
			if (mainStack.empty()) {\
				if (showErrors)\
					std::cerr <<"\aERROR: not enough data for `" <<OPERATOR <<"`.\n";\
				return p;\
			}

#define PASS_ERROR(MSG)\
	if (showErrors)	\
		std::cerr <<MSG;\
	return p;



#define CONVERT_REFS(MAINSTACK, FIRST_NODE, SHOW_ERRORS)\
	if (MAINSTACK.top().type == CalcValue::REF) {\
		CalcValue* val = MAINSTACK.top().valAtRef(FIRST_NODE);\
\
		while (val && val->type == CalcValue::REF)\
			val = valAtRef(*val, FIRST_NODE);\
\
		if (val != NULL)\
			MAINSTACK.top().setValue(*val);\
		else {\
			if (SHOW_ERRORS)\
				std::cerr <<"\aERROR: broken reference to $" <<MAINSTACK.top().string <<'\n';\
			return p;\
		}\
	}


extern bool runStringStack(
	StrStack& code, bool& errorReporting, std::stack<CalcValue>& mainStack,
	UserVar* first_node
);
extern bool runFile(FILE* prog_file, UserVar* first_node, bool& errorReporting,
	      std::stack<CalcValue>& mainStack, bool& elseStatement
);


char* processLine(std::stack<CalcValue>& mainStack, UserVar* first_node,
	bool& showErrors, char*& rpnln, bool& elseStatement
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

			CONVERT_REFS(mainStack, first_node, showErrors);

			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operatotr `" <<p <<"`. (expected two numbers)\n");
			}

			double b = getNextValue(mainStack).getNum();

			CONVERT_REFS(mainStack, first_node, showErrors);

			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: incompatible data-types for operatotr `" <<p <<"`. (expected two numbers)\n");
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
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue b = getNextValue(mainStack);

			CONVERT_REFS(mainStack, first_node, showErrors);
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
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(sin(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "cos") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(cos(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "tan") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(tan(getNextValue(mainStack).getNum()));

		// inverse trig functions
		} else if (strcmp(p, "asin") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(asin(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "acos") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(acos(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "atan") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(atan(getNextValue(mainStack).getNum()));

		// hyperbolic trig functions
		} else if (strcmp(p, "sinh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(sinh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "cosh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(cosh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "tanh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(tanh(getNextValue(mainStack).getNum()));


		// inverse hyperbolic trig functions
		} else if (strcmp(p, "asinh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(asinh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "acosh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(acosh(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "atanh") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(atanh(getNextValue(mainStack).getNum()));

		// more unary math functions
		} else if (strcmp(p, "log") == 0 || strcmp(p, "log10") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(log10(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "ln") == 0) { // natural log
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(log(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0) { // square root
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(sqrt(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "abs") == 0) { // absolute value
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(std::abs(getNextValue(mainStack).getNum()));

		// find length of a string
		} else if (strcmp(p, "strlen") == 0) {
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type == CalcValue::STR)
				mainStack.top().setValue((double)strlen(mainStack.top().string));
			else {
				PASS_ERROR("\aERROR: strlen expected a string.\n");
			}

		// find first occurance of substring in a string (strstr())
		} else if (strcmp(p, "strstr") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: stristr takes 2 strings, a haystack and a needle (strings)\n");
			}
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}

			char needle[strlen(mainStack.top().string)];
			strcpy(needle, mainStack.top().string);
			mainStack.pop();

			char haystack[strlen(mainStack.top().string)];
			strcpy(haystack, mainStack.top().string);
			mainStack.pop();

			mainStack.push(strstr(haystack, needle));

		// case-insensitive strstr
		} else if (strcmp(p, "stristr") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: stristr takes 2 strings, a haystack and a needle (strings)\n");
			}
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}

			char needle[strlen(mainStack.top().string)];
			strcpy(needle, mainStack.top().string);
			mainStack.pop();

			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: stristr expected 2 strings, a haystack and a needle to find\n");
			}

			char haystack[strlen(mainStack.top().string)];
			strcpy(haystack, mainStack.top().string);
			mainStack.pop();

			mainStack.push(strstr(haystack, needle));

		} else if (strcmp(p, "trim") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::STR) {
				PASS_ERROR("\aERROR: trim expected a string to trim whitespace off of\n");
			}

			char str[strlen(mainStack.top().string)];
			strcpy(str, mainStack.top().string);
			mainStack.pop();
			mainStack.push(trimStr(str));

		// line-comments
		} else if (*p == '#')
			break;

		// pi
		else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI); // defined in math.h
		// e
		else if (*p == 'e' && *(p + 1) == '\0')
			mainStack.push(M_E); // defined in math.h
		// literals
		else if (strcmp(p, "null") == 0) // this segfaults................
			mainStack.push(NULL_CALCVAL_OBJECT); // (char*) NULL
		else if (strcmp(p, "true") == 0)
			mainStack.push(1.0);
		else if (strcmp(p, "false") == 0)
			mainStack.push(0.0);

		// previous answer
		else if (strcmp(p, "ans") == 0) // p == "ans"
			mainStack.push(ans);

		// print to terminal
		else if (strcmp(p, "print") == 0) {
			ASSERT_NOT_EMPTY("print");

			if (printCalcValueRAW(mainStack.top(), first_node))
				return p;

			mainStack.pop();

		// print and end with a newline
		} else if (strcmp(p, "println") == 0) {
			ASSERT_NOT_EMPTY("println");

			if (printCalcValueRAW(mainStack.top(), first_node))
				return p;
			mainStack.pop();

			std::cout <<std::endl;


		} else if (strcmp(p, "printblk") == 0) {
			size_t len = 50;
			char* str = (char*) malloc(len);
			mainStack.top().block->toString(&str, &len);
			printf("len= %lu \"%s\"", len, str);
			free(str);

		// prints in color
		} else if (strcmp(p, "color_print") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: not enough data for function `print_color`. (takes 2 arguments)\n\n");
			}
			CalcValue val = getNextValue(mainStack);
			if (!val.isStr()) {
				PASS_ERROR("\aERROR: print_color expected a string containing a valid HTML color\n\n");
			}

			setFgColor(val.string);

			CalcValue msg = getNextValue(mainStack);
			if (printCalcValueRAW(msg, first_node)) {
				setFgColor();
				return p;
			}

			setFgColor();

		// changes the terminal background color for text
		} else if (strcmp(p, "setBgColor") == 0) {
			ASSERT_NOT_EMPTY("setBgColor");
			if (!mainStack.top().isStr()) {
				mainStack.pop();
				PASS_ERROR("\aERROR: setBgColor expected a string containing a valid HTML color.\n\n");
			}

			setBgColor(mainStack.top().string);
			mainStack.pop();

		// changes the terminal foreground color for text
		} else if (strcmp(p, "setFgColor") == 0) {
			ASSERT_NOT_EMPTY("setFgColor");
			if (!mainStack.top().isStr()) {
				mainStack.pop();
				PASS_ERROR("\aERROR: setFgColor expected a string containing a valid HTML color.\n\n");
			}

			setFgColor(mainStack.top().string);
			mainStack.pop();

		} else if (strcmp(p, "reset_color") == 0) {
			resetANSI();

		} else if (strcmp(p, "resetFgColor") == 0)
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

			ASSERT_NOT_EMPTY("file_get_contents");


			CONVERT_REFS(mainStack, first_node, showErrors);

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
				PASS_ERROR("\aERROR: not enough data for function `file_put_contents`. (takes 2 strings)\n\n");
			}

			CONVERT_REFS(mainStack, first_node, showErrors);

			// takes a string for the filename
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the file name.\n\n");
			}

			FILE* output_file = fopen(mainStack.top().string, "w");
			mainStack.pop();

			CONVERT_REFS(mainStack, first_node, showErrors);

			// takes a string for the contents
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the contents to write.\n\n");
			}

   			fwrite(mainStack.top().string, 1, strlen(mainStack.top().string) + 1, output_file);
			mainStack.pop();
			fclose(output_file);


		// convert to string
		} else if (strcmp(p, "str") == 0) {
			ASSERT_NOT_EMPTY("str");
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue val = getNextValue(mainStack);

		  	if (val.type == CalcValue::STR)
				mainStack.push(val.getStr());
			else if (val.type == CalcValue::NUM) {
				char str[26];
				snprintf(str, 26, "%*.*g", 10, 16, val.getNum());
				mainStack.push(trimStr(str));

				// c++ solutions don't always work as desired :P
				//mainStack.push(std::to_string(val.getNum()).c_str());
			}

		// convert to number
		} else if (strcmp(p, "num") == 0) {
			ASSERT_NOT_EMPTY("num");
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
			 	mainStack.push(0.0);
		  	else if (val.type == CalcValue::NUM)
				mainStack.push(val.getNum());
			else if (val.type == CalcValue::STR)
				mainStack.push(atof(val.getStr()));

		// convert to an integer
		} else if (strcmp(p, "int") == 0) {
			ASSERT_NOT_EMPTY("int");
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
			 	mainStack.push(0.0);
		  	else if (val.type == CalcValue::NUM)
				mainStack.push(round(val.getNum()));
			else if (val.type == CalcValue::STR)
				mainStack.push( round( atof( val.getStr() ) ) );

		} else if (strcmp(p, "floor") == 0) {
			ASSERT_NOT_EMPTY("floor");
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue val = getNextValue(mainStack);

			if (val.isEmpty())
			 	mainStack.push(0.0);
		  	else if (val.type == CalcValue::NUM)
				mainStack.push(floor(val.getNum()));
			else if (val.type == CalcValue::STR)
				mainStack.push(atoi(val.getStr()));

		// initialize a strStack
		} else if (*p == '{') {

			char* newLine;

			if (lineLen - (p - pInit) > 1) { // { more code....
				if (*++p == '\0')
					p++;
				newLine = NULL;

			} else { // {\n
				newLine = (char*) malloc(256);
				size_t lineLen = 256;

				if (getline(&newLine, &lineLen, stdin) == -1) {
					PASS_ERROR("\aERROR: `{` could not getline(). Possible missing `}`\n");
				} else
					line++;

				p = newLine;

			}
			StrStack* execArr = strstk::getStrStack(p);

			//free's mem allocated for line
			free(newLine);

			if (execArr) {
				mainStack.push(*execArr);
				delete execArr;
			} else {
				PASS_ERROR("\aERROR: `{` could not getline(). Possible missing `}`\n");
				return p;

			}

			rpnln = p;

		} else if (*p == '}') {
			std::cout <<"p =" <<p <<std::endl;
			PASS_ERROR("\aERROR: `}` without previous `{`\n\n");


		// eval operator
		} else if ((*p == '@' && *(p + 1) == '\0') || strcmp(p, "eval") == 0) {
			ASSERT_NOT_EMPTY(p);

			CONVERT_REFS(mainStack, first_node, showErrors);

			CalcValue top = CalcValue(mainStack.top());

			if (top.type == CalcValue::BLK) {
				/* I did this at 5am but there might be value here...
				// put the statement in a string
				size_t buff_size = 500;
				char* buff = (char*) malloc(buff_size);
				mainStack.top().block->toString(&buff, &buff_size);

				// put the string in a temp file
				FILE* statement = tmpfile();
				fputs(buff, statement);

				rewind(statement);
				// run the file
				if (runFile(statement, first_node, showErrors, mainStack, elseStatement)) {
					PASS_ERROR("\aERROR: @ (exec operator) failed");
				}
				*/
				if (runStringStack(*top.block, showErrors, mainStack, first_node)) {
					PASS_ERROR("\aERROR in bock/subroutine called here\n");
				}
			} else if (top.type == CalcValue::STR) {
				char* err = processLine(mainStack, first_node, showErrors, top.string, elseStatement);
				if (err) {
					PASS_ERROR("\aERROR in block near `" <<err <<"`. Called here:\n");
				}
			} else {
				PASS_ERROR("\aERROR: @ (execution operator) only accepts strings and executable arrays\n");
			}
			mainStack.pop();
		// conditionals::else
		} else if (strcmp(p, "else") == 0) {
			ASSERT_NOT_EMPTY(p);
			//CONVERT_REFS(mainStack, first_node, showErrors);
			elseStatement = true;

		// conditionals::elseif (dysfunctional)
		} else if (strcmp(p, "elseif") == 0) {
			if (mainStack.size() < 2 || (elseStatement && mainStack.size() < 3)) {
				PASS_ERROR("\aERROR: elseif expected a condition and a block of code (takes 2 arguments)\n" <<std::endl);
			}

			StrStack newElseClause;
			newElseClause.push("{");

			bool condition = mainStack.top().getNum() != 0;
			mainStack.pop();

			CONVERT_REFS(mainStack, first_node, showErrors);

			if (elseStatement) {
				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs anonymous subroutines to function");
				}
				StrStack elseifBlock = *mainStack.top().block;
				mainStack.pop();

				CONVERT_REFS(mainStack, first_node, showErrors);

				if (mainStack.top().type != CalcValue::BLK) {
					PASS_ERROR("\aERROR: `else` needs anonymous subroutines to function");
				}
				StrStack elseBlock = *mainStack.top().block;
				mainStack.pop();

				strstk::appendToStack(newElseClause, elseBlock);
				newElseClause.push("} else {");
				strstk::appendToStack(newElseClause, elseifBlock);
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
						if (runStringStack(*runTrue.block, showErrors, mainStack, first_node)) {
							PASS_ERROR("\aERROR in bock/subroutine called here\n");
						}
					} else
						mainStack.push(runTrue);

				} else {
					mainStack.pop();
					if (mainStack.top().type == CalcValue::BLK) {
						CalcValue top = mainStack.top();
						mainStack.pop();
						if (top.type == CalcValue::BLK) {
							if (runStringStack(*top.block, showErrors, mainStack, first_node)) {
								PASS_ERROR("\aERROR in bock/subroutine called here\n");
							}
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
							if (runStringStack(*top.block, showErrors, mainStack, first_node)) {
								PASS_ERROR("\aERROR in bock/subroutine called here\n");
							}
						} else
							mainStack.push(top);

					} // else, it's a value that should stay at the top of the stack
				} // else, don't do anything as there isn't an else clause

		// runs the same block of code a given number of times
		} else if (strcmp(p, "repeat") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: repeat loop needs a number of times to execute and a block.\n");
			}

			CONVERT_REFS(mainStack, first_node, showErrors);
			size_t timesToRepeat = abs(mainStack.top().getNum());
			mainStack.pop();

			CONVERT_REFS(mainStack, first_node, showErrors);
			StrStack block;
			if (mainStack.top().type == CalcValue::BLK)
				block = (*mainStack.top().block);
			else {
				PASS_ERROR("\aERROR: repeat loop expected a block of code to execute.\n")
			}
			mainStack.pop();

			for (; timesToRepeat > 0; timesToRepeat--)
				runStringStack(block, showErrors, mainStack, first_node);

		// while loop
		} else if (strcmp(p, "while") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: while loop needs two blocks - a condition and a process.\n");
			}

			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::BLK) {
				PASS_ERROR("\aERROR: while: condition must be a block/subroutine\n");
			}

			StrStack condBlock = *mainStack.top().block;
			mainStack.pop();

			std::stack<CalcValue> condStack;
			CalcValue top = mainStack.top();

			// main-loop
			for (;;) {

				// check condition
				runStringStack(condBlock, showErrors, condStack, first_node);
				if (!condStack.top().getNum())
					break;

				// run process
				if (top.type == CalcValue::BLK) {
					if (runStringStack(*top.block, showErrors, mainStack, first_node)) {
						PASS_ERROR("\aERROR in bock/subroutine called here\n");
					}
				} else
					mainStack.push(top);

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
			ans = 0.0;
		  	emptyStack(mainStack);
			vars::wipeAll(first_node);

		// useful for debugging
		} else if (strcmp(p, "vars") == 0 || strcmp(p, "ls_vars") == 0) {
			UserVar* var = first_node->next;

			while (var != NULL) {
				if (var->val.type == CalcValue::NUM)
					std::cout <<"[NUM] @ " <<var <<": $" <<var->name <<' '
							  <<var->val.getNum() <<" =\n";

				else if (var->val.type == CalcValue::STR && var->val.isNull())
					std::cout <<"[NIL] @ " <<var <<": $" <<var->name <<' '
							  <<"null =\n";

				else if (var->val.type == CalcValue::STR)
					std::cout <<"[STR] @ " <<var <<": $" <<var->name <<" \""
							  <<var->val.getStr() <<"\" =\n";

				else if (var->val.type == CalcValue::REF)
					std::cout <<"[REF] @ " <<var <<": $" <<var->name <<" $"
							  <<var->val.getRef() <<" =\n";

				else if (var->val.type == CalcValue::BLK)
					std::cout <<"[BLK] @ " <<var <<": $" <<var->name <<" has "
							  <<var->val.block->stackDepth
							  <<((var->val.block->stackDepth == 1) ? " line\n" : " lines\n");

				var = var->next;
			}

		// print the contents of the stack
		} else if (strcmp(p, "stack") == 0)
			commands::debugStack(mainStack, first_node);

		// typeof function
		else if (strcmp(p, "typeof") == 0) {
			ASSERT_NOT_EMPTY("typeof");

		 	if (mainStack.top().type == CalcValue::REF) {
				UserVar* var = vars::findVar(first_node, mainStack.top().string);
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

		// system call (problem: this conflicts with the current strategy for handling if statements.....)
		} else if (strcmp(p, "sys") == 0 || strcmp(p, "system") == 0) {

			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);

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
			} else {

				CalcValue rhs = getNextValue(mainStack),
						  lhs = getNextValue(mainStack);

				if (lhs.type == CalcValue::REF) {
					UserVar* var = vars::findVar(first_node, lhs.string);

					while (var && var->val.type == CalcValue::REF)
						var = vars::findVar(first_node, var->val.string);

					if (var == NULL) {
						var = new UserVar(first_node, lhs.string, rhs);
						var->val.type = rhs.type;
						vars::lastVar(first_node)->next = var;

					// changing the variable's value
					} else
						var->setValue(rhs);


				} else if (rhs.type == CalcValue::REF) {
					UserVar* var = vars::findVar(first_node, rhs.string);

					while (var && var->val.type == CalcValue::REF)
						var = vars::findVar(first_node, var->val.string);

					if (var == NULL) {
						var = new UserVar(first_node, rhs.string, lhs);
						var->val.type = lhs.type;
						vars::lastVar(first_node)->next = var;

					// changing the variable's value
					} else
						var->setValue(lhs);

				// nothing that can hold data
				} else {
					PASS_ERROR("\aERROR: inappropriate use of assignment operator. (no variable given)\n" <<std::endl);
				}

			}


		// variable
		} else if (*p == '$' && *(p + 1) != '\0') // user must use '$' prefix to access the variables

			mainStack.push(CalcValue().setRef(p + 1)); // beautiful hack, eh?

		else if (*p == '~' && *(p + 1) == '\0') {
			if (mainStack.empty()){
				PASS_ERROR("\aERROR: not enough data for copy operator (`~`)\n");
			}

			CONVERT_REFS(mainStack, first_node, showErrors);

		// error reporting can get annoying on final programs
		} else if (strcmp(p, "errors-off") == 0)
			showErrors = false;
		else if (strcmp(p, "errors-on") == 0)
			showErrors = true;

		else if (strcmp(p, "version") == 0)
			printVersionInfo();

		/* TODO: fix this (currently segfaults...)
		// delete a variable
		else if (strcmp(p, "delete") == 0) {
			//std::cout <<"$a deleted\n" <<std::endl;
			vars::removeVar(first_node, mainStack.top().string);
			//varNames.pop();
		}
		*/

		// clear the stack
		else if  (strcmp(p, "...") == 0)
			emptyStack(mainStack);

		// pop the top of the stack
		else if  (*p == ';' && *(p + 1) == '\0') {
			if (!mainStack.empty())
				mainStack.pop();

		// swap the top 2 elements in the stack
		} else if (strcmp(p, "swap") == 0) {
			if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy `" <<p <<"` operator." <<std::endl);
			}
			// take the top 2 elements from the top
			CalcValue val1 = getNextValue(mainStack);
			CalcValue val2 = getNextValue(mainStack);

			// push them back in reverse order
			mainStack.push(val1);
			mainStack.push(val2);


		// duplicate the top of the stack
		} else if (strcmp(p, "dup") == 0) {
			ASSERT_NOT_EMPTY("dup");
			mainStack.push(mainStack.top());

		// duplicate the top elements a set number of times
		} else if (strcmp(p, "dupx") == 0 || strcmp(p, "dupn") == 0) {
		  	if (mainStack.size() < 2) {
				PASS_ERROR("\aERROR: Not enough data to satisfy `" <<p <<"` operator." <<std::endl);
			}
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::NUM) {
				PASS_ERROR("\aERROR: operator `" <<p <<"` expected a number\n");
			}

			double copies = mainStack.top().getNum();
			mainStack.pop();

			while (copies-- > 1)
				mainStack.push(mainStack.top());

		// retarded users...
		} else if (*p == '\'') {
			PASS_ERROR("\aERROR: strings are enclosed in double-quotes (\")\n");

		// user has given a string :D
		} else if (*p == '\"') {
			mainStack.push(p + 1);
			//printf("string recieved <%s>", p + 1);
		}
		// let's try and figure out what this could be...
		else {
			// parse input
			double number = atof(p);

			// the user is an asshole :T
			if (number == 0 && *p != '0') {
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
