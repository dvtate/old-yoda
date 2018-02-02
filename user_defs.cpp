#include "string_stack.hpp"
#include "core.hpp"
#include "user_defs.hpp"

// this will store the userdefined operators
std::vector<UserDef> userDefs;

int makeoperator() {
	UserDef helloFxn;
	StrStack body;
	body.push("\"Hi\" println");
	helloFxn.setProc(body);
	helloFxn.setCond([](char* p){ return *p == '`'; });

	userDefs.push_back(helloFxn);
}

int ret = makeoperator();



namespace udefs {

	char *callOperator(char *p, std::stack<CalcValue> &mainStack, std::vector<UserVar> &var_nodes,
	                   bool &showErrors, char *&rpnln, bool &elseStatement, FILE *codeFeed,
	                   std::vector<void *> &freeable, bool& ret)
	{

		// check each operator to see if it it's condition is met
		for (auto op : userDefs) {
			if (op.cond(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable)) {

				// we found a match
				ret = true;

				// if it's a macro, run it
				if (op.proc_type == UserDef::MACRO) {
					StrStack macro_cpy = op.macro;
					return runMacroKeepScope(&macro_cpy, mainStack, var_nodes, freeable, showErrors, elseStatement);
				} else
					return op.method(p, mainStack, var_nodes, showErrors, rpnln, elseStatement, codeFeed, freeable);



			}
		}



	}

};
