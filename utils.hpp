#ifndef RPN_UTILS_H
#define RPN_UTILS_H


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stack>
#include <queue>

#include "calc_value.hpp"
#include "tokenizer.h"
#include "terminal_colors.hpp"

inline void displayHelp(){ // this could all be combined into one print statement...
	std::cout <<"If this is your first time using this language, you should check\
 the README on this project's github page for a short intro.\n";

	// I want the URL to be underlined and blink :)
	#ifndef _WIN32
		setTermEffect(TERM_EFF_BLINK);
		setTermEffect(TERM_EFF_UNDERLINE);
	#endif

	std::cout <<"<https://github.com/dvtate/rpn/>\n";

	#ifndef __WIN32
		setTermEffect();
	#endif
}


// removes char from string
// this is extremely inefficient, but the best way I can think of...
inline void deleteChar(char* toDelete)
{ memmove(toDelete, toDelete + 1, strlen(toDelete)); }
inline void deleteChars(char* toDelete, const size_t numChars)
{ memmove(toDelete, toDelete + numChars, strlen(toDelete)); }


template<class T>
inline T getNextValue(std::stack<T>& stk){
	const T topVal = stk.top();
	stk.pop();
	return topVal;
}

template <class T>
inline void emptyStack(std::stack<T>& stk){
	while (!stk.empty())
		stk.pop();
}

inline char* skipSpaces(char* p){
	while (isspace(*p))
		p++;

	return p;
}

char* getLineFromFile(const char* filename, size_t lineNumber){
	FILE *file = fopen(filename, "r");

	size_t count = 1;
	if (file != NULL) {

		char* line = (char*) malloc(200);
		size_t lineLen = 200;


		while (getline(&line, &lineLen, file) != -1)
			if (count == lineNumber)
				break;
			else
				count++;

		if (count) {
			fclose(file);
			return line;
		} else {
			std::cerr <<"\autils.h: getLineFromFile(): line index not found";
			fclose(file);
			return (char*) NULL;
		}

	} else {
		std::cerr <<"\aDAFUQ: fopen(\"" <<filename <<"\", \"r\") == NULL\n"
				  <<__FILE__ <<':' <<__LINE__ <<std::endl;
		return (char*) NULL;
	}

}
char* getLineFromFile(FILE* file, size_t lineNumber){

	size_t count = 1;
	if (file != NULL) {

		char* line = (char*) malloc(200);
		size_t lineLen = 200;


		while (getline(&line, &lineLen, file) != -1)
			if (count == lineNumber) break;
			else count++;

		if (count) {
			fclose(file);
			return line;
		} else {
			std::cerr <<"\autils.h: getLineFromFile(): line index not found";
			fclose(file);
			return (char*) NULL;
		}

	} else
		return (char*) NULL;

}

bool printCalcValue(CalcValue& val, std::vector<UserVar>& var_nodes){

	if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<'\"' <<val.getStr() <<'\"';
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(var_nodes);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(var_nodes);
		if (ret)
			return printCalcValue(*ret, var_nodes);

		// find the broken reference
		ret = val.valAtRef(var_nodes);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(var_nodes))
					ret = ret->valAtRef(var_nodes);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<(ret->string) <<"`.\n";
		return 1;
	}


	return 0;
}

bool printCalcValueRAW(CalcValue& val, std::vector<UserVar>& var_nodes){
	//printf("value is<");
	if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::BLK) {
		size_t len = 50;
		char* str = (char*) malloc(len);
		val.block->toString(&str, &len);
		printf("%s", str);
		free(str);
	} else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<val.getStr();
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(var_nodes);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(var_nodes);
		if (ret)
			return printCalcValueRAW(*ret, var_nodes);

		// find the broken reference
		ret = val.valAtRef(var_nodes);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(var_nodes))
					ret = ret->valAtRef(var_nodes);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<(ret->string) <<"`.\n";
		return 1;
	}

	return 0;

}




namespace commands {
	inline void debugStack(std::stack<CalcValue> mainStack, std::vector<UserVar>& vars){
		// since the stack is copied, we can modify it as we wish in here :)
		size_t index = 0;
		while (!mainStack.empty()) {
			std::cout <<index++ <<" : ";
			printCalcValue(mainStack.top(), vars);
			std::cout <<'\n';

			mainStack.pop();
		}

	}

	// thx @chux http://stackoverflow.com/a/27305359/4475863
	char* stristr(const char* haystack, const char* needle) {
		do {
			const char* h = haystack;
			const char* n = needle;
			while (tolower(*h) == tolower(*n) && *n) {
				h++;
				n++;
			}

			if (!*n)
				return (char *) haystack;

		} while (*haystack++);

		return NULL;
	}
}

inline char* trimStr(char* string){
	while (isspace(*string))
		string++;

	char* ret = string;
	while (!isspace(*string) && *string != '\0')
		string++;

	*(string) = '\0';

	return ret;

}

inline size_t linesToEnd(FILE* fil){
	size_t numLines = 0;

	int ch;
	while (EOF != (ch=getc(fil)))
		if ('\n' == ch)
			++numLines;

	return numLines;
}

// find the number of times a certain substring occurs in a string (unused)
unsigned int countOccurances(const char* str, char* sub) {
		unsigned int ret = 0;
		while (*str) {
			char* substr = sub;
			while (*substr && *substr == *str) {
				str++; substr++;
			}
			if (!*substr) ret++;
			else str++;
		}

		return ret;
}

// You must free the result if result is non-NULL.
// thanks: http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
// translated answer to C++
char *str_replace(char *orig, const char *rep, const char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	size_t len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;

	len_rep = strlen(rep);

	// empty rep causes infinite loop during count
	if (len_rep == 0)
		return NULL;

	if (!with) {
		char emptystring[1];
		with = emptystring;
	}
	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = (char*) malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

#endif