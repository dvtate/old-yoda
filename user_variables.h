#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdlib>
#include <cstring>

#include "calc_value.h"

#define USERVAR_NAME_MAXLENGHT 20



// the node for my linked list of user defined variables
class UserVar {
public:
	char name[USERVAR_NAME_MAXLENGHT];

  	enum type {NUM, STR, PTR} valType;

	union {
		double number;
		char* string;
		void* pointer;
	};

	UserVar *next;

	UserVar(const char* identifier, double contents):
		number(contents)
	{
		next = (UserVar*) NULL;

	  	strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);
	  	valType = NUM;
	}

	UserVar(const char* identifier, double contents, UserVar* next_node):
		number(contents)
	{
		next = next_node;

	  	strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);
		valType = NUM;
	}

	UserVar(const char* identifier, CalcValue contents){

	  	next = (UserVar*) NULL;

	  	strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);

	  	if (contents.type == CalcValue::NUM) {
			valType = NUM;
		  	number = contents.number;
		} else {
			valType = STR;
	  	  	string = (char*) malloc(strlen(contents.string) + 1);
		  	strcpy(string, contents.string);
		}
	}



	// geting the values
	double getNumber(){
		if (valType == NUM)
			return number;
	  	else
			return 0;
	}
	char* getString(){
	  	if (valType == STR)
			return string;
	  	else
		  	return (char*) NULL; // beware of segfaults...
	}
	void* getPointer(){
	  	if (valType == PTR)
		  	return pointer;
	  	else
		  	return (void*) NULL;
	}

	CalcValue getValue(){
		if (valType == NUM)
			return CalcValue(number);

	  	return CalcValue(getString());

	}

	// changing the values
	void setValue(double val){
		number = val;
	  	valType = NUM;
	}
	void setValue(const char* val){
	  	strcpy(string, val);
	  	valType = STR;
	}
	void setValue(void* val){
	  	pointer = val;
		valType = PTR;
	}
	void setValue(CalcValue val){
	  	if (val.type == CalcValue::NUM) {
		  	if (valType == STR && string != NULL)
				free(string);
			valType = NUM;
		  	number = val.number;
		} else {
			if (valType == STR && string != NULL)
			  	free(string);

			valType = STR;
		  	string = (char*) malloc(strlen(val.string) + 1);
			string = val.string;

		}
	}

};




namespace vars {
	
	extern UserVar* first_node;

  	// the last element on the linked list
  	extern UserVar* lastVar(UserVar* first);

	// to be called after reset
	extern void wipeAll(UserVar*& first);

	// to make a  new variable, or change it's value
	//extern void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT], double value);
	extern void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT], CalcValue value);

	// to remove an individial variable
	extern void removeVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);

	// returns a pointer to the variable
	extern UserVar* findVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);

	extern bool varExists(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);

}



#endif
