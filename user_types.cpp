#include "user_types.hpp"
#include "calc_value.hpp"

void UserType::addMember(const std::string nMem, const CalcValue nVal) {
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
