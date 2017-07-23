#ifndef YODA_TERMIO_HPP
#define YODA_TERMIO_HPP

	#ifdef _WIN32
	#include <conio.h>

	#else
	// not sure if this will work on OSX or not...

	namespace termio {

		// read 1 character without echo
		extern char getch(void);

		// read 1 character with echo
		extern char getche(void);
	}

	#endif


#endif //YODA_TERMIO_HPP
