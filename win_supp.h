#ifndef FUCK_WINDOWS_H
#define FUCK_WINDOWS_H

/* An open leter to Micro$oft the authors of Win$hit
 * WHY THE FUCK CANT WINDOWS BE POSIX-COMPATIBLE LIKE THE REST OF
 * THE CIVILIZED WORLD? They make me implement my own function
 * just to make this cross-platform...
 */
#ifdef _WIN32

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
//#include <Windows.h>


int getline(char** lineptr, size_t* n, FILE* stream);

//int mkstemp(char*);

#endif // _WIN32

#endif // FUCK_WINDOWS_H