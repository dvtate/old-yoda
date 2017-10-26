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


	CalcValue (double val):	type(NUM) {
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

	CalcValue(StrStack* codeBlock): type(BLK)
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


	bool operator==(const CalcValue& cv2) {

		// same type
		if (type == cv2.type) {

			// same value
			if (type == NUM && number == cv2.number)
				return true;
			if (type == STR && isEmpty() != !cv2.string)
				return false;
			if (type == STR && string == cv2.string)
				return true;
			else if (type == STR || cv2.type == CalcValue::REF)
				return strcmp(string, cv2.string) == 0;
			// this doesnt work...
			else if (type == CalcValue::ARR) {
				if (list->size() != cv2.list->size())
					return false;
				for (size_t i = 0; i < list->size() - 1; i++)
					if (!((*list)[i] == (*cv2.list)[i]))
						return false;

				return true;
			} else if (type == INX)
				return (index == cv2.index);
			else if (type == OBJ) {
				// TODO: implement this
				return false;
			}
		}

		return false;


	}

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


	// list accessor
	CalcValue* getListElem(const std::vector<ssize_t>& elem_index) {
		if (type != ARR)
			return NULL;

		CalcValue* ret = this;

		for (int16_t i = elem_index.size() - 1; i >= 0; i--) {
			if ((ssize_t) ret->list->size() <= elem_index[i] || (ssize_t) ret->list->size() < abs(elem_index[i])) {
				return NULL;
			}
			ret = elem_index[i] < 0 ?
			      &ret->list->at(ret->list->size() + elem_index[i]) :
			      &ret->list->at(elem_index[i]);
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
		for (int16_t i = elem_index.size() - 1; i >= 0; i--) {
			if ((ssize_t) ret->list->size() <= elem_index[i] || (ssize_t) ret->list->size() < abs(elem_index[i]))
				return false;

			ret = elem_index[i] < 0 ?
			      &ret->list->at(ret->list->size() + elem_index[i]) :
			      &ret->list->at(elem_index[i]);
		}
		if (!ret || !ret->list)
			return false;

		ret->list->erase(ret->list->begin() + elem_index[elem_index.size() - 1]);
		return true;

	}

	std::string toString(std::vector<UserVar>& var_nodes){

		std::string ret;

		// null
		if (isNull())
			return "null";

		// macro
		else if (type == CalcValue::BLK) {
			size_t len = 50;
			char* str = (char*) malloc(len);
			block->toString(&str, &len);
			ret = str;
			free(str);
			return ret;

		// number
		} else if (type == CalcValue::NUM) {
			char str[27];
			snprintf(str, 26, "%*.*g", 10, 16, getNum());
			return strutils::trimStr(str);

			// already a string
		} else if (type == CalcValue::STR)
			return string;

			// a variable
		else if (type == CalcValue::REF) {
			CalcValue* val = valAtRef(var_nodes);
			while (val && val->type == CalcValue::REF)
				val = val->valAtRef(var_nodes);
			if (val)
				return val->toString(var_nodes);
			else
				return "reference_error";

		// list printing a list
		} else if (type == CalcValue::ARR) {
			ret += "(";
			if (list->size()) {
				if (list->at(0).type == CalcValue::STR)
					ret += '"';
				ret += list->at(0).toString(var_nodes);
				if (list->at(0).type == CalcValue::STR)
					ret += '"';
				for (size_t i = 1; i < list->size(); i++) {
					ret += ", ";
					if (list->at(i).type == CalcValue::STR)
						ret += '"';
					ret += list->at(i).toString(var_nodes);
					if (list->at(i).type == CalcValue::STR)
						ret += '"';
				}
			}
			ret+= " )";
		} else if (type == CalcValue::OBJ) {
			return "<CV::OBJ.toString not implemented yet>";
		} else if (type == CalcValue::REQ) {
			return "<CV::REQ.toString not implemented yet>";
		}

		return ret;

	}

	CalcValue* requestMember(std::vector<UserVar>& var_nodes) {
		if (type != REQ && request->at(0) == " ")
			return NULL;

		CalcValue* ret = CalcValue()
							.setRef(request->at(0).c_str())
								.valAtRef(var_nodes);
		if (!ret)
			return NULL;

		for (uint16_t i = 1; i < request->size(); i++) {
			if (ret->type != CalcValue::OBJ)
				return NULL;

			// if the object doesn't have a member with that name
			if (! (ret = ret->object->getMember(request->at(i))))
				return NULL;

		}
		return ret;

	}

	CalcValue* requestMember(UserVar* first_node) {
		if (type != REQ || request->at(0) == " ")
			return NULL;

		CalcValue* ret = CalcValue()
				.setRef(request->at(0).c_str())
				.valAtRef(first_node);
		if (!ret)
			return NULL;

		for (uint16_t i = 1; i < request->size(); i++) {
			if (ret->type != CalcValue::OBJ)
				return NULL;

			// if the object doesn't have a member with that name
			if (! (ret = ret->object->getMember(request->at(i))))
				return NULL;

		}
		return ret;

	}

	CalcValue* requestMember(std::vector<std::string>& req, std::vector<UserVar>* var_nodes = NULL) {
		if (req.at(0) != " ") {
			return NULL;
		}


		CalcValue* ret = this;
		//printf("reqMem:%s\n", ret->toString(*var_nodes).c_str());

		for (uint16_t i = 1; i < req.size(); i++) {

			//printf("reqMem:%s\n", ret->toString(*var_nodes).c_str());

			// request expected an object where there is none...
			if (ret->type != CalcValue::OBJ)
				return NULL;

			// if the object doesn't have a member with that name
			if (!(ret = ret->object->getMember(req[i])))
				return NULL;

		}

		return ret;

	}

	// string form of type
	const char* typeName() {
		if (isNull()) // NULL string pointer
			return "NULL_VAL";

		else if (type == CalcValue::STR) // string-type
			return "string"; // STR

		else if (type == CalcValue::NUM) // number-type
			return "number/boolean"; // NUM/BLN

		else if (type == CalcValue::REF) // variable reference
			return "reference";

		else if (type == CalcValue::BLK) // string_stack
			return "macro";

		else if (type == CalcValue::ARR) // list
			return "list";

		else if (type == CalcValue::LAM) // lambda
			return "lambda";

		else if (type == CalcValue::OBJ) // object
			return "object";

		else if (type == CalcValue::REQ) // member request type
			return "memeber";

		else if (type == CalcValue::INX) // list index type
			return "index";

		return "WTF";
	}
};



#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL)



#endif