#ifndef YODA_LAMBDA_HPP
#define YODA_LAMBDA_HPP

#include <string>
#include <vector>
#include <inttypes.h>

#include "string_stack.hpp"

// this really should just be a struct defined somewhere else...
class Lambda {
public:
	std::vector<std::string> params;
	StrStack src;

	Lambda(){};
	Lambda(StrStack body, std::vector<std::string> params):
			params(params), src(body) {}

	//~Lambda(){};

	// are the params they gave acceptable?
	int16_t validParams(){
		if (!params.size())
			return -1;

		bool defaultVal = false;
		bool va_arg = false;

		for (uint8_t i = 0; i < params.size(); i++) {
			int sps = countSpaces(i);
			if (sps == 2) {
				if (va_arg)
					return i;
				defaultVal = true;
			} else if (sps == 1) // default arg
				va_arg = true;
			else if (defaultVal) // normal arg after a default val
				return i;
		}

		return -1;
	}

	int16_t requiredArgs(){
		int ret = 0;
		for (const std::string& param : params)
			if (param.at(0) != ' ')
				ret++;
			else
				return -ret;

		return ret;
	}

	int max_args() {
		int max = 0;
		for (unsigned i = 0; i < params.size(); i++) {
			if (countSpaces(i) == 1) { // lambdas with va_args can take infinite arguments
				return -1;
			}
			max++;
		}
		return max;
	}

	// returns a vector containing the parameter each argument will be bound to
	// one element per argument
	std::vector<int16_t> bindArgs(const uint8_t argc){
		std::vector<int16_t> ret;
		if (!validParams()) {
			ret.push_back(-1);
			return ret;
		}
		//if ()
		ret.reserve(argc);
		uint8_t param = 0;
		for (uint8_t i = 0; i < argc; i++)
			if (param + 1U < (unsigned) params.size())
				ret.push_back(param++);
			else
				ret.push_back(param);


		return ret;
	}


	// this doesnt actually count spaces correctly... it conserves CPU
	int countSpaces(int param) {

		// 0 spaces = normal arg
		// 1 space = va_args
		// 2 spaces = arg + handler

		//if (params.size() - 1 < param)
		//	return -2;
		// "a"
		if (params[param].at(0) != ' ')
			return 0;

		// " a process to runn if $a not provided"
		for (uint8_t i = 1; i < params[param].size(); i++) {
			if (params[param].at(i) == ' ')
				return 2;
		}

		// " a"
		return 1;
	}
};


#endif //YODA_LAMBDA_HPP
