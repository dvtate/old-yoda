#include "core.hpp"

// will be the error code given when a function return successfully
// this content is set so that if they use it out of context they are still told so
const char* lambda_finish = "\aERROR: use of `return` or `break` out of context\n";


// this gets returned from runMacro() on error
const char* macro_error = "\aERROR: in macro execution\n";

FILE* program = stdin;
char* progName = NULL;


void runFile(char* programFile, bool& errorReporting){


	progName = programFile;

	program = fopen(programFile, "r");

	// file error
	if (program == NULL) {

		setTermEffect(TERM_EFF_BOLD);

		std::cerr <<metaName <<": ";
		color_fputs(stderr, "error: ", 255, 0, 0);
		std::cerr <<": could not open file \'" <<programFile <<"\'\n";

		setTermEffect();

		exit(EXIT_FAILURE);

	}


	// the most important component of the program :)
	std::stack<CalcValue> mainStack;

	UserVar first_node(NULL, " ", 0.0);
	first_node.first = &first_node;

	std::vector<UserVar> var_nodes;
	var_nodes.reserve(2000); // overkill but it will prevent addresses from changing
	var_nodes.push_back(first_node);

	bool elseStatement = false;

	std::vector<void*> freeable;


	char* rpnln = NULL;
	char* rpnln_head = rpnln;
	size_t lineLen = 0;



	// for each line in the programFile...
	for (;;) {

		// used for line numbers in errors
		line++;

		rpnln = rpnln_head;

		// here there be bugs
		if (getline(&rpnln, &lineLen, program) == -1) { // EOF
			for (void* x : freeable)
				free(x);
			return;
		}

		// I need a copy of it to call free() on later.
		rpnln_head = rpnln;

		char *errorToken = NULL;
		// process the line
		if ((errorToken =
				     processLine(mainStack, var_nodes, errorReporting, rpnln, elseStatement, program, freeable))
		    && errorReporting
				) {


			// file name and
			setTermEffect(TERM_EFF_BOLD);
			std::cerr <<"^^ in " <<programFile <<":" <<line <<':' <<errorToken - rpnln_head <<"\n";
			setTermEffect();

			// print the problem statement
			color_fprintf(stderr, 255, 0, 0, "\t%s", fileutils::getLineFromFile(programFile, line));

			// point to the problem area
			//for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
			//	std::cerr <<' ';

			//color_fputs(stderr, "^\n", 255, 0, 0);

			// windows sucks :P
#ifdef _WIN32
			std::cin.ignore();
#endif

			//for (void* p : freeable) free(p);
			//freeable.clear();

			// you're dead :P
			exit(EXIT_FAILURE);

		}

	}

}




/* from string_stack.hpp
namespace macro {
	typedef enum {
		MACRO, LAMBDA, OTHER
	} exec_t;

	typedef enum {
		SUCCESS, RETURN, ERROR, BREAK
	} ret_t;
}
*/
macro::ret_t runFile(FILE* prog_file, std::vector<UserVar>& var_nodes, bool& errorReporting,
                     std::stack<CalcValue>& mainStack, bool& elseStatement
) {

	if (!prog_file)
		return macro::ERROR;

	std::vector<void*> freeable;

	size_t local_line = 0;

	char* rpnln = NULL, *rpnln_head = rpnln;
	size_t lineLen = 0;

	// for each line in the programFile...
	for (;;) {

		// used for line numbers in errors
		local_line++;

		rpnln = rpnln_head;
		if (getline(&rpnln, &lineLen, prog_file) == -1) {
			for (void* x : freeable)
				free(x);
			free(rpnln);
			return macro::SUCCESS; // EOF
		}

		rpnln_head = rpnln;
		// I need a copy of it to call free() on later.
		char* errorToken = NULL;
		// process the line
		if ((errorToken =
				     processLine(mainStack, var_nodes, errorReporting, rpnln, elseStatement, prog_file, freeable))
		    && errorReporting
				) {

			if (errorToken == lambda_finish) {
				// prevent memory leaks...
				for (auto x : freeable)
					free(x);
				free(rpnln_head);
				if (rpnln == lambda_finish)
					return macro::RETURN;
				else
					return macro::BREAK;
			}
			// info on location
			setTermEffect(TERM_EFF_BOLD);
			std::cerr <<"^^ in " <<progName <<": local_line=" <<local_line
			          <<":col=" <<errorToken - rpnln_head <<"?\n";

			setTermEffect();

			// print the problem statement
			rewind(prog_file);
			color_fprintf(stderr, 255, 0, 0, "\t%s\t",
			              fileutils::getLineFromFile(prog_file, local_line));

/*
			// point to the problem area
			for (uint16_t i = 0; i < errorToken - rpnln_head; i++)
				std::cerr <<' ';

			color_fputs(stderr, "^\n", 255, 0, 0);
*/


			// prevent memory leaks...
			for (void* x : freeable)
				free(x);
			free(rpnln_head);
			return macro::ERROR;
		}
		free(rpnln_head);
		rpnln = rpnln_head = NULL;
	}

	// prevent memory leaks...
	free(rpnln_head);
	return macro::SUCCESS;

}


void runShell(std::vector<UserVar>& var_nodes, bool& errorReporting,
              std::stack<CalcValue>& mainStack, bool& elseStatement, std::vector<void*> freeable
){

	std::cout <<line++ <<">>> ";

	char* rpnln = (char*) malloc(256);
	size_t lineLen = 256;

	if (getline(&rpnln, &lineLen, stdin) == -1) {
		//std::cerr <<"\aERROR: Input failed... email toast27@gmail.com if this persists\n\n";
		std::cout <<"\nGood-bye!\n";
		exit(EXIT_SUCCESS);
		return;
	}

	// I need a copy of it to call free() on later.
	char* rpnln_head = rpnln;

	// process the line
	bool errors = processLine(mainStack, var_nodes, errorReporting, rpnln, elseStatement, stdin, freeable);

	if (errors)
		while (!mainStack.empty())
			mainStack.pop();


	// prevent memory leaks...
	free(rpnln_head);

	// this fails...
	if (!mainStack.empty()) {
		if (!printCalcValue(mainStack.top(), var_nodes))
			std::cout <<'\n';
	}

	std::cout <<std::endl;

}

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
						strutils::deleteChar(current + 1);

						// bel
					} else if (*(current + 1) == 'a') { // "\\a" => "\a"
						*current = '\a';
						strutils::deleteChar(current + 1);

						// backspace
					} else if (*(current + 1) == 'b') { // "\\b" => "\b"
						*current = '\b';
						strutils::deleteChar(current + 1);

						// formfeed
					} else if (*(current + 1) == 'f') { // "\\f" => "\f"
						*current = '\f';
						strutils::deleteChar(current + 1);

						// vertical tab
					} else if (*(current + 1) == 'v') { // "\\v" => "\v"
						*current = '\v';
						strutils::deleteChar(current + 1);

						// escaped backslash
					} else if (*(current + 1) == '\\') // "\\\\" => "\\"
						strutils::deleteChar(current);

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
						strutils::deleteChars(current + 1, ptr - str);

						// escape sequence "\xhh"
					} else if (*(current + 1) == 'x' && isdigit(*(current + 2))) { // "\\hh" => "char(hh)"
						// escape sequence can be a maximum of 2 chars long
						char str[2] = { *(current + 2), *(current + 3) };
						char* ptr;
						// convert the hex literal to single char (8bit int)
						*current = (char) strtol(str, &ptr, 16);
						strutils::deleteChars(current + 1, ptr - str);

						// indicates that the next character should be ignored by the tokenizer
						// this works for \", \', etc.
					} else
						strutils::deleteChar(current);

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


char* runMacro(StrStack* macro, std::stack<CalcValue>& mainStack, std::vector<UserVar> var_nodes, std::vector<void*>& freeable, bool showErrors, bool elseStatement) {
	size_t buff_size = 500;
	char* buff = (char*) malloc(buff_size);
	macro->toString(&buff, &buff_size);

	// put the string in a temp file
	FILE* statement = fileutils::mktmpfile();
	//FILE* statement = tmpfile();
	if (!statement) {
		if (showErrors) {
			printf("\aERROR: tmpfile() returned null. This isn't your fault. :/\n");
			return (char*) macro_error;
		}
	}
	fputs(buff, statement);
	rewind(statement);
	free(buff);


	// add layer to scope
	UserVar first_node(NULL, " ", 0.0);
	var_nodes.push_back(first_node);


	// run the temp file
	macro::ret_t ret = runFile(statement, var_nodes, showErrors, (mainStack), elseStatement);\
	if (ret == macro::ERROR) {
		vars::clearScope(var_nodes);
		fclose(statement);
		return (char*) macro_error;

	} else if (ret == macro::RETURN || ret == macro::BREAK) {
		// variables go out of scope
		vars::clearScope(var_nodes);
		fclose(statement);
		return (char*) lambda_finish;
	}



}


char* spawnMacro(StrStack macro, std::stack<CalcValue> mainStack, std::vector<UserVar> var_nodes, bool showErrors, bool elseStatement) {

	size_t buff_size = 500;
	char* buff = (char*) malloc(buff_size);
	macro->toString(&buff, &buff_size);

	// put the string in a temp file
	FILE* statement = fileutils::mktmpfile();
	//FILE* statement = tmpfile();
	if (!statement) {
		if (showErrors) {
			printf("\aERROR: tmpfile() returned null. This isn't your fault. :/\n");
			return (char*) macro_error;
		}
	}
	fputs(buff, statement);
	rewind(statement);
	free(buff);


	// add layer to scope
	UserVar first_node(NULL, " ", 0.0);
	var_nodes.push_back(first_node);


	// run the temp file
	macro::ret_t ret = runFile(statement, var_nodes, showErrors, (mainStack), elseStatement);\
	if (ret == macro::ERROR) {
		vars::clearScope(var_nodes);
		fclose(statement);
		return (char*) macro_error;

	} else if (ret == macro::RETURN || ret == macro::BREAK) {
		// variables go out of scope
		vars::clearScope(var_nodes);
		fclose(statement);
		return (char*) lambda_finish;
	}



}
