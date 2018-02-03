#include "string_stack.hpp"
#include "core.hpp"
#include "user_defs.hpp"



namespace udefs {


	// this will store the user-defined operators
	std::vector<UserDef> userDefs;



	// searches userDefs for correct operator
	//  if one is found, call it
	//  if one isn't, set bool& ret to true

	char* callOperator(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                   bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                   std::vector<void*>& freeable, bool& ret)
	{

		// check each operator to see if it it's condition is met
		for (auto op : userDefs) {
			if (op.cond(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable)) {

				// we found a match
				ret = true;

				// run the code
				if (op.proc_type == UserDef::MACRO) {
					StrStack macro_cpy = op.macro;
					return runMacroKeepScope(&macro_cpy, mainStack, var_nodes, freeable, showErrors, elseStatement);
				} else
					return op.method(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable);

			}
		}

		// we didn't find one...
		ret = false;
		return NULL;


	}

	// returns index of first element which whos cond() returns true.
	int findOperator(char* p, std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes,
	                 bool& showErrors, char*& rpnln, bool& elseStatement, FILE* codeFeed,
	                 std::vector<void*>& freeable)
	{
		for (size_t i = 0; i < userDefs.size(); i++)
			if (userDefs[i].cond(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable))
				return i;

		return -1;

	}

};
