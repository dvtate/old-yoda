#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include <string.h>
#include <stdlib.h>


/// a temporary value to hold user data in
class CalcValue {

public:

	enum { NUM, STR } type;

	union {
		double number;
		char* string;
	};

  	// Null object
	CalcValue(): type(STR), string(NULL) {}


  	CalcValue (double val):	type(NUM) {
		number = val;
		type = NUM;
	}


	CalcValue(const char* const str): type(STR) {

		// allocate memory for the string
	  	string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);
		type = STR;

	}

	// this causes a core dump (QwQ)
	/*~CalcValue(){
		if (type == STR)
			free(string);
	}*/

	void setValue(const char* const str) {

		// memory leaks are pretty bad
		if (type == STR)
			free(string); // free(NULL) gives no errors :)


		string = (char*) malloc(strlen(str) + 1);

		// write the string to the buffer
		strcpy(string, str);

		type = STR;
	}

	void setValue(double val){
		if (type == STR)
			free(string); // free(NULL) gives no errors :)
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
			else if (type == STR && (string == val2.string || strcmp(string, val2.string) == 0))
				return true;
		}
	  	return false;
	}

	// A Null value
	bool isEmpty()
		{ return (type == STR) && (string == NULL); }

	bool isNull()
		{ return isEmpty(); }


};


#define NULL_CALCVAL_OBJECT CalcValue((char*) NULL);
#endif
