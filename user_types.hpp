#ifndef YODA_USER_TYPE_HPP
#define YODA_USER_TYPE_HPP

#include <vector>
#include <algorithm> // find()
#include <string>

class CalcValue;


class UserType {
public:
	std::vector<std::string> members; // I have given up on performance.. :(
	std::vector<CalcValue>   values;

	UserType(){};

	UserType(const UserType& in):
		members(in.members),
		values(in.values) {}

	// attempts to add a member, if already there, then change value
	void addMember(const std::string nMem, const CalcValue nVal);

	bool hasMember(const std::string name) {
		return find(members.begin(), members.end(), name) != members.end();
	}


};








#endif //YODA_USER_TYPE_HPP
