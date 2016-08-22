#ifndef RPN_UTILS_H
#define RPN_UTILS_H

#include "calc_value.h"


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <stack>
#include <queue>

#include "tokenizer.h"


inline void displayHelp(){
    std::cout <<"If this is your first time using this language, you should check\
the README on this project's github page. https://github.com/dvtate/rpn" <<std::endl;
}


CalcValue getNextValue(std::stack<CalcValue>& valStack){

  	if (!valStack.empty()) {
		CalcValue topVal = valStack.top();
        valStack.pop();
        return topVal;
    } else {
        std::cerr <<"\aERROR: not enough data to satisfy operator.\n" <<std::endl;
		return CalcValue();
    }
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


void printCalcValue(CalcValue& val, UserVar* first_node){

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

	}
}

void printCalcValueRAW(CalcValue& val, UserVar* first_node){

  	if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::STR)
		std::cout <<val.getStr();
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(first_node);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(first_node);
		if (ret)
			return printCalcValueRAW(*ret, first_node);

	}

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

#define CONVERT_REFS(MAINSTACK, FIRST_NODE, SHOW_ERRORS) \
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

#endif
