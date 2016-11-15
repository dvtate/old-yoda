#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <string.h>
#include <stdlib.h>


#include "string_stack.hpp"

#define USERVAR_NAME_MAXLENGHT 20
// to be defined later
class CalcValue;
class UserVar;
namespace vars {
	extern CalcValue* valueAtVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);
	extern UserVar* findVar(UserVar* first, char* name);
}


/// a temporary value to hold user data in
class CalcValue {

public:

	// which type is represented by the data union
	enum { NUM,	// number/boolean
		   STR,	// string
		   REF,	// reference to a variable
		   ARR,	// linked-list
		   STK	// string-stack (subroutine)
		 } type;

	// this contains the data
	union {
		double		number;
		char*		string;
		StrStack*	block;
	};

  	// Null object
	CalcValue(): type(STR), string(NULL) {}


  	CalcValue (double val):	type(NUM) {
		number = val;
		type = NUM;
	}


	CalcValue(const char* const str): type(STR) {

		type = STR;

		if (str) {
			// allocate memory for the string
		  	string = (char*) malloc(strlen(str) + 1);

			// write the string to the buffer
			strcpy(string, str);

		// declaring a NULL_CALCVAL_OBJECT
		} else
			string = NULL;
	}
/*
	CalcValue(const StrStack& codeBlock): type(STK) {
		block = new StrStack();


	}
*/

	// this causes a core dump (QwQ)
	/*~CalcValue(){
		if (type == STR)
			free(string);
		else if (type == STK)
			delete block;
	}*/

	void setValue(const char* const str) {

		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)


		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;
	}

	void setValue(double val){
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)
		number = val;
		type = NUM;
	}

	void setValue(const CalcValue& in){
		if (in.type == NUM) {
		  	if (type == STR && string != NULL)
				free(string);
			type = CalcValue::NUM;
		  	number = in.number;
		} else {
			if (type == STR && string != NULL)
			  	free(string);

			type = in.type;
			if (in.string != NULL) {
			  	string = (char*) malloc(strlen(in.string) + 1);
				strcpy(string, in.string);
			} else
				string = NULL;
		}
	}

	CalcValue& operator=(const CalcValue& in){
		setValue(in);
		return *this;
	}

	CalcValue& setRef(const char* const str){

		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)


		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = REF;

		return *this;
	}

	void setNull(){

		if (type == STR || type == REF)
			free(string);

		type = STR;
		string = NULL;

	}


	double getNum(){
		if (type == NUM)
			return number;
		else
			return 0;
	}

	char* getStr(){
	  	if (type == STR)
			return string;
	  	else
		  	return (char*) NULL;
	}

	char* getRef(){
	  	if (type == REF)
			return string;
	  	else
		  	return (char*) NULL;
	}

	CalcValue* valAtRef(UserVar* first){
		if (type == REF)
			return vars::valueAtVar(first, string);
		else
			return NULL;
	}

	bool operator==(const CalcValue& val2){

		// same type
		if (type == val2.type) {

			// same value
			if (type == NUM && number == val2.number)
				return true;
			else if ((type == STR || type == REF) && strcmp(string, val2.string) == 0)
				return true;

		}

	  	return false;

	}

	// A Null value
	bool isEmpty()
		{ return (type == STR) && (string == NULL); }
	bool isNull()
		{ return isEmpty(); }


	bool isRef()
		{ return !isEmpty() && type == REF; }
	bool isStr()
		{ return type == STR; }
	bool isNum()
		{ return type == NUM; }


};




#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL)


#endif
