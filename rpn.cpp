#include <iostream>
#include <stack>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>


#define MAX_LEN 200

#define PI 3.14159265359

/// > 1 1 +
/// ans = 2

/// > q
/// *program exits

/// > ans
/// ans = {ans from previous prompt}


double getNextNumber(std::stack<double>);
double ans = 0;
unsigned int line = 0;

int main(){

	// display the helper
	if (line == 0)
		std::cout <<"\n\t\tRPN Calculator\nPlace the operator after its two operands. \
Here is an example:\n > 1 1 + \n ans = 2\n\nTo use the previous answer \
replace one number with `ans` as in the following example:\n \
> ans\n ans = 2\n\nWhen you are finished, type `q` or `exit` to exit the program.\n"
				  <<std::endl;


	std::stack<double> numstack;
	double a, b;

	std::cout <<line++ <<"> ";

	char rpnln[MAX_LEN + 1];
	std::cin.getline(rpnln, MAX_LEN);

	// get first token from the input
	char* p = strtok(rpnln, " ");

	// empty string/whitespace input
	if (p == NULL)
		return main();


	while (p != NULL) {

		// char is a binary operator
		if (((*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '|'
			|| *p == '%' || *p == '&' || *p == '^') && *(p + 1) == '\0')
			|| !strcmp(p, "<<") || !strcmp(p, ">>")  || !strcmp(p, "**")
            || !strcmp(p, "logBase")
		) {

            b = getNextNumber(numstack);
            a = getNextNumber(numstack);

            /*
			if (!numstack.empty()) {
				a = numstack.top();
				numstack.pop();
			} else {
				std::cerr <<"ERROR: Too many operators.\n" <<std::endl;
				return main();
			}
            */

			switch (*p) {
				case '+': numstack.push(a + b); break;
				case '*':
					if (*(p + 1) == '*')
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
				case 'l': numstack.push(log(b) / log(a)); break;
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
		else {

            if (strcmp(p, "pi") == 0)
                numstack.push(PI);

			else if (strcmp(p, "ans") == 0) // p == "ans"
				numstack.push(ans);

			else if (*p == 'q' || !strcmp(p, "exit")) // p == "q"
				goto exit; // exit the program with

			else if (*p == '~' && *(p + 1) != '\0')
                numstack.push(~atoi(p + 1));

			else { // number constant (pushes 0 if not)
                double number = atof(p);
                if (number == 0 && *p != '0') {
                    std::cerr <<"SYNTAX ERROR\n" <<std::endl;
                    return  main();
                }
                numstack.push(number);

			}

		}

        // get next token
		p = strtok(NULL, " ");

	}

	std::cout <<"ans = " <<(ans = numstack.top()) <<'\n' <<std::endl;

	return main(); //next line...

exit:
    return 0;
}


inline double getNextNumber(std::stack<double> numberStack){
    double topNum;
    if (!numberStack.empty()) {
		topNum = numberStack.top();
		numberStack.pop();
		return topNum;
	} else {
		std::cerr <<"ERROR: Too many operators.\n" <<std::endl;
		return main();
    }


}


