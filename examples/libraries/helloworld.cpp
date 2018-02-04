#include "user_defs.hpp"


char* method(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                 std::vector<void*>& freeable) 
{
	std::cout <<"o shit wuddup\n";
}


UserDef makeSayHello() {

	UserDef sayHello;

	sayHello.setCond("hello");
	sayHello.setProc(method);
	sayHello.cond_type = UserDef::LABEL;	
	sayHello.proc_type = UserDef::FXNPTR;
	
	return sayHello;
}

std::vector<UserDef> newOps = { makeSayHello() };


extern "C" {
	void* ops_export = (void*) &newOps;
}
