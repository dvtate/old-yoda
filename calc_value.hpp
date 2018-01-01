#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <vector>


#include "string_stack.hpp"
#include "lambda.hpp"
#include "user_types.hpp"

#define USERVAR_NAME_MAXLENGTH 20

// to be defined later
class CalcValue;
class UserType;
class UserVar;

namespace vars {
	extern CalcValue* valueAtVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern CalcValue* valueAtVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
}

// utils.h
namespace strutils {
	extern char *trimStr(char *string);
}

// this will be used for list indexes
// I want numbers to by default be converted into CalcValue::NUM and not indexes
// making a separate index class ensures that I won't break anything
class index_t {
public:
	ssize_t index;
	index_t(const ssize_t& in):
			index(in){ }
	index_t& operator=(const index_t& in)
		{ index = in.index; return *this;}
};


/// a temporary value to hold user data in
class CalcValue {

public:

	// which type is represented by the data union
	enum base_type {
			NUM,	// number/boolean
			STR,	// string
			REF,	// reference to a variable
			BLK,	// Block of code (StrStack) (subroutine) (executable array)
			ARR,    // vector
			INX,    // index of an array, type made for
			LAM,    // lambda
			REQ,    // member request
			OBJ     // custom type
	} type;

	// contains the data
	union {
		double		number;
		char*		string;
		StrStack*	block;
		std::vector<CalcValue>*   list;
		ssize_t     index;
		Lambda*     lambda;
		std::vector<std::string>* request;
		UserType*   object;
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
		else if (type == OBJ)
			delete object;
		else if (type == REQ)
			delete request;
	}

	// Null object
	CalcValue(): type(STR), string(NULL) {}


	CalcValue (const double val):	type(NUM) {
		number = val;
	}

	CalcValue (const UserType& obj):
		type(OBJ)
	{
		object = new UserType(obj);
	}

	CalcValue(const char* const str): type(STR) {

		if (str) {
			// allocate memory for the string
			string = (char*) calloc(strlen(str) + 2, sizeof(char));

			// write the string to the buffer
			strcpy(string, str);

			// declaring a NULL_CALCVAL_OBJECT
		} else
			string = NULL;
	}

	CalcValue(const StrStack& codeBlock): type(BLK)
		{ block = new StrStack(codeBlock); }

	CalcValue(const StrStack* codeBlock): type(BLK)
		{ block = new StrStack(*codeBlock); }

	CalcValue(const std::vector<CalcValue>& in_list):
		type(ARR), list(new std::vector<CalcValue>(in_list))
	{}

	CalcValue(const std::vector<std::string>& in_list):
		type(REQ), request(new std::vector<std::string>(in_list))
	{}

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
		else if (type == OBJ)
			object = new UserType(*in.object);
		else if (type == REQ)
			request = new std::vector<std::string>(*in.request);

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
		else if (type == REQ)
			request = new std::vector<std::string>(*in.request);
		else if (type == OBJ)
			object = new UserType(*in.object);
	}

	void setValue(const UserType in) {
		clear();
		object = new UserType(in);
		type = OBJ;
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
		list = new std::vector<CalcValue>(arr);
	}

	void setValue(const std::vector<std::string>& req) {
		// del old val
		clear();

		type = REQ;
		request = new std::vector<std::string>(req);

	}

	void setValue(const bool in)
		{ setValue(in ? 1.0 : 0.0); }


	void setValue(const Lambda& lam) {
		// delete old value
		clear();

		type = LAM;
		lambda = new Lambda(lam);
	}


	CalcValue& setRef(const char* const str){

		// memory leaks are pretty bad
		clear();

		// set type
		type = REF;

		// create buffer
		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);


		return *this;
	}

	double getNum(){
		if (type == NUM)
			return number;
		else if (type == STR) // short-circuit logic prevents segfault
			return string && strlen(string);
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

	CalcValue* valAtRef(std::vector<UserVar>& vars){
		if (type == REF)
			return vars::valueAtVar(vars, string);
		else
			return this;
	}

	CalcValue* valAtRef(UserVar* first){
		if (type == REF)
			return vars::valueAtVar(first, string);
		else
			return this;
	}


	bool operator==(const CalcValue& cv2);

	bool operator!=(const CalcValue& vs) {
		return !operator==(vs);
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
	bool isObj()
		{ return type == OBJ; }

	// return a string representation of the variable
	std::string toString(std::vector<UserVar>& var_nodes);

	// string form of type
	const char* typeName();


	// list accessor
	CalcValue* getListElem(const std::vector<ssize_t>& elem_index);

	// list modifiers
	bool assignElem(const std::vector<ssize_t>& elem_index, const CalcValue& val);
	bool deleteListElem(const std::vector<ssize_t>& elem_index);


	// get the member of an object
	CalcValue* requestMember(std::vector<UserVar>& var_nodes);
	CalcValue* requestMember(UserVar* first_node);
	CalcValue* requestMember(std::vector<std::string>& req, std::vector<UserVar>* var_nodes);

};



#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL)



#endif