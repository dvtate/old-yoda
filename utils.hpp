#ifndef RPN_UTILS_H
#define RPN_UTILS_H

/// This file really should be called ADHD_tools.h since it's a completely disorganized array of random shit

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stack>
#include <queue>

#include "calc_value.hpp"
#include "terminal_colors.hpp"
#include "win_supp.h"


extern char* progName;

// print help monologue
void displayHelp();

namespace strutils {

	// removes char from string
	// this is extremely inefficient, but the best way I can think of...
	void deleteChar(char* toDelete);
	void deleteChars(char* toDelete, const size_t numChars);



	char* skipSpaces(char* p);


	// remove spaces preceding and following string
	char* trimStr(char* string);

	// thx @chux http://stackoverflow.com/a/27305359/4475863
	char* stristr(const char* haystack, const char* needle);

	// find the number of times a certain substring occurs in a string (unused)
	unsigned int countOccurances(const char* str, char* sub);

	// You must free the result if result is non-NULL.
	// thanks: http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
	// I only translated answer to C++
	char* str_replace(char *orig, const char *rep, const char *with);
}

namespace fileutils {

	// remember to free()
	char* getLineFromFile(const char* filename, size_t lineNumber);
	char* getLineFromFile(FILE* file, size_t lineNumber);

	char* mktmpPrefix();
	FILE* mktmpfile();

	extern std::vector<char*> tmp_files;
	void delTmpFiles();


}
// strings in quotes (debug)
bool printCalcValue(CalcValue& val, std::vector<UserVar>& var_nodes);
// no quotes on strings (prod)
bool printCalcValueRAW(CalcValue& val, std::vector<UserVar>& var_nodes);




namespace commands {
	void debugStack(std::stack<CalcValue> mainStack, std::vector<UserVar>& vars);
}


size_t linesToEnd(FILE* fil);

// variable name mutilator which uses a base 62 counting system to give variables unique names
namespace mutilate {

	extern const uint8_t L_REF_MUTILATOR; // number of numberals
	extern const char baseNumerals[62]; // base 62 numerals

	// makes a variable name unique
	char* mutilateVarName(const char *cur_name);
}



#endif