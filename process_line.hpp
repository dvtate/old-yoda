#ifndef PROCESS_LINE_H
#define PROCESS_LINE_H

#include <iostream>
#include <stack>
#include <queue>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>

// this is the class used in our stack
#include "calc_value.hpp"

// user defined variables
#include "user_variables.hpp"

// conditional statements
#include "conditionals.hpp"

// some useful functions
#include "utils.hpp"

/// TODO: add a `debug-stack` operator


extern CalcValue ans;


#define ASSERT_NOT_EMPTY(OPERATOR)\
			if (mainStack.empty()) {\
				if (showErrors)\
					std::cerr <<"\aERROR: not enough data for `" <<OPERATOR <<"`.\n";\
				return p;\
			}

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
			|| !strcmp(p, "logBase") || !strcmp(p, "logBASE") || !strcmp(p, "logbase")
			|| !strcmp(p, "pow") // for those who dont like "**"
		) {

			if (mainStack.size() < 2) {
				if (showErrors)
					std::cerr <<"\aERROR: Not enough data to satisfy operator `" <<p <<"`." <<std::endl;
				return p;
			}

			CONVERT_REFS(mainStack, first_node, showErrors);

			if (mainStack.top().type != CalcValue::NUM) {
				if (showErrors)
					std::cerr <<"ERROR: incompatible data-types! (expected two numbers)";
				return p;
			}

			double b = getNextValue(mainStack).getNum();

			CONVERT_REFS(mainStack, first_node, showErrors);

			if (mainStack.top().type != CalcValue::NUM) {
				if (showErrors)
					std::cerr <<"ERROR: incompatible data-types! (expected two numbers) ";
				return p;
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
		  		if (showErrors)
					std::cerr <<"\aERROR: Not enough data to satisfy `+` operator." <<std::endl;
				return p;
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


		// char is a unary operator
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
		} else if (strcmp(p, "ln") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(log(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(sqrt(getNextValue(mainStack).getNum()));
		} else if (strcmp(p, "abs") == 0) {
			ASSERT_NOT_EMPTY(p);
			CONVERT_REFS(mainStack, first_node, showErrors);
			mainStack.push(std::abs(getNextValue(mainStack).getNum()));


		// find length of a string
		} else if (strcmp(p, "strlen") == 0) {
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type == CalcValue::STR)
				mainStack.top().setValue((double)strlen(mainStack.top().string));
			else {
				if (showErrors)
					std::cerr <<"\aERROR: strlen expected a string.\n";

				return p;
			}

		// comments
		} else if (*p == '#')
			break;

		// pi
		else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI); // defined in math.h
		else if (strcmp(p, "null") == 0)
			mainStack.push(NULL_CALCVAL_OBJECT);
		else if (strcmp(p, "true") == 0)
			mainStack.push(1.0);
		else if (strcmp(p, "false") == 0)
			mainStack.push(0.0);

		// previous answer
		else if (strcmp(p, "ans") == 0) // p == "ans"
			mainStack.push(ans);

		else if (strcmp(p, "print") == 0) {
			ASSERT_NOT_EMPTY("print");
			printCalcValueRAW(mainStack.top(), first_node);
			mainStack.pop();

		} else if (strcmp(p, "println") == 0) {
			ASSERT_NOT_EMPTY("println");
			printCalcValueRAW(mainStack.top(), first_node);
			mainStack.pop();

			std::cout <<std::endl;

		// user input
		} else if (strcmp(p, "input") == 0) {
			char* input = (char*) malloc(256);
			size_t lineLen = 256;

			if (getline(&input, &lineLen, stdin) == -1) {
				if (showErrors)
					std::cerr <<"\aERROR: input could not getline()\n";
				return p;
			}

			mainStack.push(input);

		// convert to string
		} else if (strcmp(p, "str") == 0) {
			ASSERT_NOT_EMPTY("str");
			CONVERT_REFS(mainStack, first_node, showErrors);
			CalcValue val = getNextValue(mainStack);

		  	if (val.type == CalcValue::STR)
				mainStack.push(val.getStr());
			else if (val.type == CalcValue::NUM) {
				char str[8];
				snprintf(str, 7, "%g", val.getNum());
				mainStack.push(str);
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
			conditional(p + 3, mainStack, first_node, showErrors);

		// should never see this as it is handled by the conditional function
		} else if (strcmp(p, ":?") == 0) {
			if (showErrors)
				std::cerr <<"\aERROR: `:?` without previous `?:`\n" <<std::endl;
			return p;

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
					std::cout <<"[NUM] @ " <<var <<": $"<<var->name <<' ' <<var->val.getNum() <<" = \n";
				else if (var->val.type == CalcValue::STR)
					std::cout <<"[STR] @ " <<var <<": $"<<var->name <<" \"" <<var->val.getStr() <<"\" = \n";
				else if (var->val.type == CalcValue::REF)
					std::cout <<"[REF] @ " <<var <<": $"<<var->name <<" $" <<var->val.getRef() <<" = \n";

				var = var->next;

			}


		// typeof function
		} else if (strcmp(p, "typeof") == 0) {
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

			if (mainStack.top().type == CalcValue::STR)
				system(mainStack.top().getStr()); // gets run in BASH/CMD

			else if (mainStack.top().type == CalcValue::NUM) {
				if (showErrors)
					std::cerr <<"\aERROR: cannot make a system call with a number...\n" <<std::endl;
				return p;

			}
			mainStack.pop();


		// assignment operator
		} else if (*p == '=' && *(p + 1) == '\0') { // variable assignment

			if (mainStack.size() < 2) {
				if (showErrors)
					std::cerr <<"\aERROR: not enough data for assignment. (takes 2 arguments)\n" <<std::endl;
				return p;

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
					if (showErrors)
						std::cerr <<"\aERROR: inappropriate use of assignment operator. (no variable given)\n" <<std::endl;
					return p;
				}

			}


		// variable
		} else if (*p == '$' && *(p + 1) != '\0') // user must use '$' prefix to access the variables

			mainStack.push(CalcValue().setRef(p + 1));

		else if (*p == '~' && *(p + 1) == '\0') {
			if (mainStack.empty()){
				if (showErrors)
					std::cerr <<"\aERROR: not enough data for copy operator (`~`)\n" <<std::endl;
				return p;
			}

			CONVERT_REFS(mainStack, first_node, showErrors);

		// error reporting can get annoying on final programs
		} else if (strcmp(p, "errors-off") == 0)
			showErrors = false;
		else if (strcmp(p, "errors-on") == 0)
			showErrors = true;


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

		// duplicate the top of the stack
		} else if (strcmp(p, "dup") == 0) {
			ASSERT_NOT_EMPTY("dup");
			mainStack.push(mainStack.top());

		} else if (strcmp(p, "dupx") == 0 || strcmp(p, "dupn") == 0) {
		  	if (mainStack.size() < 2) {
		  		if (showErrors)
					std::cerr <<"\aERROR: Not enough data to satisfy `" <<p
							  <<"` operator." <<std::endl;
				return p;
			}
			CONVERT_REFS(mainStack, first_node, showErrors);
			if (mainStack.top().type != CalcValue::NUM) {
				if (showErrors)
					std::cerr <<"\aERROR: operator `" <<p <<"` expected a number";
				return p;
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
				if (showErrors)
					std::cerr <<"\aSYNTAX ERROR: near `" <<p <<"`" <<std::endl;
				return p;

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
