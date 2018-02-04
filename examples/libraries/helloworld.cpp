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
}

// fill vector with operators to export
std::vector<UserDef> newOps = { UserDef().setCond("hello").setProc(method) };

// export operators
extern "C" {
	void* ops_export = (void*) &newOps;
}
