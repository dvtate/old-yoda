#include "string_stack.hpp"

#include <stdio.h>
#include <string.h>
#include <stack>
#include <inttypes.h>


#include "fuck_windows.h"


// resets the object to it's original state
void StrStack::clear(){

	for (size_t i = 0; i < stackDepth; i++)
		free(*(buffer--));

  	// set buffer to it's original size
	stackHead = buffer = (char**) realloc(buffer, 256);

	stackDepth = 0;

}

// doubles the size of the buffer
void StrStack::grow(){

  	// don't copy the contents of an empty buffer
	if (stackDepth == 0)
		stackHead = buffer = (char**) realloc(buffer, ((1 << ++sizeFactor) * 256) * sizeof(char*));

	else {

		char** oldHead = stackHead;

	  	// make a new buffer twice as big as the old one
		char** buffer2 = stackHead = (char**) malloc(((1 << ++sizeFactor) * 256) * sizeof(char*));

	  	// copy all the strings into their new locations
	  	for (ssize_t i = 0; i < buffer - oldHead; i++)
			*(buffer2 + i) = *(oldHead + i);

		buffer2 += buffer - oldHead ;

		// delete the old buffer
	  	free(oldHead);

	  	// replace buffer with buffer2
	  	buffer = buffer2;

	}

}


// pushes a line to the top of the stack
void StrStack::push(const char* str){

	*buffer = (char*) malloc(strlen(str) + 1);

	// write the string to the buffer
	strcpy(*buffer, str);

	// go to next empty space
	buffer++;

	// if the size needs to be doubled after adding a new element
  	if (stackDepth++ == (1U <<sizeFactor) * 256)
		grow();
}



// deletes the string at the top of the stack.
void StrStack::pop(){
	if (stackDepth != 0) {
		// deallocate the line
		// & continue to point to the top
		free(*(--buffer));

		// decrease stack depth
		stackDepth--;

	}
}


// modify the top element
void StrStack::changeTop(const char* str){
	if (stackDepth) {

		char*& topStr = *(buffer - 1);

		// change the size to fit the new string
		topStr = (char*) realloc(topStr, strlen(str) + 1);

		strcpy(topStr, str);

	}

}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



extern FILE* program;

extern unsigned int line;



namespace strstk {

	static bool endOfStk(char*& str, uint16_t& depth){
		// NULL string (safety first)
		if (!str)
			return false;

		// base indentation
		if (depth == 0)
			return true;
		if (depth == 1 && *str == '}')
			return true;

		// the line is commented out, or end of string
		if (*str == '#' || !*str)
			return false;

		while (depth && *(++str) != '#' && *str) {
			if (*str == '{')
				depth++;
			else if (*str == '}')
				depth--;
			else if (*str == '?')
				depth = 0;


		}

		return depth ? false : true;

	}


	StrStack* getStrStack(char*& str){

		char* codeLine = str;
		uint16_t depth = 1; // shouldn't be >65000 levels of indentation...
		StrStack* ret = new StrStack();

		bool isEnd = endOfStk(str, depth);
		if (!isEnd)
			ret->push(codeLine);
		else {
			*str = '\0';
			str++;
			ret->push(codeLine);
			return ret;
		}

		codeLine = (char*) malloc(255);
		size_t lineLen = 255;


		while (!isEnd) {

			// read the next line from our program
			if (getline(&codeLine, &lineLen, program) == -1)
				return NULL; // this signals an error from process_line.hpp
			str = codeLine;

			line++; // we added a line to our file

			isEnd = endOfStk(str, depth);
			if (!isEnd)
				ret->push(codeLine);
			else {
				*str = '\0';
				ret->push(codeLine);
				*str = 'x';
				return ret;
			}

		}

		return ret;

	}



	void printStrStack(const StrStack& stack){

		char** buff = stack.buffer;
		//size_t sd = stack.stackDepth;


		int num = 0;

		while (buff-- > stack.stackHead)
			std::cout <<num++ <<": " <<*(buff) <<std::endl;

	}
}


