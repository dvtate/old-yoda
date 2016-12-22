#ifndef STRING_STACK_H
#define STRING_STACK_H

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <iostream>
#include <stack>

class CalcValue;
class UserVar;

class StrStack {

public:
        // how many times to double the number of lines
        uint8_t sizeFactor;

        char** buffer;
        size_t stackDepth;

        char** stackHead;


        StrStack():
                sizeFactor(0),
                buffer((char**) malloc(256 * sizeof(char*))),
                stackDepth(0),
                stackHead(buffer)
        { }

        StrStack(const StrStack& cpy):
                sizeFactor(cpy.sizeFactor),
                buffer((char**) malloc((1 << cpy.sizeFactor) * 256 * sizeof(char*))),
                stackDepth(cpy.stackDepth),
                stackHead(buffer)
        {
                char** sh = cpy.stackHead;
                while (sh != cpy.buffer) {
                        *buffer = (char*) malloc ( strlen(*sh) + 1 );
                        strcpy(*buffer++, *sh++);
                }

        }

        ~StrStack(){
                for (; stackDepth > 0; stackDepth--)
                        free(*(--buffer));

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


        StrStack& operator=(const StrStack cpy){
                sizeFactor = cpy.sizeFactor;
                buffer = (char**) malloc((1 << cpy.sizeFactor) * 256 * sizeof(char*));
                stackDepth = cpy.stackDepth;
                stackHead = buffer;

                char** sh = cpy.stackHead;
                while (sh != cpy.buffer) {
                        *buffer = (char*) malloc ( strlen(*sh) + 1 );
                        strcpy(*buffer++, *sh++);
                }

                return *this;
        }

};

// associated functions
namespace strstk {

	// gets a string stack from a file
	StrStack* getStrStack(char*& str);

}


#endif
