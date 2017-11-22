#include "utils.hpp"


void displayHelp(){ // this could all be combined into one print statement...
	std::cout <<"If this is your first time using this language, you should check\
 the README on this project's github page for a short intro.\n";

	// I want the URL to be underlined and blink :)
#ifndef _WIN32
	setTermEffect(TERM_EFF_BLINK);
	setTermEffect(TERM_EFF_UNDERLINE);
#endif

	std::cout <<"<https://github.com/dvtate/rpn/>\n";

#ifndef __WIN32
	setTermEffect();
#endif
}

namespace strutils {

	// removes char from string
	// this is extremely inefficient, but the best way I can think of...
	void deleteChar(char* toDelete)
	{ memmove(toDelete, toDelete + 1, strlen(toDelete)); }
	void deleteChars(char* toDelete, const size_t numChars)
	{ memmove(toDelete, toDelete + numChars, strlen(toDelete)); }



	char* skipSpaces(char* p){
		while (isspace(*p))
			p++;

		return p;
	}



	char* trimStr(char* string){

		char* ret = skipSpaces(string);
		while (!isspace(*string) && *string != '\0')
			string++;

		*(string) = '\0';

		return ret;

	}

	// thx @chux http://stackoverflow.com/a/27305359/4475863
	char* stristr(const char* haystack, const char* needle) {
		do {
			const char* h = haystack;
			const char* n = needle;
			while (tolower(*h) == tolower(*n) && *n) {
				h++;
				n++;
			}

			if (!*n)
				return (char *) haystack;

		} while (*haystack++);

		return NULL;
	}


	// find the number of times a certain substring occurs in a string (unused)
	unsigned int countOccurances(const char* str, char* sub) {
		unsigned int ret = 0;
		while (*str) {
			char* substr = sub;
			while (*substr && *substr == *str) {
				str++; substr++;
			}
			if (!*substr) ret++;
			else str++;
		}

		return ret;
	}

	// You must free the result if result is non-NULL.
	// thanks: http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
	// translated answer to C++
	char* str_replace(char *orig, const char *rep, const char *with) {
		char *result; // the return string
		char *ins;    // the next insert point
		char *tmp;    // varies
		size_t len_rep;  // length of rep (the string to remove)
		int len_with; // length of with (the string to replace rep with)
		int len_front; // distance between rep and end of last rep
		int count;    // number of replacements

		// sanity checks and initialization
		if (!orig || !rep)
			return NULL;

		len_rep = strlen(rep);

		// empty rep causes infinite loop during count
		if (len_rep == 0)
			return NULL;

		if (!with) {
			char emptystring[1];
			with = emptystring;
		}
		len_with = strlen(with);

		// count the number of replacements needed
		ins = orig;
		for (count = 0; (tmp = strstr(ins, rep)); ++count) {
			ins = tmp + len_rep;
		}

		tmp = result = (char*) malloc(strlen(orig) + (len_with - len_rep) * count + 1);

		if (!result)
			return NULL;

		// first time through the loop, all the variable are set correctly
		// from here on,
		//    tmp points to the end of the result string
		//    ins points to the next occurrence of rep in orig
		//    orig points to the remainder of orig after "end of rep"
		while (count--) {
			ins = strstr(orig, rep);
			len_front = ins - orig;
			tmp = strncpy(tmp, orig, len_front) + len_front;
			tmp = strcpy(tmp, with) + len_with;
			orig += len_front + len_rep; // move to next "end of rep"
		}
		strcpy(tmp, orig);
		return result;
	}

}


namespace fileutils {


	char *getLineFromFile(const char *filename, size_t lineNumber) {
		FILE *file = fopen(filename, "r");

		size_t count = 1;
		if (file != NULL) {

			char *line = (char *) malloc(200);
			size_t lineLen = 200;


			while (getline(&line, &lineLen, file) != -1)
				if (count == lineNumber)
					break;
				else
					count++;

			if (count) {
				fclose(file);
				return line;
			} else {
				std::cerr << "\autils.h: getLineFromFile(): line index not found";
				fclose(file);
				return (char *) NULL;
			}

		} else {
			std::cerr << "\aDAFUQ: fopen(\"" << filename << "\", \"r\") == NULL\n"
			          << __FILE__ << ':' << __LINE__ << std::endl;
			return (char *) NULL;
		}

	}

	char *getLineFromFile(FILE *file, size_t lineNumber) {

		size_t count = 1;
		if (file != NULL) {

			char *line = (char *) malloc(200);
			size_t lineLen = 200;


			while (getline(&line, &lineLen, file) != -1)
				if (count == lineNumber) break;
				else count++;

			if (count) {
				fclose(file);
				return line;
			} else {
				std::cerr << "\autils.h: getLineFromFile(): line index not found";
				fclose(file);
				return (char *) NULL;
			}

		} else
			return (char *) NULL;

	}

	// make the prefix/template for tmpfiles to follow
	char* mktmpPrefix() {
		const unsigned int len = progName ? strlen(progName) : 4; // "yoda" has 4 chars
		char* ret = (char*) malloc(len + 1); // not safe to free
		strcpy(ret, progName ? progName : "yoda"); // blah.ysXXXXXX | yodaXXXXXX
		return ret;
	}


	//
	FILE* mktmpfile() {
		char tmpl[] = ".yodaXXXXXXXX";
		mkstemp(tmpl);
		FILE* ret = fopen(tmpl, "w+b");
		remove(tmpl);
		return ret;
	}

	std::vector<char*> tmp_files;
	void delTmpFiles(){
		for (char* c : tmp_files) {
			remove(c);
			free(c);
		}
	}

/*
	std::string tempdir() {
		char nameTemplate[] = "yodaXXXXXXXX";
		printf(mkdtemp(nameTemplate));
	}*/
}
bool printCalcValue(CalcValue& val, std::vector<UserVar>& var_nodes){

	if (val.isNull())
		std::cout <<"null";
		//else if (val.type == CalcValue::BLK)
		//	std::cout <<"{...}";
	else if (val.type == CalcValue::BLK) {
		size_t len = 50;
		char* str = (char*) malloc(len);
		val.block->toString(&str, &len);
		printf("{%s}", str);
		free(str);
	} else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<'\"' <<val.getStr() <<'\"';
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(var_nodes);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(var_nodes);
		if (ret)
			return printCalcValue(*ret, var_nodes);

		// find the broken reference
		ret = val.valAtRef(var_nodes);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(var_nodes))
					ret = ret->valAtRef(var_nodes);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<(ret->string) <<"`.\n";
		return 1;
	} else if (val.type == CalcValue::ARR) {
		std::cout <<"(";
		if (val.list->size()) {
			if (val.list->at(0).type == CalcValue::REF)
				std::cout << "$" << val.list->at(0).string << ' ';
			printCalcValue(val.list->at(0), var_nodes);
			if (val.list->at(0).type == CalcValue::REF)
				std::cout << " =";

			for (size_t i = 1; i < val.list->size(); i++) {
				std::cout << ", ";
				if (val.list->at(i).type == CalcValue::REF)
					std::cout << "$" << val.list->at(i).string << ' ';
				printCalcValue(val.list->at(i), var_nodes);
				if (val.list->at(i).type == CalcValue::REF)
					std::cout << " =";
			}
		}
		std::cout <<" )";

	} else if (val.type == CalcValue::REQ && val.request->at(0) != " ") {
		printCalcValue(*val.requestMember(var_nodes), var_nodes);

	} else if (val.type == CalcValue::OBJ) {
		putchar('{');
		for (unsigned i = 0; i < val.object->members.size(); i++) {
			std::cout <<" :" <<val.object->members[i] <<' ';
			printCalcValue(val.object->values[i], var_nodes);
		}
		printf(" } object");
	}

	return 0;
}

bool printCalcValueRAW(CalcValue& val, std::vector<UserVar>& var_nodes){
	if (val.isNull())
		std::cout <<"null";
	else if (val.type == CalcValue::BLK) {
		size_t len = 50;
		char* str = (char*) malloc(len);
		val.block->toString(&str, &len);
		printf("{%s}", str);
		free(str);
	} else if (val.type == CalcValue::NUM)
		std::cout <<val.getNum();
	else if (val.type == CalcValue::STR)
		std::cout <<val.getStr();
	else if (val.type == CalcValue::REF) {
		CalcValue* ret = val.valAtRef(var_nodes);
		while (ret && ret->type == CalcValue::REF)
			ret = ret->valAtRef(var_nodes);
		if (ret)
			return printCalcValueRAW(*ret, var_nodes);

		// find the broken reference
		ret = val.valAtRef(var_nodes);
		if (ret)
			while (ret->type == CalcValue::REF)
				if (ret->valAtRef(var_nodes))
					ret = ret->valAtRef(var_nodes);
				else
					break;
		else
			ret = &val;

		std::cerr <<"\aERROR: broken reference to `$" <<ret->string <<"`.\n";

		return 1;

	} else if (val.type == CalcValue::ARR) {
		putchar('(');
		if (val.list->size()) {
			if (val.list->at(0).type == CalcValue::STR || val.list->at(0).type == CalcValue::ARR)
				printCalcValue(val.list->at(0), var_nodes);
			else
				printCalcValueRAW(val.list->at(0), var_nodes);
			for (size_t i = 1; i < val.list->size(); i++) {
				std::cout << ", ";

				if (val.list->at(i).type == CalcValue::STR || val.list->at(i).type == CalcValue::ARR)
					printCalcValue(val.list->at(i), var_nodes);
				else
					printCalcValueRAW(val.list->at(i), var_nodes);
			}
		}
		putchar(')');

	} else if (val.type == CalcValue::REQ && val.request->at(0) != " ") {
		printCalcValueRAW(*val.requestMember(var_nodes), var_nodes);

	} else if (val.type == CalcValue::OBJ) {
		putchar('{');
		for (unsigned i = 0; i < val.object->members.size(); i++) {
			std::cout <<"\n :" <<val.object->members[i] <<' ';
			printCalcValue(val.object->values[i], var_nodes);
		}
		printf(" } object");
	}

	return 0;

}




namespace commands {
	void debugStack(std::stack<CalcValue> mainStack, std::vector<UserVar>& vars){
		// since the stack is copied, we can modify it as we wish in here :)
		size_t index = 0;
		while (!mainStack.empty()) {
			std::cout <<index++ <<" : ";
			printCalcValue(mainStack.top(), vars);
			std::cout <<'\n';

			mainStack.pop();
		}

	}
}


inline size_t linesToEnd(FILE* fil){
	size_t numLines = 0;

	int ch;
	while (EOF != (ch = getc(fil)))
		if ('\n' == ch)
			++numLines;

	return numLines;
}

// variable name mutilator which uses a base 62 counting system to give variables unique names
namespace mutilate {

	const uint8_t L_REF_MUTILATOR = 11; // number of numberals
	const char baseNumerals[62] = { // base 62 numerals
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
			'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	// makes a variable name unique
	char* mutilateVarName(const char *cur_name) {
		static uint64_t uniqueID = 1;
		uint64_t id_cpy = uniqueID;

		char mut[L_REF_MUTILATOR];

		int8_t i;
		for (i = L_REF_MUTILATOR - 1; i >= 0; i--) {
			mut[i] = baseNumerals[id_cpy % 62];
			id_cpy /= 62;
			if (id_cpy == 0) break;
		}

		char* ret = (char*) calloc(strlen(cur_name) + 3 + L_REF_MUTILATOR - i, sizeof(char));

		strcat(ret, " ");
		strcat(ret, cur_name);
		strcat(ret, "#");
		strcat(ret, mut + i); // + i means dont include zeros (not set)

		uniqueID++;

		return ret;

	}
}
