//
// Created by tate on 15-06-17.
//

#ifndef YODA_LAMBDA_HPP
#define YODA_LAMBDA_HPP

#include <string>
#include "string_stack.hpp"

class Lambda {
public:
	StrStack src;
	std::vector<std::string> params;

	Lambda(){};
	Lambda(StrStack body, std::vector<std::string> params):
			src(body), params(params) {}

	//~Lambda(){};



};



#endif //YODA_LAMBDA_HPP
