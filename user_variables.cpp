#include <iostream>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>

#include "calc_value.hpp"

#include "user_variables.hpp"



namespace vars {


	UserVar* lastVar(UserVar* first){
		while (first->next != NULL)
			first = first->next;

		return first;
	}


	void wipeAll(UserVar*& first){

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

		first->next = NULL;

	}

	void assignVar(UserVar* first, char* name, CalcValue value){

		UserVar* var = findVar(first, name);

		// making a new variable
		if (var == NULL) {
			var = new UserVar(first, name, value);
			lastVar(first)->next = var;

			// changing the variable's value
		} else
			var->setValue(value);

	}

	// this is broken...
	void removeVar(UserVar* first, const char* name){
		if (!name || !first)
			return;

		// search the linked list for the object
		while (first->next && first) {
			if (strcmp(first->next->name, name) == 0) {

				// get object address so it doesn't become unaccessable
				UserVar* toBeDeleted = first->next;

				// replace link
				first->next = first->next->next;

				// remove the node
				delete toBeDeleted;

				// we done
				return;

			} else // keep looking for it
				first = first->next;
		}
	}

	UserVar* findVar(UserVar* first, char* name){

		if (!name || !first)
			return NULL;

		first = first->next;

		uint8_t len = strlen(name) + 1;

		// search the linked list for the object
		while (first != NULL)
			if (strncmp(first->name, name, len) == 0)
				return first;
			else
				first = first->next;


		return (UserVar*) NULL;
	}


	bool varExists(UserVar* first, char* name){

		first = first->next;

		while (first != NULL)
			if (strcmp(first->name, name) == 0)
				return true;
			else
				first = first->next;

		return false;
	}

	// de-references the variable's
	CalcValue* valueAtVar(UserVar* first, char name[USERVAR_NAME_MAXLENGTH]){
		UserVar* var = findVar(first, name);

		if (var) {
			CalcValue* val = &var->val;
			if (val->type == CalcValue::REF) {
				return valueAtVar(first, val->string);

			} else
				return val;
		} else
			return NULL;

	}

}
