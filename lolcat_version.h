#ifndef VERSION_PRINT_H
#define VERSION_PRINT_H

#include <stdio.h>

// ansi colors not supported on windows < 10
#include "win_supp.h"

#define YODA_LANG_VERSION "0.1.7-beta"

// I got this value from `$ lolcat -f -S 27 -F 0.27 ver_info.txt > color.txt && xxd -i color.txt > version_info.h && rm color.txt`

/*contents of "ver_info.txt"
*
Copyright (C) 2016 - 2017 Dustin Van Tate Testa <toast27@gmail.com>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. Check the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>
*/

// why do you have a ~10kb string constant? - because I like rainbows
extern const unsigned char ver_inf_txt[9247];


void printVersionInfo();

#endif