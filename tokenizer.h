#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.hpp"


// deleteChar() && deleteChars() defined in utils.hpp
void deleteChar(char* toDelete);
void deleteChars(char* toDelete, const size_t numChars);

// Returns the start of the token/string-constant
// manages escape-sequences within string-constants
char* qtok(char* str, char** next){
	if (str == NULL)
		return NULL;
	char *current = str, *start = str;

	// skip beginning whitespace.
	while (*current != '\0' && isspace(*current))
		current++;

	start = current;

	if (*current == '"') {
		// Quoted token
		start = current;
		current++; // Skip the beginning quote.

		for (;;) {
			// Go until we find a quote or the end of string.
			while (*current != '\0'  && *current != '"') {

				if (*current == '\\') { // escape sequences
					if (*(current + 1) == 'n') {
						*current = '\n';
						*(current + 1) = '\r';	// this is much more efficienct than deleteChar()
						// also, this doesn't do the same thing as '\n'..
						// so I probably should find a better solution...
						// TODO: find a better solution for "\n"
						// line feed
					} else if (*(current + 1) == 'r') { // "\\r" => "\r\r"
						*current = *(current + 1) = '\r'; // nice hack ;)
						current++;

						// horizontal tab
					} else if (*(current + 1) == 't') { // "\\t" => "\t"
						*current = '\t';
						deleteChar(current + 1);

						// bel
					} else if (*(current + 1) == 'a') { // "\\a" => "\a"
						*current = '\a';
						deleteChar(current + 1);

						// backspace
					} else if (*(current + 1) == 'b') { // "\\b" => "\b"
						*current = '\b';
						deleteChar(current + 1);

						// formfeed
					} else if (*(current + 1) == 'f') { // "\\f" => "\f"
						*current = '\f';
						deleteChar(current + 1);

						// vertical tab
					} else if (*(current + 1) == 'v') { // "\\v" => "\v"
						*current = '\v';
						deleteChar(current + 1);

						// escaped backslash
					} else if (*(current + 1) == '\\') // "\\\\" => "\\"
						deleteChar(current);

						// escape sequence "\nnn"
					else if (isdigit(*(current + 1))) { // "\\nnn" => "char(nnn)"
						// escape sequence can be a maximum of 3 chars long
						char str[3] = {
								*(current + 1),
								*(current + 2),
								*(current + 3)
						};
						char* ptr;
						// convert the octal literal to single char (8bit int)
						*current = (char) strtol(str, &ptr, 8);
						deleteChars(current + 1, ptr - str);

						// escape sequence "\xhh"
					} else if (*(current + 1) == 'x' && isdigit(*(current + 2))) { // "\\hh" => "char(hh)"
						// escape sequence can be a maximum of 2 chars long
						char str[2] = { *(current + 2), *(current + 3) };
						char* ptr;
						// convert the hex literal to single char (8bit int)
						*current = (char) strtol(str, &ptr, 16);
						deleteChars(current + 1, ptr - str);

						// indicates that the next character should be ignored by the tokenizer
						// this works for \", \', etc.
					} else
						deleteChar(current);

					//current++;
				}
				current++;

			}

			//current++;

			// Reached the ending quote.
			goto finalize;
		}
	}


	// Not quoted so run till we see a space.
	while (*current && !isspace(*current))
		current++;

	finalize:

	if (*current) {
		// Close token if not closed already.
		*current = '\0';
		current++;
		// Eat trailing whitespace.
		while (*current && isspace(*current))
			current++;
	}

	*next = current;

	//return unescapeToken(start);
	return start;

}


std::vector<std::string> splitList(std::string& str) {
	ssize_t pos = 0, past = 0;
	uint16_t ldepth = 0;
	uint16_t sdepth = 0;
	bool quoted = false;
	bool commented = false;
	std::vector<std::string> ret;

	for (char ch : str) {
		switch (ch) {
			case '(':
				ldepth++; break;
			case ')':
				ldepth--; break;
			case '{':
				sdepth++; break;
			case '}':
				sdepth--; break;
			case '\"':
				quoted = !quoted;
				break;
			case '#':
				commented = true;
				break;
			case '\n':
				commented = false;
				break;
			case ',':
				if (ldepth <= 0  && sdepth <= 0 && !quoted && !commented) {
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

#endif