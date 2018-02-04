#include "user_defs.hpp"


char* method(	char* p, 				// current token/operator
		std::stack<CalcValue>& mainStack,	// stack (values before operator)
		std::vector<UserVar>& var_nodes,	// variables/scopes
		bool& showErrors,			// should errors be printed?
		char*& rpnln,				// full line which called operator
		bool& elseStatement,			// is there an else statement before us?
		FILE* codeFeed,				// where we should get more code if needed
		std::vector<void*>& freeable		// garbage collector (cleared at end of scope)
) {
	std::cout <<"hi there\n";
	return NULL; // no errors
}

// this vector stores all of our operators we want to export
// this variable must be called "ops_export"
std::vector<UserDef> ops_export = {
	UserDef().setCond("hello").setProc(method) 
};


// note: more advanced conditions can also be used
// note: you can also use `StrStack` (internal class for macros) instead of functions to run yodascript code
