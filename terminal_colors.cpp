#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>


#include "terminal_colors.hpp"


// prints an rgb format string
void color_printf(const uint8_t red, const uint8_t green, const uint8_t blue, const char* format, ...){
	printf("\x1B[38;2;%d;%d;%dm", red, green, blue); // set color

	va_list args;
	va_start(args, format);
	vprintf(format, args); // print the format
	va_end(args);

	printf(COLOR_RESET); // reset color

}


// prints an rgb format string
void color_printf(const uint8_t red, const uint8_t green, const uint8_t blue, const char* format, va_list args){
	printf("\x1B[38;2;%d;%d;%dm", red, green, blue); // set color

	vprintf(format, args); // print the format
	va_end(args);

	printf(COLOR_RESET); // reset color

}

// prints an rgb format string
void color_printf(const RGB_t color, const char* format, va_list args){
	printf("\x1B[38;2;%d;%d;%dm", color.r, color.g, color.b); // set color

	vprintf(format, args); // print the format
	va_end(args);

	printf(COLOR_RESET); // reset color

}


void color_printf(const RGB_t color, const char* format, ...){
	printf("\x1B[38;2;%d;%d;%dm", color.r, color.g, color.b); // set color

	va_list args;
	va_start(args, format);
	vprintf(format, args); // print the format
	va_end(args);

	printf(COLOR_RESET); // reset color


}

static unsigned short int countSpaces(char* str){
	if (str == NULL)
		return 0;

	unsigned short int ret = 0;
	while (*(str++))
		if (isspace(*(str++)))
			ret++;

	return ret;

}


static inline bool notBlack(char* clr){
	if (clr) {
		for (int16_t i = strlen(clr); i >= 0; i--)
			*(clr + i) = tolower(*(clr + i));

		return strcmp(clr, "black");
	}

	return true;
}


RGB_t hexToClr(const char* hex){
	RGB_t ret = { 0 };

	for (uint8_t i = 0; i < 6; i++) {
		ret.val <<= 4;
		switch (hex[i]){
		case '0': break;
		case '1': ret.val += 1; break;
		case '2': ret.val += 2; break;
		case '3': ret.val += 3; break;
		case '4': ret.val += 4; break;
		case '5': ret.val += 5; break;
		case '6': ret.val += 6; break;
		case '7': ret.val += 7; break;
		case '8':
			ret.val += 8;
			break;
		case '9':
			ret.val += 9;
			break;
		case 'A': case 'a':
			ret.val += 10;
			break;
		case 'B': case 'b':
			ret.val += 11;
			break;
		case 'C': case 'c':
			ret.val += 12;
			break;
		case 'D': case 'd':
			ret.val += 13;
			break;
		case 'E': case 'e':
			ret.val += 14;
			break;
		case 'F': case 'f':
			ret.val += 15;
			break;
		default:
			return (RGB_t) { 0 };
		}
	}
	return ret;
}

RGB_t hex3ToClr(const char* hex){
	RGB_t ret = {0};

	for (uint8_t i = 0; i < 3; i++) {
		for (uint8_t b = 0; b < 2; b++){
			ret.val <<= 4;
			switch (hex[i]){
			case '0': break;
			case '1': ret.val += 1; break;
			case '2': ret.val += 2; break;
			case '3': ret.val += 3; break;
			case '4': ret.val += 4; break;
			case '5': ret.val += 5; break;
			case '6': ret.val += 6; break;
			case '7': ret.val += 7; break;
			case '8': ret.val += 8; break;
			case '9': ret.val += 9; break;
			case 'A': case 'a':
				ret.val += 10;
				break;
			case 'B': case 'b':
				ret.val += 11;
				break;
			case 'C': case 'c':
				ret.val += 12;
				break;
			case 'D': case 'd':
				ret.val += 13;
				break;
			case 'E': case 'e':
				ret.val += 14;
				break;
			case 'F': case 'f':
				ret.val += 15;
				break;
			default:
				return (RGB_t) { 0 };
			}
		}
	}
	return ret;
}
void color_printf(const char* ccolor, const char* format, ...){
	va_list args;
	va_start(args, format);

	char* color = (char*) malloc(strlen(ccolor));
	char* color_cpy = color;
	strcpy(color, ccolor);

	// skip leading spaces
	while (*color && isspace(*color))
		color++;

	// delete trailing spaces
	while (isspace(color[strlen(color) - 1]))
		color[strlen(color) - 1] = '\0';

	// no color given, this could be desired (pass no error)
	if (strlen(color) == 0) {
		vprintf(format, args); // print the format
		return;
	}

	// hex color
	if (*color == '#') {
		color++;
		if (strlen(color) == 3) {
			color_printf(hex3ToClr(color), format, args);
		} else if (strlen(color) == 6) {
			color_printf(hexToClr(color), format, args);
		} else {
			std::cerr <<"\aERROR: invalid hex color \"" <<(color - 1) <<"\".\n";
			vprintf(format, args); // print the format
		}
		va_end(args);
		return;
	}

	// rgb ()
	else if (strlen(color) >= 6
		&& (*color == 'r' || *color == 'R')
		&& (*(color + 1) == 'g' || *(color + 1) == 'G')
		&& (*(color + 2) == 'b' || *(color + 2) == 'B')
	){
		color += 3;

		// skip spaces
		while (*color && isspace(*color))
			color++;

		// skip to the first number
		if (*color && !isdigit(*color))
			color++;


		char* token = strtok(color, ", ");
		uint8_t vals[3];
		for (uint8_t i = 0; i < 3; i++) {

			if (token == NULL || *token == ')') {
				std::cerr <<"\aERROR: rgb() expected 3 arguments, " <<i + 1 <<" provided.";
				break;
			}
			vals[i] = atoi(token);
			token = strtok(NULL, ", ");
		}
		color_printf(vals[0], vals[1], vals[2], format, args);
		va_end(args);
		goto end_printf;
	}

	// color by name (worst performance)
	else if (!isdigit(*color)) {
		RGB_t clr = nameToColor(color);
		if (clr.val == 0 && notBlack(color))
			std::cerr <<"\aERROR: invalid HTML color. `" <<color <<"` doesn't name a color.";
		color_printf(clr, format, args);
		va_end(args);
		goto end_printf;
	}

end_printf:
	free(color_cpy);

}


const RGB_t nameToColor(const char* cname){
	if (!cname)
		return (RGB_t) { 0 };

	char name[strlen(cname) + 1];
	strcpy(name, cname);

	// set name to lowercase
	for (size_t i = 0; i <= strlen(name); i++)
		*(name + i) = tolower(*(name + i));

	// most used colors come first
	if (strcmp(name, "black") == 0) // useful
		return (RGB_t) { 0 };
	else if (strcmp(name, "white") == 0) // useful
		return (RGB_t) { 16777215 };
	else if (strcmp(name, "red") == 0) // useful
		return (RGB_t) { 16711680 };
	else if (strcmp(name, "cyan") == 0 || strcmp(name, "aqua") == 0) // useful
		return (RGB_t) { 65535 };
	else if (strcmp(name, "magenta") == 0 || strcmp(name, "fuchsia") == 0)//useful
		return (RGB_t) { 16711935 };
	else if (strcmp(name, "blue") == 0) // useful
		return (RGB_t) { 255 };
	else if (strcmp(name, "gray") == 0)  // useful
		return (RGB_t) { 8421504 };
	else if (strcmp(name, "yellow") == 0) // useful
		return (RGB_t) { 16776960 };
	else if (strcmp(name, "green") == 0) // useful
		return (RGB_t) { 32768 };
	else if (strcmp(name, "pink") == 0) // useful
		return (RGB_t) { 16761035 };
	else if (strcmp(name, "orange") == 0) // useful
		return (RGB_t) { 16753920 };
	else if (strcmp(name, "gold") == 0) // useful
		return (RGB_t) { 16766720 };
	else if (strcmp(name, "purple") == 0) // useful
		return (RGB_t) { 8388736 };
	else if (strcmp(name, "indigo") == 0) // useful
		return (RGB_t) { 4915330 };
	else if (strcmp(name, "brown") == 0) // useful
		return (RGB_t) { 10824234 };
	else if (strcmp(name, "navy") == 0) // useful
		return (RGB_t) { 128 };


	// reds
	if (strcmp(name, "indianred") == 0)
		return (RGB_t) { 13458524 };
	else if (strcmp(name, "lightcoral") == 0)
		return (RGB_t) { 15761536 };
	else if (strcmp(name, "salmon") == 0)
		return (RGB_t) { 16416882 };
	else if (strcmp(name, "darksalmon") == 0)
		return (RGB_t) { 15308410 };
	else if (strcmp(name, "lightsalmon") == 0)
		return (RGB_t) { 16752762 };
	else if (strcmp(name, "crimson") == 0)
		return (RGB_t) { 14423100 };
	else if (strcmp(name, "darkred") == 0)
		return (RGB_t) { 9109504 };
	else if (strcmp(name, "firebrick") == 0)
		return (RGB_t) { 11674146 };

	// pinks
	else if (strcmp(name, "lightpink") == 0)
		return (RGB_t) { 16758465 };
	else if (strcmp(name, "hotpink") == 0)
		return (RGB_t) { 16738740 };
	else if (strcmp(name, "deeppink") == 0)
		return (RGB_t) { 16716947 };
	else if (strcmp(name, "mediumvioletred") == 0)
		return (RGB_t) { 13047173 };
	else if (strcmp(name, "palevioletred") == 0)
		return (RGB_t) { 14381203 };

	// oranges
	else if (strcmp(name, "lightsalmon") == 0)
		return (RGB_t) { 16752762 };
	else if (strcmp(name, "coral") == 0)
		return (RGB_t) { 16744272 };
	else if (strcmp(name, "tomato") == 0)
		return (RGB_t) { 16737095 };
	else if (strcmp(name, "orangered") == 0)
		return (RGB_t) { 16729344 };
	else if (strcmp(name, "darkorange") == 0)
		return (RGB_t) { 16747520 };

	// yellows
	else if (strcmp(name, "lightyellow") == 0)
		return (RGB_t) { 16777184 };
	else if (strcmp(name, "lightgoldenrodyellow") == 0)
		return (RGB_t) { 16775885 };
	else if (strcmp(name, "papayawhip") == 0)
		return (RGB_t) { 16773077 };
	else if (strcmp(name, "moccasin") == 0)
		return (RGB_t) { 16770229 };
	else if (strcmp(name, "peachpuff") == 0)
		return (RGB_t) { 16767673 };
	else if (strcmp(name, "palegoldenrod") == 0)
		return (RGB_t) { 15657130 };
	else if (strcmp(name, "khaki") == 0)
		return (RGB_t) { 15787660 };
	else if (strcmp(name, "darkkhaki") == 0)
		return (RGB_t) { 12433259 };

	// purples
	else if (strcmp(name, "lavender") == 0)
		return (RGB_t) { 15132410 };
	else if (strcmp(name, "thistle") == 0)
		return (RGB_t) { 14204888 };
	else if (strcmp(name, "plum") == 0)
		return (RGB_t) { 14524637 };
	else if (strcmp(name, "violet") == 0)
		return (RGB_t) { 15631086 };
	else if (strcmp(name, "orchid") == 0)
		return (RGB_t) { 14315734 };
	else if (strcmp(name, "mediumorchid") == 0)
		return (RGB_t) { 12211667 };
	else if (strcmp(name, "mediumpurple") == 0)
		return (RGB_t) { 9662683 };
	else if (strcmp(name, "rebeccapurple") == 0)
		return (RGB_t) { 6697881 };
	else if (strcmp(name, "blueviolet") == 0)
		return (RGB_t) { 9055202 };
	else if (strcmp(name, "darkviolet") == 0)
		return (RGB_t) { 9699539 };
	else if (strcmp(name, "darkorchid") == 0)
		return (RGB_t) { 10040012 };
	else if (strcmp(name, "darkmagenta") == 0)
		return (RGB_t) { 9109643 };
	else if (strcmp(name, "slateblue") == 0)
		return (RGB_t) { 6970061 };
	else if (strcmp(name, "darkslateblue") == 0)
		return (RGB_t) { 4734347 };
	else if (strcmp(name, "mediumslateblue") == 0)
		return (RGB_t) { 8087790 };

	// greens
	else if (strcmp(name, "greenyellow") == 0)
		return (RGB_t) { 11403055 };
	else if (strcmp(name, "chartreuse") == 0)
		return (RGB_t) { 8388352 };
	else if (strcmp(name, "lawngreen") == 0)
		return (RGB_t) { 8190976 };
	else if (strcmp(name, "lime") == 0)
		return (RGB_t) { 65280 };
	else if (strcmp(name, "limegreen") == 0)
		return (RGB_t) { 3329330 };
	else if (strcmp(name, "palegreen") == 0)
		return (RGB_t) { 10025880 };
	else if (strcmp(name, "lightgreen") == 0)
		return (RGB_t) { 9498256 };
	else if (strcmp(name, "mediumspringgreen") == 0)
		return (RGB_t) { 64154 };
	else if (strcmp(name, "springgreen") == 0)
		return (RGB_t) { 64154 };
	else if (strcmp(name, "mediumseagreen") == 0)
		return (RGB_t) { 65407 };
	else if (strcmp(name, "forestgreen") == 0)
		return (RGB_t) { 2263842 };
	else if (strcmp(name, "darkgreen") == 0)
		return (RGB_t) { 25600 };
	else if (strcmp(name, "yellowgreen") == 0)
		return (RGB_t) { 10145074 };
	else if (strcmp(name, "olivedarb") == 0)
		return (RGB_t) { 7048739 };
	else if (strcmp(name, "olive") == 0)
		return (RGB_t) { 8421376 };
	else if (strcmp(name, "darkolivegreen") == 0)
		return (RGB_t) { 5597999 };
	else if (strcmp(name, "mediumaquamarine") == 0)
		return (RGB_t) { 6737322 };
	else if (strcmp(name, "darkseagreen") == 0)
		return (RGB_t) { 9419915 };
	else if (strcmp(name, "lightseagreen") == 0)
		return (RGB_t) { 2142890 };
	else if (strcmp(name, "darkcyan") == 0)
		return (RGB_t) { 35723 };
	else if (strcmp(name, "teal") == 0)
		return (RGB_t) { 32896 };

	// blues
	else if (strcmp(name, "lightcyan") == 0)
		return (RGB_t) { 14745599 };
	else if (strcmp(name, "paleturquoise") == 0)
		return (RGB_t) { 11529966 };
	else if (strcmp(name, "aquamarine") == 0)
		return (RGB_t) { 8388564 };
	else if (strcmp(name, "turquoise") == 0)
		return (RGB_t) { 4251856 };
	else if (strcmp(name, "mediumturquoise") == 0)
		return (RGB_t) { 4772300 };
	else if (strcmp(name, "darkturquoise") == 0)
		return (RGB_t) { 52945 };
	else if (strcmp(name, "cadetblue") == 0)
		return (RGB_t) { 6266528 };
	else if (strcmp(name, "steelblue") == 0)
		return (RGB_t) { 4620980 };
	else if (strcmp(name, "lightsteelblue") == 0)
		return (RGB_t) { 11584734 };
	else if (strcmp(name, "powderblue") == 0)
		return (RGB_t) { 11591910 };
	else if (strcmp(name, "lightblue") == 0)
		return (RGB_t) { 11393254 };
	else if (strcmp(name, "skyblue") == 0)
		return (RGB_t) { 8900331 };
	else if (strcmp(name, "lightskyblue") == 0)
		return (RGB_t) { 8900346 };
	else if (strcmp(name, "deepskyblue") == 0)
		return (RGB_t) { 49151 };
	else if (strcmp(name, "dodgerblue") == 0)
		return (RGB_t) { 2003199 };
	else if (strcmp(name, "cornflowerblue") == 0)
		return (RGB_t) { 6591981 };
	else if (strcmp(name, "mediumslateblue") == 0)
		return (RGB_t) { 8087790 };
	else if (strcmp(name, "royalblue") == 0)
		return (RGB_t) { 4286945 };
	else if (strcmp(name, "mediumblue") == 0)
		return (RGB_t) { 205 };
	else if (strcmp(name, "darkblue") == 0)
		return (RGB_t) { 139 };
	else if (strcmp(name, "midnightblue") == 0)
		return (RGB_t) { 1644912 };

	// browns
	else if (strcmp(name, "cornsilk") == 0)
		return (RGB_t) { 16775388 };
	else if (strcmp(name, "blanchedalmond") == 0)
		return (RGB_t) { 16772045 };
	else if (strcmp(name, "bisque") == 0)
		return (RGB_t) { 16770244 };
	else if (strcmp(name, "navajowhite") == 0)
		return (RGB_t) { 16768685 };
	else if (strcmp(name, "wheat") == 0)
		return (RGB_t) { 16113325 };
	else if (strcmp(name, "burlywood") == 0)
		return (RGB_t) { 14596231 };
	else if (strcmp(name, "tan") == 0)
		return (RGB_t) { 13808780 };
	else if (strcmp(name, "rosybrown") == 0)
		return (RGB_t) { 12357519 };
	else if (strcmp(name, "sandybrown") == 0)
		return (RGB_t) { 16032864 };
	else if (strcmp(name, "goldenrod") == 0)
		return (RGB_t) { 14329120 };
	else if (strcmp(name, "peru") == 0)
		return (RGB_t) { 13468991 };
	else if (strcmp(name, "chocolate") == 0)
		return (RGB_t) { 13789470 };
	else if (strcmp(name, "saddlebrown") == 0)
		return (RGB_t) { 9127187 };
	else if (strcmp(name, "sienna") == 0)
		return (RGB_t) { 10506797 };
	else if (strcmp(name, "maroon") == 0)
		return (RGB_t) { 8388608 };

	// whites
	else if (strcmp(name, "snow") == 0)
		return (RGB_t) { 16775930 };
	else if (strcmp(name, "honeydew") == 0)
		return (RGB_t) { 15794160 };
	else if (strcmp(name, "mintcream") == 0)
		return (RGB_t) { 16121850 };
	else if (strcmp(name, "azure") == 0)
		return (RGB_t) { 15794175 };
	else if (strcmp(name, "aliceblue") == 0)
		return (RGB_t) { 15792383 };
	else if (strcmp(name, "ghostwhite") == 0)
		return (RGB_t) { 16316671 };
	else if (strcmp(name, "whitesmoke") == 0)
		return (RGB_t) { 16119285 };
	else if (strcmp(name, "seashell") == 0)
		return (RGB_t) { 16774638 };
	else if (strcmp(name, "beige") == 0)
		return (RGB_t) { 16119260 };
	else if (strcmp(name, "oldlace") == 0)
		return (RGB_t) { 16643558 };
	else if (strcmp(name, "floralwhite") == 0)
		return (RGB_t) { 16775920 };
	else if (strcmp(name, "ivory") == 0)
		return (RGB_t) { 16777200 };
	else if (strcmp(name, "antiquewhite") == 0)
		return (RGB_t) { 16444375 };
	else if (strcmp(name, "linen") == 0)
		return (RGB_t) { 16445670 };
	else if (strcmp(name, "lavenderblush") == 0)
		return (RGB_t) { 16773365 };
	else if (strcmp(name, "mistyrose") == 0)
		return (RGB_t) { 16770273 };

	// greys
	else if (strcmp(name, "gainsboro") == 0)
		return (RGB_t) { 14474460 };
	else if (strcmp(name, "lightgrey") == 0)
		return (RGB_t) { 13882323 };
	else if (strcmp(name, "silver") == 0)
		return (RGB_t) { 12632256 };
	else if (strcmp(name, "darkgrey") == 0)
		return (RGB_t) { 11119017 };
	else if (strcmp(name, "dimgrey") == 0)
		return (RGB_t) { 6908265 };
	else if (strcmp(name, "lightslategrey") == 0)
		return (RGB_t) { 7833753 };
	else if (strcmp(name, "slategrey") == 0)
		return (RGB_t) { 7372944 };
	else if (strcmp(name, "darkslategrey") == 0)
		return (RGB_t) { 3100495 };
	else
		return (RGB_t) { 0 };

}
