#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

// these are winshit comptible for windoge 10...
#define COLOR_RESET		"\x1B[0m"
#define COLOR_RED		"\x1B[31m"
#define COLOR_GREEN		"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE		"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN		"\x1B[36m"
#define COLOR_WHITE		"\x1B[37m"

#include <stdio.h>
#include <inttypes.h>
#define TERM_CLR_RESET		0
#define TERM_CLR_BRIGHT 	1
#define TERM_CLR_DIM		2
#define TERM_CLR_UNDERLINE	3
#define TERM_CLR_BLINK		4
#define TERM_CLR_REVERSE	7
#define TERM_CLR_HIDDEN		8


inline void textColor(uint8_t attr, uint8_t fg, uint8_t bg)
	{ printf("\x1B[%d;%d;%dm", attr, fg + 30, bg + 40); }

inline void textColor(uint8_t attr, uint8_t fg)
	{ printf("\x1B[%d;%dm", attr, fg + 30); }

inline void textColor(uint8_t fg)
	{ printf("\x1B[%dm", fg + 30); }

inline void textEffect(uint8_t eff)
	{ printf("\x1B[%dm", eff); }

inline void textColor()
	{ printf(COLOR_RESET); }

inline void textEffect()
	{ textColor(); }




inline void color_puts(const char* text, uint8_t red, uint8_t green, uint8_t blue){
		printf("\x1B[38;2;%d;%d;%dm%s\x1B[0m", red, green, blue, text); 
}	

// prints an rgb format string
void color_printf(uint8_t red, uint8_t green, uint8_t blue, const char* format, ...){

	va_list args;
	va_start(args, format);

	printf("\x1B[38;2;%d;%d;%dm", red, green, blue); // set color

	vprintf(format, args); // print the format

	va_end(args);

	printf(COLOR_RESET); // reset color

}	


namespace colors {
	// this might get used in the distant future
	inline void cycle3(uint8_t& v0, uint8_t& v1, uint8_t& v2, uint8_t& curHi){

		if (curHi == 0) {
			v0--; v1++;
		} else if (curHi == 1) {
			v1--; v2++;
		} else if (curHi == 2) {
			v2--; v0++;
		}

		if (v0 <= 0 && curHi == 0)
			curHi = 1;
		else if (v1 <= 0 && curHi == 1)
			curHi = 2;
		else if (v2 <= 0 && curHi == 2)
			curHi = 0;
	}
}

#endif
