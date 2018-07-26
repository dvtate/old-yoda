
#include "user_types.hpp"
#include "calc_value.hpp"


CalcValue UserType::getKeys() {
	std::vector<CalcValue> keys(members.size());
	for (size_t i = 0; i < members.size(); i++)
		keys[i] = CalcValue(members[i].c_str());
	return CalcValue(keys);
}

CalcValue* UserType::getMember(const std::string query) {

	//printf("ut.getmem(%s)\n", query.c_str());

	// ghetto & inefficient
	if (query == "keys" || query == "self") {
		addMember("keys", getKeys());
		auto loc = find(attrNames.begin(), attrNames.end(), query);
		return &attributes[loc - attrNames.begin()];
	}


	for (unsigned i = 0; i < members.size(); i++)
		if (members[i] == query)
			return &values[i];
		//else printf("ut.gl != %s\n", members[i].c_str());

	return NULL;

	/*
	auto tmp = find(members.begin(), members.end(), query);
	printf("UT.getMember(%d):%s\n", tmp - members.begin(), values[tmp - members.begin()].typeName());
	if (tmp == members.end()) {
		return NULL;
	}

	return &values[tmp - members.begin()];
	*/
}


UserType& UserType::addMember(std::string nMem, CalcValue nVal){
	//printf("UT.am(%s)", nMem.c_str());

	if (nMem == "self" || nMem == "keys") {
		auto loc = find(attrNames.begin(), attrNames.end(), nMem);
		attributes[loc - attrNames.begin()] = nVal;
		return *this;
	}
	std::vector<std::string>::iterator loc = find(members.begin(), members.end(), nMem);

	// not yet a member
	if (loc == members.end()) {
		members.push_back(nMem);
		values.push_back(nVal);

	// already a member
	} else {
		values[loc - members.begin()] = nVal;
	}

	return *this;
}

UserType& UserType::addMember(std::vector<std::string> loc, const CalcValue nVal){
	//printf("ut.addmem: adding ");
	/*for (std::string s : loc) {
		printf(":%s", s.c_str());
	}
	printf("\n");*/


	UserType* obj = this;
	for (uint16_t i = 1; i < loc.size() - 1; i++) {
		obj->addMember(loc[i], UserType());
		obj = obj->getMember(loc[i])->object;
	}
	return obj->addMember(loc[loc.size() - 1], nVal);

}

bool UserType::operator==(const UserType& vs){
	if (members.size() != vs.members.size())
		return false;
	for (uint16_t i = 0; i < members.size(); i++) {
		if (values[i].type != CalcValue::LAM)
			continue;

		auto tmp = find(vs.members.begin(), vs.members.end(), members[i]);

		if (tmp == vs.members.end())
			return false;

		if (values[i] != vs.values[tmp - vs.members.begin()])
			return false;

	}
	return true;


}

void UserType::addSelfRef(std::string ref) {

	addMember("self", CalcValue().setRef(ref.c_str()));

	for (CalcValue& val : values)
		if (val.type == CalcValue::LAM)
			val.lambda->src.edit(0, ("\n $self \"" + ref + "\" $ = " + val.lambda->src.at(0)).c_str());

}
