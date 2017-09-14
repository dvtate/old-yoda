#include "user_types.hpp"
#include "calc_value.hpp"

void UserType::addMember(const std::string nMem, const CalcValue nVal){
	std::vector<std::string>::iterator loc = find(members.begin(), members.end(), nMem);

	// not yet a member
	if (loc == members.end()) {
		members.push_back(nMem);
		values.push_back(nVal);

	// already a member
	} else {
		values[loc - members.begin()] = nVal;
	}
}

void UserType::addMember(std::vector<std::string> loc, const CalcValue nVal){
	UserType* obj = this;
	for (uint16_t i = 0; i < loc.size() - 1; i++) {
		obj->addMember(loc[i], UserType());
		obj = obj->getMember(loc[i])->object;
	}
	obj->addMember(loc[loc.size() - 1], nVal);
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