#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

#include "string_stack.hpp"

#define USERVAR_NAME_MAXLENGTH 20

// to be defined later
class CalcValue;
class UserVar;

namespace vars {
	extern CalcValue* valueAtVar(UserVar* first, char name[USERVAR_NAME_MAXLENGTH]);
	extern CalcValue* valueAtVar(std::vector<UserVar>& vars, char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(std::vector<UserVar>& vars, char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(UserVar* first, char name[USERVAR_NAME_MAXLENGTH]);
}


/// a temporary value to hold user data in
class CalcValue {

public:

	// which type is represented by the data union
	enum {  NUM,	// number/boolean
			STR,	// string
			REF,	// reference to a variable
			ARR,	// vector
			BLK		// Block of code (StrStack) (subroutine) (executable array)
	} type;

	// contains the data
	union {
		double		number;
		char*		string;
		StrStack*	block;
		std::vector<CalcValue>* list;
	};

	// Null object
	CalcValue(): type(STR), string(NULL) {}


	CalcValue (double val):	type(NUM) {
		number = val;
		type = NUM;
	}


	CalcValue(const char* const str): type(STR) {

		if (str) {
			// allocate memory for the string
			string = (char*) malloc(strlen(str) + 1);

			// write the string to the buffer
			strcpy(string, str);

			// declaring a NULL_CALCVAL_OBJECT
		} else
			string = NULL;
	}

	CalcValue(const StrStack& codeBlock): type(BLK)
		{ block = new StrStack(codeBlock); }

	CalcValue(StrStack* codeBlock): type(BLK)
		{ block = new StrStack(*codeBlock); }

	CalcValue(const std::vector<CalcValue>& in_list):
		type(ARR)
	{
		list = new std::vector<CalcValue>();
	}

	CalcValue(const CalcValue& in){
		// no need to delete anything as nothing is there yet

		// they will be the same type of data
		type = in.type;

		// copy in the value
		if (type == NUM)
			number = in.number;
		else if (type == STR || type == REF) {

			if (in.string) {
				string = (char*) malloc(strlen(in.string) + 1);
				strcpy(string, in.string);
			} else
				string = NULL;

		} else if (type == BLK)
			block = new StrStack(*in.block);
		else if (type == ARR) {
			list->resize(in.list->size());
			for (CalcValue elem : *in.list)
				list->push_back(elem);

		}

		//printf("copying CV...\n");
		//printf("copyied CV...\n");
	}

	// lol
	template<class T>
	CalcValue& operator=(const T val) {
		setValue(val);
		return *this;
	}

	CalcValue& operator=(const CalcValue in) {
		setValue(in);
		return *this;
	}
	void setValue(const CalcValue in){

		// delete old value
		if (type == STR || type == REF)
			free(string);
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;


		// they will be the same type of data
		type = in.type;

		// copy in the value
		if (type == NUM)
			number = in.number;
		else if (type == STR || type == REF) {

			if (in.string) {
				string = (char*) malloc(strlen(in.string) + 1);
				strcpy(string, in.string);
			} else
				string = NULL;

		} else if (type == BLK)
			block = new StrStack(*in.block);
		else if (type == ARR)
			list = in.list;
	}

	// this sometimes causes a core dump (QwQ)
	~CalcValue(){
		if (type == STR || type == REF)
			free(string); // free() accepts NULL pointers
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;

	}

	void setValue(const char* const str){

		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free() accepts NULL pointers
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;

		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;

	}

	void setValue(const char ch){

		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free() accepts NULL pointers
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;


		type = STR;
		string = (char*) malloc(2);
		*string = ch;
		*(string + 1) = '\0';

	}
	void setValue(double val){

		// delete old value
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;

		type = NUM;
		number = val;
	}

	void setValue(const std::vector<CalcValue>& arr) {
		// delete old value
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;

		type = ARR;
		*list = arr;
		list->resize(arr.size());
		for (CalcValue elem : arr)
			list->push_back(elem);
	}

	CalcValue& setRef(const char* const str){

		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;


		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = REF;

		return *this;
	}

	CalcValue& setStr(const char* const str){
		// memory leaks are pretty bad
		if (type == STR || type == REF)
			free(string); // free(NULL) gives no errors :)
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;


		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;

		return *this;

	}

	void setNull(){

		if (type == STR || type == REF)
			free(string);
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;

		type = STR;
		string = NULL;

	}


	double getNum(){
		if (type == NUM)
			return number;
		else if (type == STR)
			return strlen(string);
		else
			return 0;
	}

	char* getStr(){
		if (type == STR)
			return string;
		else
			return NULL;
	}

	char* getRef(){
		if (type == REF)
			return string;
		else
			return NULL;
	}

	CalcValue* valAtRef(UserVar* first){
		if (type == REF)
			return vars::valueAtVar(first, string);
		else
			return NULL;
	}

	CalcValue* valAtRef(std::vector<UserVar>& vars){
		if (type == REF)
			return vars::valueAtVar(vars, string);
		else
			return NULL;
	}


	bool operator==(const CalcValue& cv2) {

		// same type
		if (type == cv2.type) {

			// same value
			if (type == CalcValue::NUM && number == cv2.number)
				return true;
			else if ((type == CalcValue::STR || cv2.type == CalcValue::REF))
				return strcmp(string, cv2.string) == 0;
			// this doesnt work...
			else if (type == CalcValue::ARR) {
				if (list->size() != cv2.list->size())
					return false;
				for (size_t i = 0; i < list->size() - 1; i++)
					if (!((*list)[i] == (*cv2.list)[i]))
						return false;

				return true;
			}
		}

		return false;


	}

	// A Null value
	bool isEmpty()
		{ return (type == STR) && (string == NULL); }
	bool isNull()
		{ return isEmpty(); }

	// type checking (doesn't get used) :/
	bool isRef()
		{ return type == REF; }
	bool isStr()
		{ return !isEmpty() && type == STR; }
	bool isNum()
		{ return type == NUM; }
	bool isBlk()
		{ return type == BLK; }
	bool isArr()
		{ return type == ARR; }
};



#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL)


#endif