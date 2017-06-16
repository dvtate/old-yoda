#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

#include "string_stack.hpp"
#include "lambda.hpp"

#define USERVAR_NAME_MAXLENGTH 20

// to be defined later
class CalcValue;
class UserVar;

namespace vars {
	extern CalcValue* valueAtVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern CalcValue* valueAtVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
}


// this will be used for list indexes
// I want numbers to by default be converted into CalcValue::NUM and not indexes
// making a separate index class ensures that I won't break anything
class index_t {
public:
	ssize_t index;
	index_t(const ssize_t& in):
			index(in){ }
};

/// a temporary value to hold user data in
class CalcValue {

public:

	// which type is represented by the data union
	enum {  NUM,	// number/boolean
			STR,	// string
			REF,	// reference to a variable
			BLK,	// Block of code (StrStack) (subroutine) (executable array)
			ARR,    // vector
			INX,    // index of an array, type made for
			LAM     // lambda
	} type;

	// contains the data
	union {
		double		number;
		char*		string;
		StrStack*	block;
		std::vector<CalcValue>* list;
		ssize_t     index;
		Lambda*     lambda;
	};


	// this sometimes causes a core dump (QwQ)
	~CalcValue(){
		clear();
	}

	// delete current value
	void clear() {
		if (type == STR || type == REF)
			free(string); // free() accepts NULL pointers
		else if (type == BLK)
			delete block;
		else if (type == ARR)
			delete list;
		else if (type == LAM)
			delete lambda;
	}

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

	CalcValue(const std::vector<CalcValue> in_list):
		type(ARR)
	{
		list = new std::vector<CalcValue>();
		for (CalcValue elem : in_list)
			list->push_back(elem);

	}

	CalcValue(const index_t in_index):
			type(INX), index(in_index.index)
	{}

	CalcValue(const Lambda& lam) {
		type = LAM;
		lambda = new Lambda(lam);
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
			list = new std::vector<CalcValue>();
			for (const CalcValue elem : *in.list)
				list->push_back(elem);
		} else if (type == INX)
			index = in.index;
		else if (type == LAM)
			lambda = new Lambda(*in.lambda);

	}

	// C++ is awesome
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
		clear();
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
			list = new std::vector<CalcValue>();
			for (CalcValue elem : *in.list)
				list->push_back(elem);
		} else if (type == INX)
			index = in.index;
		else if (type == LAM)
			lambda = new Lambda(*in.lambda);


	}

	void setValue(const char* const str){

		clear();

		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;

	}

	void setValue(const char ch){

		clear();

		type = STR;
		string = (char*) malloc(2);
		*string = ch;
		*(string + 1) = '\0';

	}
	void setValue(double val){

		clear();

		type = NUM;
		number = val;
	}

	void setValue(const std::vector<CalcValue>& arr){
		// delete old value
		clear();

		type = ARR;
		list = new std::vector<CalcValue>();
		for (const CalcValue elem : arr)
			list->push_back(elem);
	}

	void setValue(const bool in)
		{ setValue(in ? 1.0 : 0.0); }


	void setValue(const Lambda& lam) {
		// delete old value
		clear();

		type == LAM;
		lambda = new Lambda(lam);
	}


	CalcValue& setRef(const char* const str){

		// memory leaks are pretty bad
		clear();

		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = REF;

		return *this;
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
			} else if (type == INX) {
				return (index == cv2.index);
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



	// list accessor
	CalcValue* getListElem(const std::vector<ssize_t>& elem_index) {
		if (type != ARR)
			return NULL;

		CalcValue* ret = this;

		for (ssize_t i : elem_index) {
			if (ret->list->size() <= i || ret->list->size() < abs(i)) {
				return NULL;
			}
			ret = i < 0 ?
			      &ret->list->at(ret->list->size() + i) :
			      &ret->list->at(i);
		}

		return ret;
	}
	// list modifier
	bool assignElem(const std::vector<ssize_t>& elem_index, const CalcValue& val) {

		CalcValue* mod = getListElem(elem_index);
		if (!mod)
			return false;

		mod->setValue(val);

		return true;
	}

	bool deleteListElem(const std::vector<ssize_t>& elem_index) {
		if (type != ARR)
			return false;

		CalcValue* ret = this;
		for (uint16_t i = 0; i < elem_index.size() - 1; i++) {
			if (ret->list->size() <= elem_index[i] || ret->list->size() < abs(elem_index[i])) {
				return false;
			}
			ret = i < 0 ?
			      &ret->list->at(ret->list->size() + elem_index[i]) :
			      &ret->list->at(elem_index[i]);
		}
		if (!ret || !ret->list)
			return false;

		ret->list->erase(ret->list->begin() + elem_index[elem_index.size() - 1]);
		return true;

	}
};



#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL)


#endif