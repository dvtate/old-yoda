#include <iostream>
#include <stack>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "user_variables.h"


#define MAX_LEN 200





long double getNextNumber(std::stack<long double>& );

long double ans = 0;
unsigned int line = 0;



inline void displayHelp(){
    std::cout <<"\n\t\tRPN Calculator\nPlace the operator after its two operands. \
Here is an example:\n > 1 1 + \n ans = 2\n\nTo use the previous answer \
replace one number with `ans` as in the following example:\n \
> ans\n ans = 2\n\nWhen you are finished, type `q` or `exit` to exit the program.\n"
              <<std::endl;
}



int main(){

	// display the helper
	if (line == 0)
		displayHelp();

// goto's can be evil, but this program is too smalle for this to be an issue.
main_start_after_help:

	std::stack<long double> numstack;
	long double a, b;

	std::cout <<line++ <<"> ";

	char rpnln[MAX_LEN + 1];
	std::cin.getline(rpnln, MAX_LEN);

	// used for storing the name for user variables
	char* variableName = NULL; // will get used later


	// get first token from the input
	char* p = strtok(rpnln, " ");

	// empty string/whitespace input
	if (p == NULL)
		return main();

	while (p != NULL) {

		// char is a binary operator
		if (((*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%'
			|| *p == '|' || *p == '&' || *p == '^') && *(p + 1) == '\0')
			|| !strcmp(p, "<<") || !strcmp(p, ">>")  || !strcmp(p, "**")
            || !strcmp(p, "logBase") || !strcmp(p, "logBASE") || !strcmp(p, "logbase")
            || !strcmp(p, "pow")
		) {

            b = getNextNumber(numstack);
            a = getNextNumber(numstack);

			switch (*p) {
				case '+': numstack.push(a + b); break;
				case '*':
					if (strcmp(p, "**") == 0)
						numstack.push(pow(a, b));
					else
						numstack.push(a * b);
					break;
				case '/': numstack.push(a / b); break;
				case '-': numstack.push(a - b); break;
				case '%': numstack.push((int) a % (int) b); break;
				case '|': numstack.push((int) a | (int) b); break;
				case '^': numstack.push((int) a ^ (int) b); break;
				case '&': numstack.push((int) a & (int) b); break;
				case '<': numstack.push((int) a << (int) b); break;
				case '>': numstack.push((int) a >> (int) b); break;
				case 'l': numstack.push(log10(b) / log10(a)); break;
				case 'p': numstack.push(pow(a, b)); break;
			}

		}
		// char is a unary operator
            //trig functions
        else if (strcmp(p, "sin") == 0)
            numstack.push(sin(getNextNumber(numstack)));
        else if (strcmp(p, "cos") == 0)
            numstack.push(cos(getNextNumber(numstack)));
        else if (strcmp(p, "tan") == 0)
            numstack.push(tan(getNextNumber(numstack)));

        else if (strcmp(p, "asin") == 0)
            numstack.push(asin(getNextNumber(numstack)));
        else if (strcmp(p, "acos") == 0)
            numstack.push(acos(getNextNumber(numstack)));
        else if (strcmp(p, "atan") == 0)
            numstack.push(atan(getNextNumber(numstack)));

        else if (strcmp(p, "sinh") == 0)
            numstack.push(sinh(getNextNumber(numstack)));
        else if (strcmp(p, "cosh") == 0)
            numstack.push(cosh(getNextNumber(numstack)));
        else if (strcmp(p, "tanh") == 0)
            numstack.push(tanh(getNextNumber(numstack)));

        else if (strcmp(p, "asinh") == 0)
            numstack.push(asinh(getNextNumber(numstack)));
        else if (strcmp(p, "acosh") == 0)
            numstack.push(acosh(getNextNumber(numstack)));
        else if (strcmp(p, "atanh") == 0)
            numstack.push(atanh(getNextNumber(numstack)));

        else if (strcmp(p, "log") == 0 || strcmp(p, "log10") == 0)
            numstack.push(log10(getNextNumber(numstack)));
        else if (strcmp(p, "ln") == 0)
            numstack.push(log(getNextNumber(numstack)));
        else if (strcmp(p, "sqrt") == 0 || strcmp(p, "sqr") == 0)
            numstack.push(sqrt(getNextNumber(numstack)));

        // not an operator
		else if (*p == '#') { // comments... because I can XDDDDDDDDD
                if (numstack.size() == 0)
                    goto main_start_after_help;
                break;

		} else if (strcmp(p, "pi") == 0)
			numstack.push(M_PI); 

		else if (strcmp(p, "ans") == 0) // p == "ans"
				numstack.push(ans);

		else if (*p == 'q' || !strcmp(p, "exit")) // p == "q"
			goto exit; // exit the program

		else if (strcmp(p, "help") == 0) {
			displayHelp();
			return main();
		} else if (strcmp(p, "clear") == 0 || strcmp(p, "cls") == 0) {
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif
			return main();

        } else if (strcmp(p, "reset") == 0 ) {
			ans = line = 0;
			goto main_start_after_help;

		} else if (*p == '~' && *(p + 1) != '\0')
			numstack.push(~atoi(p + 1));

		else if (*p == '=' && *(p + 1) == '\0')
			vars::assignVar(variableName, numstack.top());
	
		else if (*p == '$'){
			if (strlen(p + 1) > USERVAR_NAME_MAXLENGHT) {
				std::cerr <<"ERROR: Variable name too long.\n" <<std::endl;
				return main(); // start over
			} else if (vars::varExists(p + 1))
				numstack.push(vars::findVar(p + 1)->value);
			 else 
				variableName = p + 1;
		}

		else { // anything else
			long double number = atof(p);
			if (number == 0 && *p != '0') {
				std::cerr <<"SYNTAX ERROR\n" <<std::endl;
				return main();
			} else
				numstack.push(number);
		}

		// get next token
		p = strtok(NULL, " ");

	}

	std::cout <<"ans = " <<(double)(ans = numstack.top()) <<'\n' <<std::endl;

	return main(); //next line...

exit:
	vars::wipeAll();
	delete vars::first;

	return 0;
}

long double getNextNumber(std::stack<long double>& numberStack){
    double topNum;
    if (!numberStack.empty()) {
		topNum = numberStack.top();
		numberStack.pop();
		return topNum;
	} else {
		std::cerr <<"ERROR: Too many operators.\n" <<std::endl;
		main();
		return 0;
    }
}
