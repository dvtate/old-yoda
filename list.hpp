#ifndef YODA_LIST_HPP
#define YODA_LIST_HPP

#include "calc_value.hpp"
#include "win_supp.h"


extern int line;

// some functions for handling lists and stuff
namespace list {

	// note, str is modified and this is used in processLine()
	std::string getList(char*& str, FILE* codeFeed, std::vector<void*>& freeable);

	// separate different parts of the list into sections
	std::vector<std::string> split(std::string& str);

}


#endif //YODA_LIST_HPP