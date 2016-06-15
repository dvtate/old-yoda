#ifndef CALC_VALUE_H
#define CALC_VALUE_H

class CalcValue {

public:

	enum type { NUM, STR } type;

	union {
		long double number;
		char* string;
	};

	CalcValue(){}

  	CalcValue (long double val){
		number = val;
		type = NUM;
	}

	CalcValue(const char* val){
		strcpy(string, val);
		type = STR;
	}




	void setValue(const char* str){
		strcpy(string, str);
		type = STR;
	}

	void setValue(long double val){
		number = val;
		type = NUM;
	}

	long double getNum(){
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
};

#endif
