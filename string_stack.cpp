#include "string_stack.hpp"

#include <iostream>
#include <string.h>

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

	  	// make a new buffer twice as big as the old one
		char** buffer2 = stackHead = (char**) malloc(((1 << ++sizeFactor) * 256) * sizeof(char*));

	  	// copy all the strings into their new locations
	  	for (size_t i = 0; i < stackDepth; i++)
			*(buffer2 + i) = *(buffer--);
		// delete the old buffer
	  	free(stackHead);

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
