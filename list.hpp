#ifndef YODA_LIST_HPP
#define YODA_LIST_HPP

#include "calc_value.hpp"

namespace list {

	bool endOfList(char*& str, uint16_t& depth){
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

			/*di
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

}

#endif //YODA_LIST_HPP
