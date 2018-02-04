
#ifndef YODA_USER_DEFS_HPP
#define YODA_USER_DEFS_HPP

#include <stack>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "calc_value.hpp"
#include "core.hpp"
#include "process_line.hpp"



class UserDef {
public:

	UserDef(){}
	UserDef(const UserDef& in) {
		proc_type = in.proc_type;
		cond_type = in.cond_type;

		if (proc_type == FXNPTR)
			method = in.method;
		else
			macro = in.macro;

		if (cond_type == BASIC)
			bas_cond = in.bas_cond;
		else if (cond_type == LABEL) {
			label = (char*) malloc(strlen(in.label) + 1);
			strcpy(label, in.label);
		} else
			adv_cond = in.adv_cond;
	}

	~UserDef(){
		if (cond_type == LABEL)
			free(label);
	}


	// what actually happens, the code run when operator is triggered
	union {
		char* (*method)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
		                bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
		                std::vector<void*>& freeable);

		StrStack macro;
	};
	// using C++ or YodaScript to perform the operation
	enum { FXNPTR, MACRO } proc_type;





	// 3 condition types
	// what triggers the operator
	union {
		bool (*adv_cond)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
		                       bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
		                       std::vector<void*>& freeable);
		bool (*bas_cond)(char* p);

		char* label;
	};
	// are they usng a basic or advanced condition?
	enum { ADVANCED, BASIC, LABEL } cond_type;



	UserDef& operator=(const UserDef& in){

		// no mem leaks here :)
		if (cond_type == LABEL)
			free(label);


		proc_type = in.proc_type;
		cond_type = in.cond_type;

		// copy
		if (proc_type == FXNPTR)
			method = in.method;
		else
			macro = in.macro;

		// copy condition in
		if (cond_type == BASIC)
			bas_cond = in.bas_cond;
		else if (cond_type == LABEL) {
			label = (char*) malloc(strlen(in.label) + 1);
			strcpy(label, in.label);
		} else
			adv_cond = in.adv_cond;

		return *this;
	}


	// assign process
	UserDef& setProc(char* (*_method)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                 std::vector<void*>& freeable))
	{
		method = _method;
		proc_type = FXNPTR;
		return *this;
	};

	UserDef& setProc(const StrStack& _macro) {
		macro = _macro;
		proc_type = MACRO;
		return *this;
	}



	// assign condition
	UserDef& setCond(bool (*condition)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                               bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                               std::vector<void*>& freeable)
	) {
		// prevent mem leak
		if (cond_type == LABEL)
			free(label);

		adv_cond = condition;
		cond_type = ADVANCED;
		return *this;
	}
	UserDef& setCond(bool (*condition)(char* p)) {
		// prevent mem leak
		if (cond_type == LABEL)
			free(label);

		bas_cond = condition;
		cond_type = BASIC;
		return *this;
	}
	UserDef& setCond(const char* _label) {
		// prevent mem leak
		if (cond_type == LABEL)
			free(label);

		label = (char*) malloc(strlen(_label) + 1);
		strcpy(label, _label);
		cond_type = LABEL;

		return *this;
	}


	// check the condition
	bool cond(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	          bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	          std::vector<void*>& freeable)
	{
		if (cond_type == BASIC)
			return bas_cond(p);
		else if (cond_type == LABEL) {
			return strcmp(label, p) == 0;
		} else
			return adv_cond(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable);
	}

};



namespace udefs {

	// list of defined operators
	extern std::vector<UserDef> userDefs;

	// attempt call an operator
	char* callOperator(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                   bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                   std::vector<void*>& freeable, bool& ret);

	// find index of appropriate operator
	int findOperator(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                 std::vector<void*>& freeable);

};


#endif //YODA_USER_DEFS_HPP
