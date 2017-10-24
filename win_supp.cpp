#include "win_supp.h"

#ifdef _WIN32

int getline(char** lineptr, size_t* n, FILE* stream) {

	char *bufptr, *p;

	ssize_t size;
	int c;

	// all parameters are required
	if (!lineptr || !stream || !n)
		return -1;

	bufptr	= *lineptr;
	size 	= *n;
	c 		= fgetc(stream);

	if (c == EOF)
		return -1;

	if (bufptr == NULL) {
		bufptr = (char*) malloc(128);
		if (bufptr == NULL)
			return -1;

		size = 128;
	}

	p = bufptr;

	while (c != EOF) {

		if ((p - bufptr) > (size - 2)) {
			size = size + 128;
			bufptr = (char*) realloc(bufptr, size);
			if (bufptr == NULL)
				return -1;
		}

		*p++ = c;
		if (c == '\n' || c == '\0')
			break;

		c = fgetc(stream);

	}

	*p++	 = '\0';
	*lineptr = bufptr;
	*n		 = size;

	return p - bufptr - 1;

}

#endif // _WIN32

