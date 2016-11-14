#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h> // va_args

// I'm doubing this supports windoge as linux support is also lacking
#define COLOR_RESET		"\x1B[0m"
#define TERM_EFF_CLR_RESET	0
#define TERM_EFF_BOLD		1
#define TERM_EFF_FAINT		2
#define TERM_EFF_ITALIC_ON	3
#define TERM_EFF_UNDERLINE	4
#define TERM_EFF_BLINK		5
#define TERM_EFF_FASTBLINK	6
#define TERM_EFF_INVERT		7
#define TERM_EFF_HIDDEN		8
#define TERM_EFF_CROSSOUT	9
#define TERM_EFF_MAIN_FONT	9


void resetASCII();
inline void setEffect(uint8_t eff)
{
	printf("\x1B[%dm", eff);
	atexit(resetASCII);
}




// this solution is dependent on endianess, and is thus not cross-platform
typedef struct RGB_t {
		union {
			unsigned int val : 32;

			struct {

				// NOTE: this program does not handle middle-endian and
				// may produce undefined behavior on such archatectures
				// TBH, I'm not even sure it would work on anything but
				// little-endian, as I've only tested on x86_64 and ARM

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

// making colors
RGB_t hexToRGB(const char* hex);
RGB_t hex3ToRGB(const char* hex);
const RGB_t nameToColor(const char* cname);


// change the forground color
void setFgColor(const uint8_t red, const uint8_t green, const uint8_t blue);
void setFgColor(const RGB_t color);
void setFgColor(const char* color);

// change the background color
void setBgColor(const uint8_t red, const uint8_t green, const uint8_t blue);
void setBgColor(const RGB_t color);
void setBgColor(const char* color);


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


#endif
