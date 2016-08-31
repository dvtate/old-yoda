#ifndef STRING_STACK_H
#define STRING_STACK_H

#include <cstdlib>
#include <cstring>
#include <inttypes.h>


class StrStack {
public:

	// how many times to double the number of lines
	uint8_t sizeFactor;

	char** buffer;
  	size_t stackDepth;

	char** stackHead;

  	StrStack():
  		sizeFactor(0),
  		buffer((char**) malloc(256)),
  		stackDepth(0),
  		stackHead(buffer)
  	{}

  	~StrStack(){

		for ( ; stackDepth > 0; stackDepth--)
			free(*(buffer--));

		free(buffer);

	}

	// resets the object
	void clear();

	// doubles the size of the buffer
	void grow();

	// pushes a line to the top of the stack
	void push(const char* str);

	// deletes the string at the top of the stack.
	void pop();

	// the string at the top of the stack
	/// be sure to copy it before calling pop()
	char* top()
		{ return stackDepth ? *(buffer - 1) : NULL; }

	size_t& size()
		{ return stackDepth; }

	void changeTop(const char* str);
	void top(const char* str)
		{ return changeTop(str);}

};



#endif
