#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <cstring>
#include <cstdlib>


/// a temporary value to hold user data in
class CalcValue {

public:

	enum { NUM, STR } type;

	union {
		double number;
		char* string;
	};

	CalcValue(){}


  	CalcValue (double val){
		number = val;
		type = NUM;
	}

	CalcValue(const char* str){
		// allocate memory for the string
	  	string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);
		type = STR;
	}

	// memory-leak solved :D
/*	~CalcValue(){
		if (type == STR)
			free(string);
	}
*/
	void setValue(const char* str){
		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;
	}

	void setValue(double val){
		if (type == STR)
			free(string);
		number = val;
		type = NUM;
	}

	double getNum(){
		if (type == NUM)
			return number;
		else
			return 0;
	}

	char* getStr(){
	  	if (type == STR)
			return string;
	  	else
		  	return (char*) NULL;
	}

	bool operator==(const CalcValue& val2){
		if (type == val2.type) {
			if (type == NUM && number == val2.number)
				return true;
			else if (type == STR && (string == val2.string || strcmp(string,val2.string) == 0))
				return true;
		}
	  	return false;
	}
};

#endif
