
#ifndef YODA_USER_DEFS_HPP
#define YODA_USER_DEFS_HPP

#include <stack>
#include <vector>

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
		else if (cond_type == LABEL)
			label = in.label;
		else
			adv_cond = in.adv_cond;

	}

	~UserDef(){}


	// what actually happens, the code run when operator is triggered
	union {
		char* (*method)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
		                bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
		                std::vector<void*>& freeable);

		StrStack macro;
	};
	// using C++ or YodaScript to perform the operation
	enum { FXNPTR, MACRO } proc_type;


	// two condition types
	// what triggers the operator
	union {
		bool (*adv_cond)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
		                       bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
		                       std::vector<void*>& freeable);
		bool (*bas_cond)(char* p);

		std::string label;
	};
	// are they usng a basic or advanced condition?
	enum { ADVANCED, BASIC, LABEL } cond_type;


	void setProc(char* (*_method)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                 std::vector<void*>& freeable))
	{
		method = _method;
		proc_type = FXNPTR;
	};

	void setProc(const StrStack _macro) {
		macro = _macro;
		proc_type = MACRO;
	}

	void setCond(bool (*condition)(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                               bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                               std::vector<void*>& freeable)
	) {
		adv_cond = condition;
		cond_type = ADVANCED;
	}

	void setCond(bool (*condition)(char* p)) {
		bas_cond = condition;
		cond_type = BASIC;
	}

	void setCond(std::string _label) {
		label.assign(_label);
		cond_type = LABEL;
	}

	void setCond(const char* _label) {
		label.assign(_label);
		cond_type = LABEL;
	}

	// check the condition
	bool cond(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	          bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	          std::vector<void*>& freeable)
	{
		if (cond_type == BASIC)
			return bas_cond(p);
		else if (cond_type == LABEL)
			return label == p;
		else
			return adv_cond(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable);
	}

};



namespace udefs {

	extern std::vector<UserDef> userDefs;

	char *callOperator(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                   bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                   std::vector<void*>& freeable, bool& ret);

};
#endif //YODA_USER_DEFS_HPP
