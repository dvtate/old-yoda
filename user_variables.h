#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdlib>
#include <cstring>

#include "calc_value.h"

// variable names longer than 20chars will be called by their first 19 chars
#define USERVAR_NAME_MAXLENGHT 20



// the node for my linked list of user defined variables
class UserVar {
public:
	char name[USERVAR_NAME_MAXLENGHT];

	CalcValue val;

	UserVar *next;

	UserVar(const char* identifier, double contents):
		val(contents)
	{
		next = (UserVar*) NULL;

	  	strncpy(name, identifier, USERVAR_NAME_MAXLENGHT - 1);
	  	val.type = CalcValue::NUM;
	}

	UserVar(const char* identifier, CalcValue contents){

	  	next = (UserVar*) NULL;

	  	strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);

	  	if (contents.type == CalcValue::NUM) {
			val.type = CalcValue::NUM;
		  	val.number = contents.number;
		} else {
			val.type = CalcValue::STR;
	  	  	val.string = (char*) malloc(strlen(contents.string) + 1);
		  	strcpy(val.string, contents.string);
		}
	}


	// geting the values
	double getNumber()
		{ return val.getNum(); }

	char* getString()
		{ return val.getStr(); }

	CalcValue& getValue()
		{ return val; }


	// changing the values
	void setValue(double in){

		val.number = in;
	  	val.type = CalcValue::NUM;
	}

	void setValue(const char* in){
	  	if (val.type == CalcValue::STR && val.string != NULL)
			  	free(val.string);

	  	val.string = (char*) malloc(strlen(in) + 1);
	  	strcpy(val.string, in);

	  	val.type = CalcValue::STR;
	}

	void setValue(CalcValue in){
	  	if (in.type == CalcValue::NUM) {
		  	if (val.type == CalcValue::STR && val.string != NULL)
				free(val.string);
			val.type = CalcValue::NUM;
		  	val.number = in.number;
		} else {
			if (val.type == CalcValue::STR && val.string != NULL)
			  	free(val.string);

			val.type = CalcValue::STR;
		  	val.string = (char*) malloc(strlen(in.string) + 1);
			strcpy(val.string, in.string);

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
