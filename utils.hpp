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
#include "tokenizer.hpp"
#include "terminal_colors.h"

inline void displayHelp(){ // this could all be combined into one print statement...
    std::cout <<"If this is your first time using this language, you should check\
 the README on this project's github page.";

	// I want the URL to be underlined and blink :)
	textEffect(TERM_CLR_UNDERLINE);
	textEffect(TERM_CLR_BLINK);
	std::cout <<"https://github.com/dvtate/rpn/\n";
	textEffect(); // reset
}


inline CalcValue getNextValue(std::stack<CalcValue>& valStack){
	const CalcValue topVal = valStack.top();
    valStack.pop();
    return topVal;
}


template <class T>
inline void emptyStack(std::stack<T>& stk){
	while (!stk.empty())
		stk.pop();
}
template <class T>
inline void emptyStack(std::queue<T>& stk){
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
	if ( file != NULL ) {

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

	if (val.isNull())
		std::cout <<"null";
  	else if (val.type == CalcValue::NUM)
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
			std::cout <<std::endl;

			mainStack.pop();
		}

	}
}

inline char* trimStr(char* string){
	while (isspace(*string))
		string++;

	char* ret = string;
	while (!isspace(*string) && *string != '\0')
		string++;

	*(string + 1) = 0;

	return ret;
}


#endif
