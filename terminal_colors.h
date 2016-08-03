#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

// idk if these are winshit comptible
#define COLOR_RESET		"\x1B[0m"
#define COLOR_RED		"\x1B[31m"
#define COLOR_GREEN		"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE		"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN		"\x1B[36m"
#define COLOR_WHITE		"\x1B[37m"


#include <cstdio>

#define TERM_CLR_RESET		0
#define TERM_CLR_BRIGHT 	1
#define TERM_CLR_DIM		2
#define TERM_CLR_UNDERLINE	3
#define TERM_CLR_BLINK		4
#define TERM_CLR_REVERSE	7
#define TERM_CLR_HIDDEN		8


inline void textColor(uint8_t attr, uint8_t fg, uint8_t bg){
	printf("\x1B[%d;%d;%dm",
		attr,
		fg + 30,
		bg + 40
	);
}

inline void textColor(uint8_t attr, uint8_t fg){
	printf("\x1B[%d;%dm",
		attr,
		fg + 30
	);
}

inline void textColor(uint8_t fg){
	printf("\x1B[%dm",
		fg + 30
	);
}

inline void textEffect(uint8_t eff){
	printf("\x1B[%dm",
		eff
	);
}

inline void textColor(){
	puts(COLOR_RESET);
}
inline void textEffect(){
  	textColor();
}

#endif
