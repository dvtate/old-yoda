#ifndef RPN_UTILS_H
#define RPN_UTILS_H

#include "calc_value.h"


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctype.h>
#include <stack>
#include <queue>
#include <cstring>


extern int main(int, char**);

extern int argc_cpy;
extern char** argv_cpy;


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



// removes char from string from it's pointer
// this is extremely inefficient...
static inline void deleteChar(char *toDelete)
	{ memmove(toDelete, toDelete + 1, strlen(toDelete)); }

// Returns the start of the token/string-constant
// manages escape-sequences within string-constants
char* qtok(char* str, char** next){
	if (str == NULL)
		return NULL;
	char *current = str, *start = str;

    // skip beginning whitespace.
    while (*current != '\0' && isspace(*current))
        current++;

    start = current;

    if (*current == '"') {
        // Quoted token
        start = current;
        current++; // Skip the beginning quote.

		bool lastWasSlash = false;

        for (;;) {
            // Go until we find a quote or the end of string.
            while (*current != '\0'  && *current != '"') {
                lastWasSlash = false;

                if (*current == '\\') {
                    if (*(current + 1) == 'n') {
                        *current = '\n';
                        *(current + 1) = '\r';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'r') {
                        *current = *(current + 1) = '\r';
                        current++;
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 'a') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == 't') {
                        *current = '\t';
                        deleteChar(current + 1);
                    } else if (*(current + 1) == '\\') {
                        *current = '\\';
                        deleteChar(current + 1);
                        lastWasSlash = true;
                    }
                    //current++;
                }
                current++;

            }

            //current++;


            if (*current == '\0') // Reached the end of the string.
                goto finalize;

            if (*(current - 1) == '\\' && lastWasSlash == false) {
                // Escaped quote (keep going)
                current++;
                continue;
            }

            // Reached the ending quote.
            goto finalize;
        }
    }


    // Not quoted so run till we see a space.
    while (*current && !isspace(*current))
        current++;

finalize:

    if (*current) {
        // Close token if not closed already.
        *current = '\0';
        current++;
        // Eat trailing whitespace.
        while (*current && isspace(*current))
            current++;
    }

    *next = current;

    //return unescapeToken(start);
    return start;

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
