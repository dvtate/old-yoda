#include <iostream>
#include <string.h>

#include "calc_value.hpp"

#include "user_variables.hpp"



namespace vars {

	// return the last variable on the list
	UserVar* lastVar(UserVar* first) {
		while (first->next != NULL)
			first = first->next;

		return first;
	}

	// delete all variables
	void wipeAll(UserVar* first) {

		UserVar *var1, *var2;

		var1 = first->next;

		// leap-frog and delete
		while (var1) {
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

	// add a variable to the linked list
	UserVar* assignVar(UserVar* first, const char* name, CalcValue value) {
		UserVar* var;
		if (*name == ' ')
			goto make_new_var;

		var = findVar(first, name);

		// making a new variable
		if (var == NULL) {
			make_new_var:
			var = new UserVar(first, name, value);
			lastVar(first)->next = var;

			// changing the variable's value
		} else
			var->setValue(value);

		return var;

	}


	UserVar* assignNewVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH], CalcValue value){
		// making a new variable
		UserVar* var = new UserVar(first, name, value);
		lastVar(first)->next = var;
		return var;
	}


	// remove a variable from the linked list
	void removeVar(UserVar* first, const char* name) {
		if (!name || !first)
			return;

		// search the linked list for the object
		while (first->next && first) {
			if (strcmp(first->next->name, name) == 0) {

				// get object address so it doesn't become unaccessable
				UserVar *toBeDeleted = first->next;

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


	// returns pointer to given variable
	UserVar* findVar(UserVar* first, const char* name) {

		if (!name || !first)
			return NULL;

		first = first->next;

		// search the linked list for the object
		while (first != NULL)
			if (strcmp(first->name, name) == 0)
				return first;
			else
				first = first->next;


		return NULL;
	}

	UserVar* findVar(std::vector<UserVar>& vars, const char* name) {
		for (int i = vars.size() - 1; i >= 0; i--) {
			UserVar *ret = findVar(&vars[i], name);
			if (ret)
				return ret;
		}

		return NULL;

	}


	// returns whether variable is in list or not
	bool varExists(UserVar* first, const char* name) {

		first = first->next;

		while (first)
			if (strcmp(first->name, name) == 0)
				return true;
			else
				first = first->next;

		return false;
	}

	bool varExists(std::vector<UserVar>& vars, const char* name) {
		for (ssize_t i = vars.size() - 1; i >= 0; i--)
			if (varExists(&vars[i], name))
				return true;
		return false;

	}


	// de-references the variable's
	CalcValue* valueAtVar(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]) {
		UserVar* var = findVar(first, name);

		return var ?
			var->val.type == CalcValue::REF ?
				valueAtVar(first, var->val.string)
				: &var->val
			: NULL;
	}

	CalcValue* valueAtVar(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]) {
		UserVar* var = findVar(vars, name);

		return var ?
			var->val.type == CalcValue::REF ?
				valueAtVar(vars, var->val.string)
				: &var->val
			: NULL;
	}

	// finds the last var in a reference chain
	UserVar* lastVarInRefChain(UserVar* first, const char name[USERVAR_NAME_MAXLENGTH]) {
		UserVar* var = findVar(first, name);

		return var ? // var defined ?
			var->val.type == CalcValue::REF ? // is it a ref ?
				lastVarInRefChain(first, var->val.string)
				: var
			: NULL;
	}

	UserVar* lastVarInRefChain(std::vector<UserVar>& vars, const char name[USERVAR_NAME_MAXLENGTH]) {
		UserVar* var = findVar(vars, name);

		return var ?
			var->val.type == CalcValue::REF ?
				lastVarInRefChain(vars, var->val.string) // recursion to find ref-dest
				: var // var found and isn't a ref
			: NULL; // not defined
	}


	void clearScope(std::vector<UserVar>& scopes){
		wipeAll(&scopes[scopes.size() - 1]);
		scopes.pop_back();
	}
}
