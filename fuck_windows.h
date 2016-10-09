#ifndef FUCK_WINDOWS_H
#define FUCK_WINDOWS_H

/* An open leter to Micro$oft the authors of Win$hit
 * WHY THE FUCK CANT WINDOWS BE POSTIX-COMPATABLE LIKE THE REST OF
 * THE CIVILIZED WORLD? They make me implement my own function
 * just to make this cross-platform... SO SALTY... FML
 */
#ifdef _WIN32

#include <stdlib.h>
#include <stdio.h>


int getline(char **lineptr, size_t *n, FILE *stream) {

	char *bufptr = NULL,
		 *p = bufptr;

	size_t size;
	int c;

	if (lineptr == NULL)
		return -1;

	if (stream == NULL)
		return -1;

	if (n == NULL)
		return -1;

	bufptr	= *lineptr;
	size 	= *n;

	c = fgetc(stream);

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

		if ((p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = (char*) realloc(bufptr, size);
			if (bufptr == NULL)
				return -1;
		}

		*p++ = c;
		if (c == '\n')
			break;

		c = fgetc(stream);

	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;

}

#endif // _WIN32

#endif // FUCK_WINDOWS_H
