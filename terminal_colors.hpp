#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H


#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h> // va_args

// these are winshit comptible for windoge 10...
#define COLOR_RESET		"\x1B[0m"
#define COLOR_RED		"\x1B[31m"
#define COLOR_GREEN		"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE		"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN		"\x1B[36m"
#define COLOR_WHITE		"\x1B[37m"

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


// this solution is dependent on endianess, and is thus not cross-platform
typedef struct RGB_t {
		union {
			unsigned int val : 32;

			struct {
				// NOTE: this program does not handle middle-endian and
				// may produce undefined behavior on such archatectures
				#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
					unsigned char b, g, r;
				#else // __ORDER_BIG_ENDIAN__
					unsigned char r, g, b;
				#endif // __ORDER_PDP_ENDIAN__
			};

		};
} RGB_t;

// prints a string in rgb
inline void color_puts(const char* text, uint8_t red, uint8_t green, uint8_t blue)
	{ printf("\x1B[38;2;%d;%d;%dm%s\x1B[0m", red, green, blue, text); }

// prints an rgb format string
void color_printf(const uint8_t red, const uint8_t green, const uint8_t blue, const char* format, ...);
void color_printf(const RGB_t color, const char* format, ...);

// prints a format string in a color defined by a string
void color_printf(const char* color, const char* format, ...);

RGB_t hexToRGB(const char* hex);
RGB_t hex3ToRGB(const char* hex);

// this might get used in the distant future
inline void cycle3(uint8_t& v0, uint8_t& v1, uint8_t& v2, uint8_t& curHi){
	// modify color
	if (curHi == 0)
		{ v0--; v1++; }
	else if (curHi == 1)
		{ v1--; v2++; }
	else if (curHi == 2)
		{ v2--; v0++; }

	// change colors as needed
	if (v0 <= 0 && curHi == 0)
		curHi = 1;
	else if (v1 <= 0 && curHi == 1)
		curHi = 2;
	else if (v2 <= 0 && curHi == 2)
		curHi = 0;
}

const RGB_t nameToColor(const char* cname);

#endif
