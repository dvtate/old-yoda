#include <cstdlib>
#include <cstring>

#include "calc_value.h"

#include "user_variables.h"

namespace vars {

	UserVar* first_node = new UserVar(" ", 0);


	UserVar* lastVar(UserVar* first){
		while (first->next != NULL)
			first++;

		return first;
	}


	void wipeAll(UserVar* first){

		UserVar* var1, * var2;

		var1 = first->next;

		// leap-frog and delete
		while (var1 != NULL) {
			var2 = var1->next;

			delete var1;
			var1 = NULL;

			if (var2 != NULL) {
				var1 = var2->next;
				delete var2;
			}
		}

	}

	void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT], long double value){

		UserVar* var = findVar(first, name);

		// making a new variable
		if (var == NULL) {
			var = new UserVar(name, value);
			lastVar(first)->next = var;

		} else // changing the value
			var->setValue(value);

	}

	extern void assignVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT], CalcValue value){

		UserVar* var = findVar(first, name);

		// making a new variable
		if (var == NULL) {
			var = new UserVar(name, value);
			lastVar(first)->next = var;
		} else {
			var->setValue(value);
		}

	}

	void removeVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]){
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

	UserVar* findVar(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]){

		UserVar* var = first->next;
		// search the linked list for the object
		while (var != NULL)
			if (strcmp(var->name, name) == 0)
				return var;
			else
				var = var->next;

		return (UserVar*) NULL;
	}

	bool varExists(UserVar* first, char name[USERVAR_NAME_MAXLENGHT]){
		UserVar* var = first->next;

		while (var != NULL)
			if (strcmp(var->name, name) == 0)
				return true;
			else
				var = var->next;

		return false;
	}

}
