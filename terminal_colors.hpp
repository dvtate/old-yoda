#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h> // va_args

// I'm doubing this supports windoge
// see https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
#define COLOR_RESET		"\x1B[0m"
#define TERM_EFF_RESET		0
#define TERM_EFF_BOLD		1
#define TERM_EFF_FAINT		2
#define TERM_EFF_ITALIC_ON	3
#define TERM_EFF_UNDERLINE	4
#define TERM_EFF_BLINK		5 // < 150 / minute
#define TERM_EFF_FASTBLINK	6 // MS-DOS ANSI.SYS; 150+ per minute; not widely supported
#define TERM_EFF_INVERT		7
#define TERM_EFF_HIDDEN		8
#define TERM_EFF_CROSSOUT	9
#define TERM_EFF_MAIN_FONT	9
#define TERM_EFF_ALT_FONT_1	10
#define TERM_EFF_ALT_FONT_2	11
#define TERM_EFF_ALT_FONT_3	12
#define TERM_EFF_ALT_FONT_4	13
#define TERM_EFF_ALT_FONT_5	14
#define TERM_EFF_ALT_FONT_6	15
#define TERM_EFF_ALT_FONT_7	16
#define TERM_EFF_ALT_FONT_8	17
#define TERM_EFF_ALT_FONT_9	18
#define TERM_EFF_ALT_FONT_10 19
#define TERM_EFF_FRAKTUR	20
// effect#21 = bold:off or Underline:double ||| Bold off not widely supported; double underline hardly ever supported.
#define TERM_EFF_NORM_CLR	22
#define TERM_EFF_ITALIC_OFF	23 // also disables fraktur
#define TERM_EFF_UNDERLINE_NONE 24
#define TERM_EFF_BLINK_OFF	25
// #26 = reserved
#define TERM_EFF_IMG_POS	27
#define TERM_EFF_HIDDEN_OFF	28
#define TERM_EFF_CROSSOUT_OFF 29
// 30 - 37 = reserved for fg colors
// #38 = reserved for fg colors
#define TERM_EFF_DEFAULT_FG	39
// 40 - 47 = reserved for fg colors
// #48 = reserved for bg colors
#define TERM_EFF_DEFAULT_BG	49
// #50 = reserved
#define TERM_EFF_FRAME		51
#define TERM_EFF_ENCIRCLE	52
#define TERM_EFF_OVERLINE	53
#define TERM_EFF_FRAME_ENCIRCLE_OFF	54
#define TERM_EFF_OVERLINE_OFF 55
// 56 - 59 = reserved
// all of the other effects are rarely supported or non-standard




// why do I have to define this in the .cpp?
// it would do perfectly fine here
inline void resetANSI()
	{ printf("\x1B[0m"); }

inline void fresetANSI(FILE* file)
	{ fprintf(file, "\x1B[0m"); }

// sets text effects as defined in the above macros
inline void setTermEffect(const uint8_t eff = TERM_EFF_RESET)
{
	printf("\x1B[%dm", eff);
	atexit(resetANSI);
}
inline void fsetTermEffect(FILE* file, const uint8_t eff = TERM_EFF_RESET)
{
	fprintf(file, "\x1B[%dm", eff);
	atexit(resetANSI);
}


// this solution is dependent on endianess, and is thus not cross-platform
// this structure is used to store a 24bit color.
typedef struct RGB_t {
		union {

			// this is used as a handle. an example application is for converting
			// hex strings to rgb values. (see hexToClr())
			unsigned int val : 24;

			struct {

				// NOTE: this solutiondoes not handle middle-endian and
				// may produce undefined behavior on such archatectures

				#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
					unsigned int b : 8, g : 8, r : 8;
				#else // __ORDER_BIG_ENDIAN__
					unsigned int r : 8, g : 8, b : 8;
				#endif // __ORDER_PDP_ENDIAN__ not supported

			};

		};
} RGB_t;


// prints a string in rgb
inline void color_puts(const char* text, uint8_t red, uint8_t green, uint8_t blue)
	{ printf("\x1B[38;2;%d;%d;%dm%s\x1B[0m", red, green, blue, text); }
inline void color_fputs(FILE* file, const char* text, uint8_t red, uint8_t green, uint8_t blue)
	{ fprintf(file, "\x1B[38;2;%d;%d;%dm%s\x1B[0m", red, green, blue, text); }

// prints an rgb format string
void color_printf(const uint8_t red, const uint8_t green, const uint8_t blue, const char* format, ...);
void color_printf(const RGB_t color, const char* format, ...);

// prints a format string in a color defined by a string
void color_printf(const char* color, const char* format, ...);

// prints an rgb format string to a file
void color_fprintf(FILE* file, const uint8_t red, const uint8_t green, const uint8_t blue, const char* format, ...);
void color_fprintf(FILE* file, const RGB_t color, const char* format, ...);

// prints a format string in a color defined by a string to a file
void color_fprintf(FILE* file, const char* color, const char* format, ...);





// making colors
RGB_t hexToRGB(const char* hex);
RGB_t hex3ToRGB(const char* hex);
const RGB_t nameToColor(const char* cname);


// change the forground color
void fsetFgColor(FILE* file, const uint8_t red, const uint8_t green, const uint8_t blue);
void fsetFgColor(FILE* file, const RGB_t color);
void fsetFgColor(FILE* file, const char* color);
inline void setFgColor(FILE* file)
	{  fsetTermEffect(file, TERM_EFF_DEFAULT_FG); }
void setFgColor(const uint8_t red, const uint8_t green, const uint8_t blue);
void setFgColor(const RGB_t color);
inline void setFgColor(const char* ccolor)
	{ return fsetFgColor(stdout, ccolor); }
inline void setFgColor()
	{  setTermEffect(TERM_EFF_DEFAULT_FG); }


// change the background color
void fsetBgColor(FILE* file, const uint8_t red, const uint8_t green, const uint8_t blue);
void fsetBgColor(FILE* file, const RGB_t color);
void fsetBgColor(FILE* file, const char* color);
inline void setBgColor(FILE* file)
	{  fsetTermEffect(file, TERM_EFF_DEFAULT_BG); }
void setBgColor(const uint8_t red, const uint8_t green, const uint8_t blue);
void setBgColor(const RGB_t color);
inline void setBgColor(const char* ccolor)
	{ return fsetBgColor(stdout, ccolor); }
inline void setBgColor()
	{  setTermEffect(TERM_EFF_DEFAULT_BG); }

// this might get used in the distant future
static inline void cycle3(uint8_t& v0, uint8_t& v1, uint8_t& v2, uint8_t& curHi)
{
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
