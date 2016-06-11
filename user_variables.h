#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdlib>

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

	extern UserVar* first;

	// to be called after reset
	void wipeAll(void);

	// to make a  new variable, or change it's value
	void assignVar(char name[USERVAR_NAME_MAXLENGHT], long double value);

	// to remove an individial variable
	void removeVar(char name[USERVAR_NAME_MAXLENGHT]);

	// returns a pointer to the variable
	UserVar* findVar(char name[USERVAR_NAME_MAXLENGHT]);

	bool varExists(char name[USERVAR_NAME_MAXLENGHT]);


}



#endif
