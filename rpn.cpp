#include <iostream>
#include <stack>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>

// this is the class used in our stack
#include "calc_value.h"

// user defined variables
#include "user_variables.h"

#define MAX_LEN 200

// some useful functions
#include "utils.h"


/*This file is about to go through the biggest rewrite I've ever done:
* - main will take in cmdline options for the name of the file to run
* - if none are given then it will go to the interactive shell (currently the only option)
*
*
*
*/

CalcValue ans = 0.0; // here `0` could be a pointer

unsigned int line = 0;

// this will get removed in the rewrite...
uint16_t nestedIf = 0;



int main(){


	std::stack<CalcValue> mainStack;

	if (!nestedIf)
		std::cout <<line++ <<">>> ";



	// I know... this is really lame...
	//char* rpnln = (char*) malloc(MAX_LEN + 1);


	char* rpnln = (char*) malloc(256);
	size_t lineLen = 256;

	if (getline(&rpnln, &lineLen, stdin) == -1) {
		std::cerr <<"\aERROR: Input failed...\n" <<std::endl;
		return main();
	}

	// I need a copy of it to call free() on later.
	char* rpnln_head = rpnln;

	//std::cin.getline(rpnln, MAX_LEN);



	// used for storing the name for user variables
	char* variableName1 = NULL; // will get used later
	char* variableName2 = NULL;

	// get first token from the input
	char* p = qtok(rpnln, &rpnln);

	// empty string/whitespace input
	if (p == NULL)
		return main();

//	char* nextString;

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

			if (mainStack.empty()) {
				std::cerr <<"\aERROR: Not enough data to satisfy operator." <<std::endl;
				return main();
			}
			if (mainStack.top().type != CalcValue::NUM) {
				std::cerr <<"ERROR: incompatible data-types!";
				return main();
			}

			double b = getNextValue(mainStack).getNum();

			if (mainStack.empty()) {
				std::cerr <<"\aERROR: Not enough data to satisfy operator." <<std::endl;
				return main();
			}
			if (mainStack.top().type != CalcValue::NUM) {
				std::cerr <<"ERROR: incompatible data-types!";
				return main();
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

		} else if (*p == '+' ) {

			CalcValue b = getNextValue(mainStack),
					  a = getNextValue(mainStack);

			if (a.type == CalcValue::STR)
				if (b.type == CalcValue::STR) {
					// allocate enough memory for both strings and a null terminator
					char combined[strlen(a.getStr()) + strlen(b.getStr()) + 1];

					// combine the strings
					strcpy(combined, a.getStr());
					strcpy(&combined[strlen(a.getStr())], b.getStr());

					mainStack.push(combined);

				} else { // b is a number

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

			else
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

				} else
					mainStack.push(a.getNum() + b.getNum());

		} else if (strcmp(p, "==") == 0)
			mainStack.push(getNextValue(mainStack) == getNextValue(mainStack));

		// not equal to
		else if (strcmp(p, "!=") == 0)
			mainStack.push(!(getNextValue(mainStack) == getNextValue(mainStack)));

		// logical not operator
		else if (*p == '!' && *(p + 1) == '\0')
			mainStack.push(getNextValue(mainStack).getNum() == 0);


		// char is a unary operator
			//trig functions
		else if (strcmp(p, "sin") == 0)
			mainStack.push(sin(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "cos") == 0)
			mainStack.push(cos(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "tan") == 0)
			mainStack.push(tan(getNextValue(mainStack).getNum()));

		else if (strcmp(p, "asin") == 0)
			mainStack.push(asin(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "acos") == 0)
			mainStack.push(acos(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "atan") == 0)
			mainStack.push(atan(getNextValue(mainStack).getNum()));

		else if (strcmp(p, "sinh") == 0)
			mainStack.push(sinh(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "cosh") == 0)
			mainStack.push(cosh(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "tanh") == 0)
			mainStack.push(tanh(getNextValue(mainStack).getNum()));

		else if (strcmp(p, "asinh") == 0)
			mainStack.push(asinh(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "acosh") == 0)
			mainStack.push(acosh(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "atanh") == 0)
			mainStack.push(atanh(getNextValue(mainStack).getNum()));

		// more unary functions
		else if (strcmp(p, "log") == 0 || strcmp(p, "log10") == 0)
			mainStack.push(log10(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "ln") == 0)
			mainStack.push(log(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0)
			mainStack.push(sqrt(getNextValue(mainStack).getNum()));

		// comments
		else if (*p == '#') {
			if (mainStack.size() == 0)
				return main();
			break;

		// pi
		} else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI);

		// ans
		else if (strcmp(p, "ans") == 0) // p == "ans"
				mainStack.push(ans);

		else if (strcmp(p, "print") == 0) {
			if (mainStack.empty()) {
				if (variableName1) {
					UserVar* var = vars::findVar(vars::first_node, variableName1);
					if (var != NULL) {
						if (var->val.type == CalcValue::NUM)
							std::cout <<var->val.getNum();
						else
							std::cout <<var->val.getStr();
						return main();
					}
				}
				if (variableName2) {
					UserVar* var = vars::findVar(vars::first_node, variableName2);
					if (var != NULL) {
						if (var->val.type == CalcValue::NUM)
							std::cout <<var->val.getNum();
						else
							std::cout <<var->val.getStr();
						return main();
					} else
						std::cout <<std::endl;
				}
				// remove for file-based interpreter...
				std::cout <<std::endl;
			} else {
				CalcValue& val = mainStack.top();
				if (val.type == CalcValue::NUM)
					std::cout <<val.getNum();
				else
					std::cout <<val.getStr();
				mainStack.pop();

				// remove for file-based interpreter...
				std::cout <<std::endl;

			}


		// user input
		} else if (strcmp(p, "input") == 0) {
			char input[255];
			std::cin.getline(input, 255);
			mainStack.push(input);

		// convert to string
		} else if (strcmp(p, "str") == 0) {
			CalcValue val = getNextValue(mainStack);
			if (val.type == CalcValue::STR)
				mainStack.push(val.getStr());
			else {
				char str[8];
				snprintf(str, 7, "%g", val.getNum());
				mainStack.push(str);
			}

		// convert to number
		} else if (strcmp(p, "num") == 0) {
			CalcValue val = getNextValue(mainStack);
			if (val.type == CalcValue::NUM)
				mainStack.push(val.getNum());
			else
				mainStack.push(atof(val.getStr()));

		// convert to an integer
		} else if (strcmp(p, "int") == 0) {
			CalcValue val = getNextValue(mainStack);
			if (val.type == CalcValue::NUM)
				mainStack.push(floor(val.getNum()));
			else
				mainStack.push(atoi(val.getStr()));

		// starting conditional
		} else if (strcmp(p, "?:") == 0) {

			/* REWRITE THIS
			* - if true
			*	+ write contents of the conditional to a StrStack object
			* 	+ run the contents
			* - continue checking for more elseifs on same line as ending :?
			* - else follows same syntax as elseif but without a condition.
			* - else without previous if = multi-line comment (contemplating the security of this...)
			*/

			nestedIf++;

			bool condition = !mainStack.empty() && mainStack.top().getNum();

			//emptyStack(mainStack); // is this really desired?

			if (condition) // change this...
				return main();
			else {
elseif:
				if ((rpnln = strstr(rpnln, ":?")) != NULL)
					goto next_token;

				do {

					if (getline(&rpnln, &lineLen, stdin) == -1) {
						std::cerr <<"\aERROR: Input failed...\n" <<std::endl;
						return main();
					}

					while (isspace(*rpnln))
						rpnln++;

				} while ((rpnln[0] != ':' || rpnln[0] != '\0') && rpnln[1] != '?');

				nestedIf--;

				if ((rpnln = strstr(rpnln, "?:")) != NULL)
					goto elseif;

				// get next token if there...

				goto next_token;

			}

		// ending conditional
		} else if (strcmp(p, ":?") == 0) {
			if (!nestedIf)
				std::cerr <<"\aERROR: `:?` without previous `?:`\n" <<std::endl;
			else
				nestedIf--;
		// exit the program
		} else if (*p == 'q' || !strcmp(p, "exit")) // p == "q"
			goto exit; // exit the program

		// show help
		else if (strcmp(p, "help") == 0) {
			displayHelp();
			return main();

		// clear screen
		} else if (strcmp(p, "clear") == 0 || strcmp(p, "cls") == 0) {
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif
			return main();


		// essentially restarts the program (don't display help)
		} else if (strcmp(p, "reset") == 0 ) {
			ans = line = 0;
			vars::wipeAll(vars::first_node);
			return main();

		// useful for debugging
		} else if (strcmp(p, "showvars") == 0 || strcmp(p, "vars") == 0 || strcmp(p, "listvars") == 0) {
			UserVar* var = vars::first_node->next;

			while (var != NULL){
				if (var->val.type == CalcValue::NUM)
					std::cout <<"\n $"<<var->name <<" = " <<var->val.getNum();
				else
					std::cout <<"\n $"<<var->name <<" = \"" <<var->val.getStr() <<'\"';

				var = var->next;
			}

			std::cout <<std::endl;

		// typeof function
		} else if (strcmp(p, "typeof") == 0) {
			if (mainStack.top().type == CalcValue::STR) {
				mainStack.pop();
				mainStack.push("string");
			} else if (mainStack.top().type == CalcValue::NUM) {
				mainStack.pop();
				mainStack.push("number/boolean");
			}

		// system call (problem: this conflicts with the current strategy for handling if statements.....)
		} else if (strcmp(p, "syscall") == 0 || strcmp(p, "systemcall") == 0) {
			if (mainStack.top().type == CalcValue::STR)
				system(mainStack.top().getStr());
			else
				std::cerr <<"Ummm..." <<std::endl;

		// bitwise not operator
		} else if (*p == '~' && *(p + 1) != '\0')
			mainStack.push(~atoi(p + 1));

		// assignment operator
		else if (*p == '=' && *(p + 1) == '\0')
			if (variableName2 != NULL)
				vars::assignVar(vars::first_node, variableName2, mainStack.top());
			else if (variableName1 != NULL)
				vars::assignVar(vars::first_node, variableName1, mainStack.top());
			else {
				std::cerr <<"\aERROR: inappropriate use of assignment operator.\n" <<std::endl;
				return main();
			}

		// variable
		else if (*p == '$' && *(p + 1) != '\0') { // user must use '$' prefix to access the variables

				UserVar* var = vars::findVar(vars::first_node, p + 1);

				if (var != NULL) {

					mainStack.push(var->val);

					if (variableName2 == NULL)
						variableName2 = p + 1;
					else
						variableName1 = p + 1;

				} else
					if (variableName2 == NULL)
						variableName2 = p + 1;
					else
						variableName1 = p + 1;

		}
		else if (strcmp(p, "segfault") == 0)
			for (uint8_t i = 0; i < 10; i++)
				std::cout <<getNextValue(mainStack).getNum() <<std::endl;

		/*delete a variable
		else if (strcmp(p, "delete") == 0) {
			std::cout <<"$a deleted\n" <<std::endl;
			vars::removeVar(vars::first_node, variableName1 ? variableName1 : variableName2);
		}

		* Functions haven't been implemented yet
		// user is defining a function
		} else if (strcmp(p, "@function")) {


		// user is calling a function
		} else  if (*p == '@' && *(p + 1) != '\0') {



		// anything else
		} */

		// user has given a string :D
		else if (*p == '\"')
			mainStack.push((p + 1)); // segfault?
		else {
			// parse input
			double number = atof(p);

			// the user is an asshole :T
			if (number == 0 && *p != '0') {
				std::cerr <<"\aSYNTAX ERROR: near `" <<p <<"`" <<std::endl;
				return main();

			// the user has given us a number :D
			} else
				mainStack.push(number);

		}

next_token:

		// get next token
		p = qtok(rpnln, &rpnln);

	}

	// prevent memory leaks...
	free(rpnln_head);

	if (!mainStack.empty() && !nestedIf) {
		ans = mainStack.top();
		if (ans.type == CalcValue::NUM)
			std::cout <<"ans " <<ans.getNum() <<" =\n";
		else
			std::cout <<"ans \"" <<ans.getStr() <<"\" =\n";
	}

	return main(); //next line... (infinite loop)

exit:
//	vars::wipeAll();
	delete vars::first_node;

	exit(EXIT_SUCCESS);
}
