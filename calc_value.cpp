#include "calc_value.hpp"

const char* CVtypename(CalcValue val) {
	if (val.isNull()) // NULL string pointer
		return "NULL_VAL";

	else if (val.type == CalcValue::STR) // string-type
		return "string"; // STR

	else if (val.type == CalcValue::NUM) // number-type
		return "number/boolean"; // NUM/BLN

	else if (val.type == CalcValue::REF) // variable reference
		return "reference";

	else if (val.type == CalcValue::BLK) // string_stack
		return "macro";

	else if (val.type == CalcValue::ARR) // list
		return "list";

	else if (val.type == CalcValue::LAM) // lambda
		return "lambda";

	else if (val.type == CalcValue::OBJ) // object
		return "object";

	else if (val.type == CalcValue::REQ) // member request type
		return "memeber";

	else if (val.type == CalcValue::INX) // list index type
		return "index";

	return "WTF";
}