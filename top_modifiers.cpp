#include "top_modifiers.hpp"


// recursively evaluate the top value
CalcValue* conv_top(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, const bool showErrors, std::vector<void*>& freeable){
	CalcValue* p = NULL, * ret = NULL;

	if (mainStack.empty())
		return NULL;

	//printf("conv: %s\n", mainStack.top().typeName());


	// variable reference
	if (mainStack.top().type == CalcValue::REF) {
		ret = mainStack.top().valAtRef(var_nodes);
		if (!ret) {
			PASS_ERROR("\aERROR: broken reference to $" <<mainStack.top().string <<std::endl);
		}
		mainStack.pop();
		return ret;
	}
	if (mainStack.top().type == CalcValue::REQ && mainStack.top().object->members.at(0) != " "){
		ret = mainStack.top().requestMember(var_nodes);
		if (!ret) {
			PASS_ERROR("\aERROR: object member `" <<mainStack.top().toString(var_nodes) <<"` not found\n");
		}
		mainStack.pop();
		return ret;
	}

	return conv_top_keep_refs(mainStack, var_nodes, showErrors, freeable);
}

CalcValue* get_top(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, bool showErrors, std::vector<void*>& freeable){
	CalcValue* p = NULL, *ret = NULL;


	if (mainStack.empty())
		return NULL;

	if (mainStack.top().type == CalcValue::REF) {
		ret = mainStack.top().valAtRef(var_nodes);
		// if the variable doesn't exist, assign it to null
		if (!ret)
			ret = &vars::assignNewVar(var_nodes, mainStack.top().string, CalcValue())->val;

		mainStack.pop();
		return ret;
	}
	if (mainStack.top().type == CalcValue::INX) {
		std::vector <ssize_t> inx;
		GET_LIST_INDEX(mainStack, var_nodes, inx);
		CalcValue* cv = conv_top(mainStack, var_nodes, showErrors, freeable);
		if (!cv || cv->type != CalcValue::ARR) {
			PASS_ERROR("\aERROR: index without list\n");
		}

		ret = cv->getListElem(inx);
		if (!cv) {
			PASS_ERROR("\aERROR: list index out of bounds\n");
		}
		return ret;
	}
	if (mainStack.top().type == CalcValue::REQ) {
		// <literal> :scope :scope
		if (mainStack.top().request->at(0) == " ") {
			std::vector<std::string> request = *mainStack.top().request;
			mainStack.pop();
			CalcValue* cv = conv_top(mainStack, var_nodes, showErrors, freeable);
			if (cv->type != CalcValue::OBJ) {
				PASS_ERROR("\aERROR: member request without object\n");
			}
			ret = cv->requestMember(request, &var_nodes);
			if (!ret) {
				PASS_ERROR("\aERROR: invalid member request\n");
			}
			return ret;

		}

		// $variable :scope :scope
		UserVar* var = vars::findVar(var_nodes, mainStack.top().request->at(0).c_str());
		if (!var)
			var = vars::assignNewVar(var_nodes, mainStack.top().request->at(0).c_str(), UserType());


		ret = mainStack.top().requestMember(var_nodes);
		if (!ret) {
			var->val.object->addMember(*mainStack.top().request, CalcValue());
			ret = mainStack.top().requestMember(var_nodes);
		}

		/*
		ret = var->val.requestMember(*mainStack.top().request);
		if (!ret)
			var->val.object->addMember(*mainStack.top().request, CalcValue());
		*/

		mainStack.pop();
		return ret;

	}
	ret = new CalcValue(mainStack.top());
	freeable.push_back(ret);
	mainStack.pop();
	return ret;

}

// recursively evaluate the top value
CalcValue* conv_top_keep_refs(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, const bool showErrors, std::vector<void*>& freeable){
	CalcValue* p = NULL, *ret = NULL;

	if (mainStack.empty())
		return NULL;

	//printf("conv: %s\n", mainStack.top().typeName());


	// list index
	if (mainStack.top().type == CalcValue::INX) {
		std::vector <ssize_t> inx;
		GET_LIST_INDEX(mainStack, var_nodes, inx);
		CalcValue* cv = conv_top(mainStack, var_nodes, showErrors, freeable);
		if (!cv || cv->type != CalcValue::ARR) {
			PASS_ERROR("\aERROR: index without list\n");
		} else {
			ret = cv->getListElem(inx);
			if (!ret) {
				PASS_ERROR("\aERROR: list index out of bounds\n");
			}
			return ret;
		}
	}

	// object member request
	if (mainStack.top().type == CalcValue::REQ) {

		// <literal> :scope :scope
		if (mainStack.top().request->at(0) == " ") {
			std::vector<std::string> request = *mainStack.top().request;
			mainStack.pop();

			CalcValue* cv = conv_top(mainStack, var_nodes, showErrors, freeable);
			if (cv->type != CalcValue::OBJ) {
				PASS_ERROR("\aERROR: member request without object\n");
			}
			ret = cv->requestMember(request, &var_nodes);
			if (!ret) {
				PASS_ERROR("\aERROR: invalid member request\n");
			}

			return ret;

			// $variable :scope :scope
		}
	}

	// const/literal
	ret = new CalcValue(mainStack.top()); // a waste of memory...
	freeable.push_back(ret);
	mainStack.pop();
	return ret;

}
