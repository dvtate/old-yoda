#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <stdlib.h>
#include <string.h>
#include <vector>

#include "calc_value.hpp"

//USERVAR_NAME_MAXLENGTH = 20
// variable names longer than 20chars will be called by their first 19 chars

// the node for my linked list of user defined variables
class UserVar {
public:

	char name[USERVAR_NAME_MAXLENGTH]; // USERVAR_NAME_MAXLENGTH defined in calc_value.h

	CalcValue val;

	UserVar *first, *next;

	UserVar(UserVar* firstn, const char* const identifier, double contents):
			val(contents), first(firstn), next((UserVar*) NULL)
	{
		strncpy(name, identifier, USERVAR_NAME_MAXLENGTH - 1);
		val.type = CalcValue::NUM;
	}

	UserVar(UserVar* firstn, const char* const identifier, const char* const contents):
			val(contents), first(firstn), next((UserVar*) NULL)
	{
		strncpy(name, identifier, USERVAR_NAME_MAXLENGTH - 1);
		val.type = CalcValue::NUM;
	}

	UserVar(UserVar* firstn, const char* const identifier, CalcValue contents):
			val(contents), first(firstn), next((UserVar*) NULL)
	{
		strncpy(name, identifier, USERVAR_NAME_MAXLENGTH - 1);
	}


	// geting the values
	double getNumber()
	{ return val.getNum(); }

	char* getString()
	{ return val.getStr(); }

	// recursive method of accessing Values of CalcValue::REF type
	CalcValue getValue(){

		if (val.type == CalcValue::REF) {
			CalcValue* value = val.valAtRef(first);

			while (value != NULL && value->type == CalcValue::REF)
				value = value->valAtRef(first);

			// broken reference (should I tell them???)
			return value ? *value : NULL_CALCVAL_OBJECT;

		}

		return val;

	}

	// recursive method of accessing Values of CalcValue::REF type
	CalcValue* getValPtr(){

		if (val.type == CalcValue::REF) {
			CalcValue* value = val.valAtRef(first);

			while (value != NULL && value->type == CalcValue::REF)
				value = value->valAtRef(first);

			return value;
		}

		return &val;

	}

	CalcValue& getVal()
	{ return val; }


	// changing the values

	void setValue(const char* in)
	{
		CalcValue* value = getValPtr();
		value->setValue(in);
	}

	void setValue(const double in)
	{ val.setValue(in); }

	void setValue(const CalcValue in)
	{ val.setValue(in); }

	void setValue(UserVar* in)
	{ val = CalcValue().setRef(in->name); }


};


namespace vars {

	// the last element on the linked list
	extern UserVar* lastVar(UserVar* first);

	// to be called after reset
	extern void wipeAll(UserVar* first);

	// to make a  new variable, or change it's value
	//extern void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGTH], double value);
	extern UserVar* assignVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH], CalcValue value);
	inline UserVar* assignVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH], CalcValue value){
		return assignVar(&vars[vars.size() - 1], name, value);
	}
	extern UserVar* assignNewVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH], CalcValue value);
	inline UserVar* assignNewVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH], CalcValue value){
		return assignNewVar(&vars[vars.size() - 1], name, value);
	}

	// to remove an individial variable
	extern void removeVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);

	// returns a pointer to the variable
	extern UserVar* findVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* findVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);

	// has the variable been declared yet?
	extern bool varExists(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern bool varExists(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);

	// de-references a value
	extern CalcValue* valueAtVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern CalcValue* valueAtVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]);

	// last variable in a chain of variables linking eachother
	// similar to valueAtVar() but returns var instead of val
	extern UserVar* lastVarInRefChain(UserVar *first, const char name[USERVAR_NAME_MAXLENGTH]);
	extern UserVar* lastVarInRefChain(std::vector<UserVar> &vars, const char name[USERVAR_NAME_MAXLENGTH]);

	extern void clearScope(std::vector<UserVar>& scopes);
}



#endif