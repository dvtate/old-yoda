
#include <cstdlib>
#include <cstring>
#include "user_variables.h"


namespace vars {

	UserVar* first = new UserVar(" ", 0, NULL);
	UserVar* last = first;


	void wipeAll(void){
		
	}

	void assignVar(char name[USERVAR_NAME_MAXLENGHT], long double value){
		UserVar* var = findVar(name);

		// making a new variable		
		if (var == NULL) {
			var = new UserVar(name, value, NULL);
			last->next = var; 

		} else // changing the value
			var->value = value;

	}

	void removeVar(char name[USERVAR_NAME_MAXLENGHT]){
		UserVar* var = first;

		// search the linked list for the object
		while (var != NULL)
			if (strcmp(var->next->name, name) == 0) {

				// get object address so it doesn't become unaccessable
				UserVar* toBeDeleted = var->next;
				
				// replace link
				var->next = var->next->next;
				
				// remove the link
				delete toBeDeleted;

			} else // keep looking for it
				var = var->next;
	}

	UserVar* findVar(char name[USERVAR_NAME_MAXLENGHT]){

		UserVar* var = first->next;;
		// search the linked list for the object
		while (var != NULL)
			if (strcmp(var->name, name) == 0)
				return var;
			else 
				var = var->next;

		return (UserVar*) NULL;
	}

	bool varExists(char name[USERVAR_NAME_MAXLENGHT]){
		UserVar* var = first->next;;

		while (var != NULL)
			if (strcmp(var->name, name) == 0)
				return true;
			else 
				var = var->next;

		return false;
	}

}
