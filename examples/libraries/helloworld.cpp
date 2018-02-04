#include "/home/tate/CLionProjects/yoda/user_defs.hpp"


char* method(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
                 std::vector<void*>& freeable) 
{
	std::cout <<"o shit wuddup\n";
}


std::vector<UserDef> newOps = { UserDef().setCond("hello").setProc(method) };


extern "C" {
	void* ops_export = (void*) &newOps;
}
