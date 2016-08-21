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
#include "calc_value.h"

// user defined variables
#include "user_variables.h"

// conditional statements
#include "conditionals.h"

// some useful functions
#include "utils.h"



extern CalcValue ans;

char* processLine(
	std::stack<CalcValue>& mainStack, UserVar* first_node,
	std::queue<char*>& varNames, bool& showErrors, char*& rpnln
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

			if (mainStack.top().type != CalcValue::NUM) {
				if (showErrors)
					std::cerr <<"ERROR: incompatible data-types! (expected two numbers)";
				return p;
			}

			double b = getNextValue(mainStack).getNum();

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

			CalcValue b = getNextValue(mainStack),
					  a = getNextValue(mainStack);

plusop_check_values:

			// handling null values
		  	if (a.isEmpty() != b.isEmpty()) {
		  		if (a.isEmpty())
		  			mainStack.push(b);
		  		else
		  			mainStack.push(a);

				// get next token
				p = qtok(rpnln, &rpnln);

				continue;

			} else if (a.isEmpty() && b.isEmpty()) {
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

				} else if (b.type == CalcValue::REF) {
					CalcValue* val = b.valAtRef(first_node);
					if (val != NULL) {
						b = *val;
						goto plusop_check_values;

					} else {
						if (showErrors)
							std::cerr <<"\aERROR: not enough data to satisfy `+` operator.\n";
						return p;

					}

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

				else if (b.type == CalcValue::REF) {
					CalcValue* val = b.valAtRef(first_node);
					if (val != NULL) {
						b = *val;
						goto plusop_check_values;

					} else {
						if (showErrors)
							std::cerr <<"\aERROR: not enough data to satisfy `+` operator.\n";
						return p;

					}

				}

			} else if (a.type == CalcValue::REF) {
					CalcValue* val = a.valAtRef(first_node);
					if (val != NULL) {
						a = *val;
						goto plusop_check_values;

					} else {
						if (showErrors)
							std::cerr <<"\aERROR: not enough data to satisfy `+` operator.\n";
						return p;

					}
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

		// more unary math functions
		else if (strcmp(p, "log") == 0 || strcmp(p, "log10") == 0)
			mainStack.push(log10(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "ln") == 0)
			mainStack.push(log(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0)
			mainStack.push(sqrt(getNextValue(mainStack).getNum()));
		else if (strcmp(p, "abs") == 0)
			mainStack.push(std::abs(getNextValue(mainStack).getNum()));


		// comments
		else if (*p == '#') {
			if (mainStack.size() == 0)
				return NULL;
			break;

		// pi
		} else if (strcmp(p, "pi") == 0)
			mainStack.push(M_PI);

		// ans
		else if (strcmp(p, "ans") == 0) // p == "ans"
				mainStack.push(ans);

		else if (strcmp(p, "print") == 0) {
			if (mainStack.empty()) {

				if (showErrors)
					std::cerr <<"\aERROR: not enough data to satisfy print\n";

				return p;

			} else {

				printCalcValueRAW(mainStack.top(), first_node);
				mainStack.pop();

			}


		// user input
		} else if (strcmp(p, "input") == 0) {
			char* input = (char*) malloc(256);
			size_t lineLen = 256;

			if (getline(&input, &lineLen, program) == -1) {
				if (showErrors)
					std::cerr <<"\aERROR: input could not getline()\n";
				return p;
			}

			mainStack.push(input);

		// convert to string
		} else if (strcmp(p, "str") == 0) {
			CalcValue val = getNextValue(mainStack);

str_convert_process_value:
		  	if (val.type == CalcValue::STR)
				mainStack.push(val.getStr());
			else if (val.type == CalcValue::NUM) {
				char str[8];
				snprintf(str, 7, "%g", val.getNum());
				mainStack.push(str);
			} else if (val.type == CalcValue::REF) {
				CalcValue* value = val.valAtRef(first_node);

				while (value != NULL && value->type == CalcValue::REF)
					value = value->valAtRef(first_node);

				if (value == NULL) {
					if (showErrors)
						std::cerr <<"\aERROR: str: broken reference to `$" <<val.string <<"`.\n";
					return p;
				} else {
					val = *value;
					goto str_convert_process_value;
				}
			}

		// convert to number
		} else if (strcmp(p, "num") == 0) {
			CalcValue val = getNextValue(mainStack);

num_convert_process_value:
			if (val.isEmpty())
			 	return p;
		  	else if (val.type == CalcValue::NUM)
				mainStack.push(val.getNum());
			else if (val.type == CalcValue::STR)
				mainStack.push(atof(val.getStr()));
			else if (val.type == CalcValue::REF) {
				CalcValue* value = val.valAtRef(first_node);

				while (value != NULL && value->type == CalcValue::REF)
					value = value->valAtRef(first_node);

				if (value == NULL) {
					if (showErrors)
						std::cerr <<"\aERROR: num: broken reference to `$" <<val.string <<"`.\n";
					return p;
				} else {
					val = *value;
					goto num_convert_process_value;
				}

			}
		// convert to an integer
		} else if (strcmp(p, "int") == 0) {
			CalcValue val = getNextValue(mainStack);

int_convert_process_value:
			if (val.isEmpty())
			 	return p;
		  	else if (val.type == CalcValue::NUM)
				mainStack.push(round(val.getNum()));
			else if (val.type == CalcValue::STR)
				mainStack.push(atoi(val.getStr()));
			else if (val.type == CalcValue::REF) {
				CalcValue* value = val.valAtRef(first_node);

				while (value != NULL && value->type == CalcValue::REF)
					value = value->valAtRef(first_node);

				if (value == NULL) {
					if (showErrors)
						std::cerr <<"\aERROR: int: broken reference to `$" <<val.string <<"`.\n";
					return p;
				} else {
					val = *value;
					goto int_convert_process_value;
				}

			}
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
			if (!mainStack.empty()) {
				if (mainStack.top().isNull()) { // NULL string pointer
					mainStack.pop();
					mainStack.push("NULL_PTR");

				} else if (mainStack.top().type == CalcValue::STR) { // string-type
					mainStack.pop();
					mainStack.push("string"); // STR

				} else if (mainStack.top().type == CalcValue::NUM) { // number-type
					mainStack.pop();
					mainStack.push("number/boolean"); // NUM/BLN

				} else if (mainStack.top().type == CalcValue::REF) { // variable reference
					mainStack.pop();
					mainStack.push("reference");

				}
			} else
				mainStack.push("DataError");

		// system call (problem: this conflicts with the current strategy for handling if statements.....)
		} else if (strcmp(p, "sys") == 0 || strcmp(p, "system") == 0) {

syscall_process_value:
			if (mainStack.top().type == CalcValue::STR)
				system(mainStack.top().getStr()); // gets run in BASH/CMD

			else if (mainStack.top().type == CalcValue::STR) {
				if (showErrors)
					std::cerr <<"\aERROR: cannot make a system call with a number...\n" <<std::endl;
				return p;

			} else if (mainStack.top().type == CalcValue::REF) {
				CalcValue* value = mainStack.top().valAtRef(first_node);

				while (value != NULL && value->type == CalcValue::REF)
					value = value->valAtRef(first_node);

				if (value == NULL) {
					if (showErrors)
						std::cerr <<"\aERROR: " <<p <<": broken reference to `$"
								  <<value->string <<"`.\n";
					return p;
				}

				mainStack.top().setValue(*value);
				goto syscall_process_value;

			}

			mainStack.pop();

		// bitwise not operator
		} else if (*p == '~' && *(p + 1) != '\0')
			mainStack.push(~atoi(p + 1));

		// assignment operator
		else if (*p == '=' && *(p + 1) == '\0') { // variable assignment

			if (mainStack.size() < 2) {
				if (showErrors)
					std::cerr <<"\aERROR: not enough data for assignment.\n" <<std::endl;
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
						std::cerr <<"\aERROR: inappropriate use of assignment operator.\n" <<std::endl;
					return p;
				}

			}


		// variable
		} else if (*p == '$' && *(p + 1) != '\0') // user must use '$' prefix to access the variables

			mainStack.push(CalcValue().setRef(p + 1));

		else if (*p == '~' && *(p + 1) == '\0') {
			if (mainStack.empty()){
				if (showErrors)
					std::cerr <<"\aERROR: not enough data for copy operator\n" <<std::endl;
				return p;
			}

			if (mainStack.top().type == CalcValue::REF) {
				CalcValue* val = mainStack.top().valAtRef(first_node);

				while (val && val->type == CalcValue::REF)
					val = valAtRef(*val, first_node);

				if (val != NULL)
					mainStack.top().setValue(*val);
				else {
					if (showErrors)
						std::cerr <<"\aERROR: broken reference to $" <<mainStack.top().string <<'\n';
					return p;
				}


			} else
				{ }// do nothing...


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
		} */

		// clear the stack
		else if  (strcmp(p, "...") == 0)

			emptyStack(mainStack);

		// pop the top of the stack
		else if  (*p == ';' && *(p + 1) == '\0') {

			if (!mainStack.empty())
				mainStack.pop();

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
