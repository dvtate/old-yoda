#ifndef _WIN32

#include <termios.h>
#include <stdio.h>

#ifdef __WIN32
	#include <conio.h>

	namespace terminp {

		// read 1 character without echo
		char getch(void) {
			return getch();
		}

		// read 1 character with echo
		char getche(void){
			return getche();
		}
	}
#else
	namespace terminp {
		static struct termios old, modified;

		/* Initialize new terminal i/o settings */
		static void initTermios(bool echo){
			tcgetattr(0, &old); /* grab old terminal i/o settings */
			modified = old; /* make new settings same as old settings */
			modified.c_lflag &= ~ICANON; /* disable buffered i/o */
			modified.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
			tcsetattr(0, TCSANOW, &modified); /* use these new terminal i/o settings now */
		}

		/* Restore old terminal i/o settings */
		static void resetTermios(void){
			tcsetattr(0, TCSANOW, &old);
		}

		// read 1 character - echo defines echo mode
		static char getch_(bool echo){
			char ch;
			initTermios(echo);
			ch = getchar();
			resetTermios();
			return ch;
		}

		// read 1 character without echo
		char getch(void){
			return getch_(0);
		}

		// read 1 character with echo
		char getche(void){
			return getch_(1);
		}

	}
#endif

#endif