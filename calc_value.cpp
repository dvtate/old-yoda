#include "calc_value.hpp"





bool CalcValue::operator==(const CalcValue& cv2) {

	// same type
	if (type == cv2.type) {

		// same value
		if (type == NUM && number == cv2.number)
			return true;
		if (type == STR && isEmpty() != !cv2.string)
			return false;
		if (type == STR && string == cv2.string)
			return true;
		else if (type == STR || cv2.type == CalcValue::REF)
			return strcmp(string, cv2.string) == 0;
			// this doesnt work...
		else if (type == CalcValue::ARR) {
			if (list->size() != cv2.list->size())
				return false;
			for (size_t i = 0; i < list->size() - 1; i++)
				if (!((*list)[i] == (*cv2.list)[i]))
					return false;

			return true;
		} else if (type == INX)
			return (index == cv2.index);
		else if (type == OBJ) {
			return false;

			// TODO: find a better solution for REQ == REQ
		} else if (type == REQ) {
			if (request->size() != cv2.request->size())
				return false;
			for (size_t i = 0; i < request->size() - 1; i++)
				if (!((*request)[i] == (*cv2.request)[i]))
					return false;

			return true;
		}
	}

	return false;


}

std::string CalcValue::toString(std::vector<UserVar>& var_nodes){

	std::string ret;

	// null
	if (isNull())
		return "null";

		// macro
	else if (type == CalcValue::BLK) {
		size_t len = 50;
		char* str = (char*) malloc(len); // alloc mem for str
		block->toString(&str, &len);     // fill str
		ret = str;   // copy str into ret
		free(str);   // recycle memory
		return ret;

		// number
	} else if (type == CalcValue::NUM) {
		char str[27];
		snprintf(str, 26, "%*.*g", 10, 16, getNum());
		return strutils::trimStr(str);

		// already a string
	} else if (type == CalcValue::STR)
		return  "\"" + std::string(string) + "\"";

		// a variable
	else if (type == CalcValue::REF) {
		CalcValue* val = valAtRef(var_nodes);
		while (val && val->type == CalcValue::REF)
			val = val->valAtRef(var_nodes);
		if (val)
			return val->toString(var_nodes);
		else
			return "reference_error";

		// list printing a list
	} else if (type == CalcValue::ARR) {
		ret += "(";
		if (list->size()) {
			ret += list->at(0).toString(var_nodes);
			for (size_t i = 1; i < list->size(); i++)
				ret += ", " + list->at(i).toString(var_nodes);
		}
		ret+= " )";

		// an object
	} else if (type == CalcValue::OBJ) {
		ret += "{";
		for (unsigned i = 0; i < object->members.size(); i++) {
			ret += "\n :" + object->members[i] + " ";
			ret += object->values[i].toString(var_nodes);
		}
		ret += "\n} object";

		// a member request
	} else if (type == CalcValue::REQ) {
		if (request->at(0) == " ")
			ret += "__";
		else
			ret += "$" + request->at(0);

		for (unsigned i = 1; i < request->size(); i++)
			ret += " :" + request->at(i);

	}

	return ret;

}

// string form of type
const char* CalcValue::typeName() {
	switch (type) {
		case STR:
			if (isNull()) // null ptr
				return "null";
			else
				return "string";
		case NUM: return "number/boolean";
		case REF: return "reference";
		case BLK: return "macro";
		case ARR: return "list";
		case LAM: return "lambda";
		case OBJ: return "object";
		case REQ: return "member";
		case INX: return "index";
		default: return "WTF";
	}
}


/* Sub type specific functions which make it easier to work with specific types,
 * such as objects/members, lists/indexes, etc.
 * ##### #   # ###  ####        ### ###  ###   ##  ### #### ###  ##       #### #   # #   #  ###
 *   #    # #  #  # #          #    #  # #    #  #  #  #     #  #  #      #     # #  ##  # #
 *   #     #   ###  ###         ##  ###  ##  #      #  ###   #  #         ###    #   # # #  ##
 *   #     #   #    #             # #    #    #  #  #  #     #  #  #      #     # #  #  ##    #
 *   #     #   #    ####       ###  #    ###   ##  ### #    ###  ##       #    #   # #   # ###  #
 */

// list accessor
CalcValue* CalcValue::getListElem(const std::vector<ssize_t>& elem_index) {
	if (type != ARR)
		return NULL;

	CalcValue* ret = this;

	for (int16_t i = elem_index.size() - 1; i >= 0; i--) {
		if ((ssize_t) ret->list->size() <= elem_index[i] || (ssize_t) ret->list->size() < abs(elem_index[i])) {
			return NULL;
		}
		ret = elem_index[i] < 0 ?
		      &ret->list->at(ret->list->size() + elem_index[i]) :
		      &ret->list->at(elem_index[i]);
	}

	return ret;
}
// list modifier
bool CalcValue::assignElem(const std::vector<ssize_t>& elem_index, const CalcValue& val) {

	CalcValue* mod = getListElem(elem_index);
	if (!mod)
		return false;

	mod->setValue(val);

	return true;
}

bool CalcValue::deleteListElem(const std::vector<ssize_t>& elem_index) {
	if (type != ARR)
		return false;

	CalcValue* ret = this;
	for (int16_t i = elem_index.size() - 1; i > 0; i--) {
		if ((ssize_t) ret->list->size() <= elem_index[i] || (ssize_t) ret->list->size() < abs(elem_index[i]))
			return false;

		ret = elem_index[i] < 0 ?
		      &ret->list->at(ret->list->size() + elem_index[i]) :
		      &ret->list->at(elem_index[i]);
	}
	if (!ret || !ret->list)
		return false;

	ret->list->erase(ret->list->begin() + elem_index[0]);
	return true;

}


CalcValue* CalcValue::requestMember(std::vector<UserVar>& var_nodes) {
	if (type != REQ && request->at(0) == " ")
		return NULL;

	CalcValue* ret = CalcValue()
			.setRef(request->at(0).c_str())
			.valAtRef(var_nodes);
	if (!ret)
		return NULL;

	for (uint16_t i = 1; i < request->size(); i++) {
		if (ret->type != CalcValue::OBJ)
			return NULL;

		// if the object doesn't have a member with that name
		if (! (ret = ret->object->getMember(request->at(i))))
			return NULL;

	}
	return ret;

}

CalcValue* CalcValue::requestMember(UserVar* first_node) {
	if (type != REQ || request->at(0) == " ")
		return NULL;

	CalcValue* ret = CalcValue()
			.setRef(request->at(0).c_str())
			.valAtRef(first_node);
	if (!ret)
		return NULL;

	for (uint16_t i = 1; i < request->size(); i++) {
		if (ret->type != CalcValue::OBJ)
			return NULL;

		// if the object doesn't have a member with that name
		if (! (ret = ret->object->getMember(request->at(i))))
			return NULL;

	}
	return ret;

}

CalcValue* CalcValue::requestMember(std::vector<std::string>& req, std::vector<UserVar>* var_nodes = NULL) {
	if (req.at(0) != " ") {
		return NULL;
	}


	CalcValue* ret = this;
	//printf("reqMem:%s\n", ret->toString(*var_nodes).c_str());

	for (uint16_t i = 1; i < req.size(); i++) {

		//printf("reqMem:%s\n", ret->toString(*var_nodes).c_str());

		// request expected an object where there is none...
		if (ret->type != CalcValue::OBJ)
			return NULL;

		// if the object doesn't have a member with that name
		if (!(ret = ret->object->getMember(req[i])))
			return NULL;

	}

	return ret;

}
