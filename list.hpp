#ifndef YODA_LIST_HPP
#define YODA_LIST_HPP

#include "calc_value.hpp"



// some functions for handling lists and stuff
namespace list {

	// does this string end the list?
	// if so:
	//      - returns true
	//      - sets str = the end of the list
	//
	// if not
	//      - returns false
	//      - str's value is of no use
	static bool endOfList(char*& str, uint16_t& depth){
		// NULL string (safety first)
		if (!str)
			return false;

		// base indentation
		if (depth == 0)
			return true;
		if (depth == 1 && *str == ')')
			return true;
		if (*str == '(')
			depth++;

		// the line is commented out, or end of string
		if (*str == '#' || !*str)
			return false;

		while (depth && *(++str) != '#' && *str)
			if (*str == '(')
				depth++;
			else if (*str == ')')
				depth--;

		return !depth;

	}



	// note, str is modified and this is used in processLine()
	std::string getList(char*& str, FILE* codeFeed){
		char* codeLine = str;
		uint16_t depth = 1; // shouldn't be 65000 dimensional array
		std::string arrBody = "";

		bool isEnd = endOfList(str, depth);
		if (!isEnd)
			arrBody += codeLine;
		else {
			*str = '\0';
			str++;
			arrBody += codeLine;
			return arrBody;
		}

		codeLine = (char*) malloc(255);
		size_t lineLen = 255;

		while (!isEnd) {

			if (getline(&codeLine, &lineLen, codeFeed) == -1)
				return "("; // this signals an error from process_line.hpp

			/*
			// remove trailing newline
			size_t len = strlen(codeLine) - 1;
			if (codeLine[len] == '\n')
				codeLine[len] = '\0';
			*/

			str = codeLine;

			line++; // we added a line to our file

			isEnd = endOfList(str, depth);
			if (!isEnd)
				arrBody += codeLine;
			else {
				*str = '\0';
				arrBody += codeLine;
				*str = ' ';
				free(codeLine);
				printf(str);
				return arrBody;
			}

		}
		free(codeLine);
		return arrBody;
	}


	// separate different parts of the list into sections
	std::vector<std::string> split(std::string& str) {
		ssize_t pos = 0, past = 0;
		uint16_t ldepth = 0; // up to 65k dimensions
		uint16_t sdepth = 0; // up to 65k layers of nested structures
		bool quoted = false; // are we in a string?
		bool commented = false; // is this commented?
		std::vector<std::string> ret;

		for (size_t i = 0; i < str.size(); i++) {

			switch (str[i]) {
				case '(':
					if (!commented && !quoted)
						ldepth++;
					break;
				case ')':
					if (!commented && !quoted)
						ldepth--;
					break;
				case '{':
					if (!commented && !quoted)
						sdepth++;
					break;
				case '}':
					if (!commented && !quoted)
						sdepth--;
					break;
				case '\"':
					if (!commented)
						if (!(quoted && str[i-1] == '\\')) // make sure quote isn't escaped
							quoted = !quoted;
					break;
				case '#':
					if (!quoted) commented = true;
					break;
				case '\n':
					commented = false;
					break;
				case ',':
					if ( !quoted && !commented && ldepth <= 0  && sdepth <= 0) {
						ret.push_back(str.substr(past, pos - past));
						past = pos + 1;
					}
					break;
			}
			pos++;
		}

		// gets value after last comma
		ret.push_back(str.substr(past, pos - past));


		return ret;

	}


//	std::string getIndex(char*& str, FILE* codeFeed){
//
//	}


}


#endif //YODA_LIST_HPP
