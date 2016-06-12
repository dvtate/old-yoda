#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdlib>
#include <cstring>

#define USERVAR_NAME_MAXLENGHT 20



// the node for my linked list of user defined variables
class UserVar {
public:
	char name[USERVAR_NAME_MAXLENGHT];
	long double value;

	UserVar *next;

	UserVar(const char* identifier, long double contents):
		value(contents)
	{
		next = (UserVar*) NULL;
		strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);
	}

	UserVar(const char* identifier, long double contents, UserVar* next_node):
		value(contents)
	{
		next = next_node;
		strncpy(name, identifier, USERVAR_NAME_MAXLENGHT);
	}

	long double& getValue()
		{ return value; }

	void setvalue(long double val)
		{ value = val; }

};


namespace vars {

	extern UserVar* first_node;

  	// the last element on the linked list
  	extern UserVar* lastVar(UserVar* first);

	// to be called after reset
	extern void wipeAll(UserVar* first);

	// to make a  new variable, or change it's value
	extern void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT], long double value);

	// to remove an individial variable
	extern void removeVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);

	// returns a pointer to the variable
	extern UserVar* findVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);

	extern bool varExists(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]);


}



#endif
