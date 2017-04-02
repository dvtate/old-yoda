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

	std::cout <<"<https://github.com/dvtate/yoda/>\n";

	#ifndef __WIN32
		setTermEffect();
	#endif
}

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
			std::cerr <<"\autils.h@getLineFromFile(): line index not found";
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
			if (count == lineNumber)
				break;
			else
				count++;

		if (count) {
			fclose(file);
			return line;
		} else {
			std::cerr <<"\autils.h@getLineFromFile(): line index not found";
			fclose(file);
			return (char*) NULL;
		}

	} else {
		return (char*) NULL;
	}

}

bool printCalcValue(CalcValue& val, UserVar* first_node){

	if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<'\"' <<val.getStr() <<'\"';
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(first_node);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(first_node);
		if (ret)
			return printCalcValue(*ret, first_node);

		// find the broken reference
		ret = val.valAtRef(first_node);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(first_node))
					ret = ret->valAtRef(first_node);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<(ret->string) <<"`.\n";
		return 1;
	}


	return 0;
}

bool printCalcValueRAW(CalcValue& val, UserVar* first_node){
	//printf("value is<");
	if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::BLK) {
		size_t len = 50;
		char* str = (char*) malloc(len);
		val.block->toString(&str, &len);
		printf(str);
		free(str);
  	} else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<val.getStr();
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(first_node);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(first_node);
		if (ret)
			return printCalcValueRAW(*ret, first_node);

		// find the broken reference
		ret = val.valAtRef(first_node);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(first_node))
					ret = ret->valAtRef(first_node);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<(ret->string) <<"`.\n";
		return 1;
	}

	//printf(">\n");

	return 0;

}


CalcValue* valAtRef(CalcValue cv, UserVar* first){

find_var:
	if (cv.type != CalcValue::REF)
		return (CalcValue*) 0x0;

	UserVar* var = vars::findVar(first, cv.string);
	if (var == NULL)
		return (CalcValue*) 0x0;

	CalcValue* ret = &var->val;


	if (ret->type == CalcValue::REF) {
		cv = *ret;
		goto find_var;
	} else
		return ret;


}


/*
CalcValue* valAtRef(const CalcValue& cv, UserVar* first){

find_var:
	if (cv.type != CalcValue::REF)
		return (CalcValue*) NULL;

	UserVar* var = vars::findVar(first, cv.string);
	if (var == NULL)
		return (CalcValue*) NULL;

	CalcValue* ret = &var->val;

	if (ret->type == CalcValue::REF) {
		cv = *ret;
		goto find_var;
	} else
		return ret;


}

char*& valAtRef_debug(const CalcValue& cv, UserVar* first){

find_var:
	if (cv.type != CalcValue::REF)
		return (char*) NULL;

	UserVar* var = vars::findVar(first, cv.string);
	if (var == NULL)
		return cv.string;

	CalcValue* ret = &var->val;

	if (ret->type == CalcValue::REF) {
		cv = *ret;
		goto find_var;
	} else
		return (char*) NULL;


}
*/

namespace commands {
	inline void debugStack(std::stack<CalcValue> mainStack, UserVar* first_node){
		// since the stack is copied, we can modify it as we wish in here :)
		size_t index = 0;
		while (!mainStack.empty()) {
			std::cout <<index++ <<" : ";
			printCalcValue(mainStack.top(), first_node);
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

#endif
