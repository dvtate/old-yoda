#ifndef PROCESS_LINE_H
#define PROCESS_LINE_H

#include <iostream>
#include <stack>
#include <queue>
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

// conditional statements
#include "conditionals.hpp"

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
				if (showErrors)	std::cerr <<MSG;\
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



char* processLine(std::stack<CalcValue>& mainStack, UserVar* first_node,
	bool& showErrors, char*& rpnln
){

	// get first token from the input
	char* p = qtok(rpnln, &rpnln);

	// empty string/whitespace input
	if (p == NULL)
		return p;


	while (p != NULL && *p != '\0') {

		/*
		nextString = NULL;
startCheck:
		if (nextString != NULL)
			p = nextString;
		*/

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
					char str[8];
					snprintf(str, 7, "%g", b.getNum());

					// allocate memory
					char combined[strlen(a.getStr()) + strlen(str) + 1];
					// combine them
					strcpy(combined, a.getStr());
					strcpy(&combined[strlen(a.getStr())], str);

					mainStack.push(combined);

				}

			} else if (a.type == CalcValue::NUM) {
				if (b.type == CalcValue::STR) {

					// convert the double to a string
					char str[8];
					snprintf(str, 7, "%g", a.getNum());

					// allocate memory
					char combined[strlen(str) + strlen(b.getStr()) + 1];
					// combine them
					strcpy(combined, str);
					strcpy(&combined[strlen(str)], b.getStr());

					mainStack.push(combined);

				} else if (b.type == CalcValue::NUM)
					mainStack.push(a.getNum() + b.getNum());

			}

		} else if (strcmp(p, "==") == 0) {
			mainStack.push(getNextValue(mainStack) == getNextValue(mainStack));

		// not equal to
		} else if (strcmp(p, "!=") == 0)
			mainStack.push(!(getNextValue(mainStack) == getNextValue(mainStack)));

		// logical not operator
		else if (*p == '!' && *(p + 1) == '\0')
			mainStack.push(getNextValue(mainStack).getNum() == 0);

		
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

		// comments
		} else if (*p == '#')
			break;

		// pi
		else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI); // defined in math.h
		// e
		else if (*p == 'e' && *(p + 1) == '\0')
			mainStack.push(M_E); // defined in math.h
		//
		else if (strcmp(p, "null") == 0) // this segfaults................
			mainStack.push((char*) NULL);
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

			mainStack.push(input);

			free(input);

		// get a single character from stdin
		} else if (strcmp(p, "getchar") == 0) {
			char input[2] = { (char) getc(stdin), 0 };
			mainStack.push(input);

		// load the contents of a file into a string
		} else if (strcmp(p, "file_get_contents") == 0) {

			ASSERT_NOT_EMPTY("file_get_contents");

			// didn't recieve an string...
			if (!mainStack.top().isStr()) {
				PASS_ERROR("\aERROR: file_get_contents expected a string for the file name.\n\n");
			}

			char *file_contents;
			size_t input_file_size;

			// open the file
			FILE *input_file = fopen(mainStack.top().string, "rb");
			mainStack.pop();

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

		// load the contents of a string to a file
		} else if (strcmp(p, "fileGetContents") == 0) {


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

		// starting conditional
		} else if (strcmp(p, "?:") == 0) {
			p += 3;
			char* err = conditional(p, mainStack, first_node, showErrors);
			if (err)
				return err;

		// should never see this as it is handled by the conditional function
		} else if (strcmp(p, ":?") == 0) {
			/*PASS_ERROR("\aERROR: `:?` without previous `?:` (might be my fault...)\n" <<std::endl);*/
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
		} else if (strcmp(p, "showvars") == 0 || strcmp(p, "vars") == 0 || strcmp(p, "listvars") == 0) {
			UserVar* var = first_node->next;

			while (var != NULL) {
				if (var->val.type == CalcValue::NUM)
					std::cout <<"[NUM] @ " <<var <<": $" <<var->name <<' ' <<var->val.getNum() <<" =\n";
				else if (var->val.type == CalcValue::STR && var->val.isNull())
					std::cout <<"[NIL] @ " <<var <<": $" <<var->name <<' ' <<"null =\n";
				else if (var->val.type == CalcValue::STR)
					std::cout <<"[STR] @ " <<var <<": $" <<var->name <<" \"" <<var->val.getStr() <<"\" =\n";
				else if (var->val.type == CalcValue::REF)
					std::cout <<"[REF] @ " <<var <<": $" <<var->name <<" $" <<var->val.getRef() <<" =\n";

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

			mainStack.push(CalcValue().setRef(p + 1));

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

		/*delete a variable
		else if (strcmp(p, "delete") == 0) {
			std::cout <<"$a deleted\n" <<std::endl;
			vars::removeVar(first_node, varNames.front());
			varNames.pop();
		}

		* Functions haven't been implemented yet
		// user is defining a function
		} else if (strcmp(p, "@function")) {


		// user is calling a function
		} else  if (*p == '@' && *(p + 1) != '\0') {



		// anything else
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

		// user has given a string :D
		} else if (*p == '\"')
			mainStack.push((p + 1));
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

	return (char*) NULL;

}






#endif
