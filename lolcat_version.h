#ifndef VERSION_PRINT_H
#define VERSION_PRINT_H

#define SBS_LANG_VERSION "0.0.3-alpha"

#include <stdio.h>
void printVersionInfo(){
	// I got this value from `$ lolcat -f -S 27 -F 0.27 ver_info.txt > color.txt && cat color.txt`
 	/*contents of "ver_info.txt"
	*
	*	Stack Based Scripting Language  - v0.0.3-alpha
	*	Copyright (C) 2016 Dustin Van Tate Testa <toast27@gmail.com>
	*	This program is free software: you can redistribute it and/or modify
	*	it under the terms of the GNU General Public License as published by
	*	the Free Software Foundation.
	*
	*	This program is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. Check the
	*	GNU General Public License for more details.
	*
	*	You should have received a copy of the GNU General Public License
	*	along with this program. If not, see <http://www.gnu.org/licenses/>
	*
	*/
	puts(
"\x1B[38;5;208mS\x1B[0m\x1B[38;5;209mt\x1B[0m\x1B[38;5;203ma\x1B[0m\x1B[38;5;203mc\
\x1B[0m\x1B[38;5;203mk\x1B[0m\x1B[38;5;203m \x1B[0m\x1B[38;5;198mB\x1B[0m\x1B[38;5;198ma\
\x1B[0m\x1B[38;5;198ms\x1B[0m\x1B[38;5;198me\x1B[0m\x1B[38;5;199md\x1B[0m\x1B[38;5;199m \
\x1B[0m\x1B[38;5;199mS\x1B[0m\x1B[38;5;164mc\x1B[0m\x1B[38;5;164mr\x1B[0m\x1B[38;5;164mi\
\x1B[0m\x1B[38;5;164mp\x1B[0m\x1B[38;5;128mt\x1B[0m\x1B[38;5;129mi\x1B[0m\x1B[38;5;129mn\
\x1B[0m\x1B[38;5;129mg\x1B[0m\x1B[38;5;93m \x1B[0m\x1B[38;5;93mL\x1B[0m\x1B[38;5;93ma\x1B\
[0m\x1B[38;5;93mn\x1B[0m\x1B[38;5;63mg\x1B[0m\x1B[38;5;63mu\x1B[0m\x1B[38;5;63ma\x1B[0m\
\x1B[38;5;63mg\x1B[0m\x1B[38;5;33me\x1B[0m\x1B[38;5;33m \x1B[0m\x1B[38;5;33m \x1B[0m\
\x1B[38;5;33m-\x1B[0m\x1B[38;5;39m \x1B[0m\x1B[38;5;39mv\x1B[0m\x1B[38;5;39m0\x1B[0m\
\x1B[38;5;38m.\x1B[0m\x1B[38;5;44m0\x1B[0m\x1B[38;5;44m.\x1B[0m\x1B[38;5;44m3\x1B[0m\
\x1B[38;5;44m-\x1B[0m\x1B[38;5;49ma\x1B[0m\x1B[38;5;49ml\x1B[0m\x1B[38;5;49mp\x1B[0m\
\x1B[38;5;48mh\x1B[0m\x1B[38;5;48ma\x1B[0m\n\
\x1B[38;5;203mC\x1B[0m\x1B[38;5;203mo\x1B[0m\x1B[38;5;203mp\x1B[0m\x1B[38;5;198my\
\x1B[0m\x1B[38;5;198mr\x1B[0m\x1B[38;5;198mi\x1B[0m\x1B[38;5;198mg\x1B[0m\
\x1B[38;5;199mh\x1B[0m\x1B[38;5;199mt\x1B[0m\x1B[38;5;199m \x1B[0m\x1B[38;5;164m(\
\x1B[0m\x1B[38;5;164mC\x1B[0m\x1B[38;5;164m)\x1B[0m\x1B[38;5;164m \x1B[0m\
\x1B[38;5;128m2\x1B[0m\x1B[38;5;129m0\x1B[0m\x1B[38;5;129m1\x1B[0m\x1B[38;5;129m6\
\x1B[0m\x1B[38;5;93m \x1B[0m\x1B[38;5;93mD\x1B[0m\x1B[38;5;93mu\x1B[0m\x1B[38;5;93ms\
\x1B[0m\x1B[38;5;63mt\x1B[0m\x1B[38;5;63mi\x1B[0m\x1B[38;5;63mn\x1B[0m\x1B[38;5;63m \
\x1B[0m\x1B[38;5;33mV\x1B[0m\x1B[38;5;33ma\x1B[0m\x1B[38;5;33mn\x1B[0m\x1B[38;5;33m \
\x1B[0m\x1B[38;5;39mT\x1B[0m\x1B[38;5;39ma\x1B[0m\x1B[38;5;39mt\x1B[0m\x1B[38;5;38me\
\x1B[0m\x1B[38;5;44m \x1B[0m\x1B[38;5;44mT\x1B[0m\x1B[38;5;44me\x1B[0m\x1B[38;5;44ms\
\x1B[0m\x1B[38;5;49mt\x1B[0m\x1B[38;5;49ma\x1B[0m\x1B[38;5;49m \x1B[0m\x1B[38;5;48m<\
\x1B[0m\x1B[38;5;48mt\x1B[0m\x1B[38;5;48mo\x1B[0m\x1B[38;5;48ma\x1B[0m\x1B[38;5;83ms\
\x1B[0m\x1B[38;5;83mt\x1B[0m\x1B[38;5;83m2\x1B[0m\x1B[38;5;83m7\x1B[0m\x1B[38;5;119m@\
\x1B[0m\x1B[38;5;118mg\x1B[0m\x1B[38;5;118mm\x1B[0m\x1B[38;5;118ma\x1B[0m\
\x1B[38;5;154mi\x1B[0m\x1B[38;5;154ml\x1B[0m\x1B[38;5;154m.\x1B[0m\x1B[38;5;148mc\
\x1B[0m\x1B[38;5;184mo\x1B[0m\x1B[38;5;184mm\x1B[0m\x1B[38;5;184m>\x1B[0m\n\
\x1B[38;5;198mT\x1B[0m\x1B[38;5;198mh\x1B[0m\x1B[38;5;198mi\x1B[0m\x1B[38;5;198ms\
\x1B[0m\x1B[38;5;199m \x1B[0m\x1B[38;5;199mp\x1B[0m\x1B[38;5;199mr\x1B[0m\
\x1B[38;5;164mo\x1B[0m\x1B[38;5;164mg\x1B[0m\x1B[38;5;164mr\x1B[0m\x1B[38;5;164ma\
\x1B[0m\x1B[38;5;128mm\x1B[0m\x1B[38;5;129m \x1B[0m\x1B[38;5;129mi\x1B[0m\
\x1B[38;5;129ms\x1B[0m\x1B[38;5;93m \x1B[0m\x1B[38;5;93mf\x1B[0m\x1B[38;5;93mr\
\x1B[0m\x1B[38;5;93me\x1B[0m\x1B[38;5;63me\x1B[0m\x1B[38;5;63m \x1B[0m\x1B[38;5;63ms\
\x1B[0m\x1B[38;5;63mo\x1B[0m\x1B[38;5;33mf\x1B[0m\x1B[38;5;33mt\x1B[0m\x1B[38;5;33mw\
\x1B[0m\x1B[38;5;33ma\x1B[0m\x1B[38;5;39mr\x1B[0m\x1B[38;5;39me\x1B[0m\x1B[38;5;39m:\
\x1B[0m\x1B[38;5;38m \x1B[0m\x1B[38;5;44my\x1B[0m\x1B[38;5;44mo\x1B[0m\x1B[38;5;44mu\
\x1B[0m\x1B[38;5;44m \x1B[0m\x1B[38;5;49mc\x1B[0m\x1B[38;5;49ma\x1B[0m\x1B[38;5;49mn\
\x1B[0m\x1B[38;5;48m \x1B[0m\x1B[38;5;48mr\x1B[0m\x1B[38;5;48me\x1B[0m\x1B[38;5;48md\
\x1B[0m\x1B[38;5;83mi\x1B[0m\x1B[38;5;83ms\x1B[0m\x1B[38;5;83mt\x1B[0m\x1B[38;5;83mr\
\x1B[0m\x1B[38;5;119mi\x1B[0m\x1B[38;5;118mb\x1B[0m\x1B[38;5;118mu\x1B[0m\
\x1B[38;5;118mt\x1B[0m\x1B[38;5;154me\x1B[0m\x1B[38;5;154m \x1B[0m\x1B[38;5;154mi\
\x1B[0m\x1B[38;5;148mt\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;184ma\x1B[0m\
\x1B[38;5;184mn\x1B[0m\x1B[38;5;184md\x1B[0m\x1B[38;5;214m/\x1B[0m\x1B[38;5;214mo\
\x1B[0m\x1B[38;5;214mr\x1B[0m\x1B[38;5;214m \x1B[0m\x1B[38;5;208mm\x1B[0m\
\x1B[38;5;208mo\x1B[0m\x1B[38;5;208md\x1B[0m\x1B[38;5;209mi\x1B[0m\x1B[38;5;203mf\
\x1B[0m\x1B[38;5;203my\x1B[0m\n\
\x1B[38;5;198mi\x1B[0m\x1B[38;5;199mt\x1B[0m\x1B[38;5;199m \x1B[0m\x1B[38;5;199mu\
\x1B[0m\x1B[38;5;164mn\x1B[0m\x1B[38;5;164md\x1B[0m\x1B[38;5;164me\x1B[0m\
\x1B[38;5;164mr\x1B[0m\x1B[38;5;128m \x1B[0m\x1B[38;5;129mt\x1B[0m\x1B[38;5;129mh\
\x1B[0m\x1B[38;5;129me\x1B[0m\x1B[38;5;93m \x1B[0m\x1B[38;5;93mt\x1B[0m\x1B[38;5;93me\
\x1B[0m\x1B[38;5;93mr\x1B[0m\x1B[38;5;63mm\x1B[0m\x1B[38;5;63ms\x1B[0m\x1B[38;5;63m \
\x1B[0m\x1B[38;5;63mo\x1B[0m\x1B[38;5;33mf\x1B[0m\x1B[38;5;33m \x1B[0m\x1B[38;5;33mt\
\x1B[0m\x1B[38;5;33mh\x1B[0m\x1B[38;5;39me\x1B[0m\x1B[38;5;39m \x1B[0m\x1B[38;5;39mG\
\x1B[0m\x1B[38;5;38mN\x1B[0m\x1B[38;5;44mU\x1B[0m\x1B[38;5;44m \x1B[0m\x1B[38;5;44mG\
\x1B[0m\x1B[38;5;44me\x1B[0m\x1B[38;5;49mn\x1B[0m\x1B[38;5;49me\x1B[0m\x1B[38;5;49mr\
\x1B[0m\x1B[38;5;48ma\x1B[0m\x1B[38;5;48ml\x1B[0m\x1B[38;5;48m \x1B[0m\x1B[38;5;48mP\
\x1B[0m\x1B[38;5;83mu\x1B[0m\x1B[38;5;83mb\x1B[0m\x1B[38;5;83ml\x1B[0m\x1B[38;5;83mi\
\x1B[0m\x1B[38;5;119mc\x1B[0m\x1B[38;5;118m \x1B[0m\x1B[38;5;118mL\x1B[0m\x1B[38;5;118mi\
\x1B[0m\x1B[38;5;154mc\x1B[0m\x1B[38;5;154me\x1B[0m\x1B[38;5;154mn\x1B[0m\x1B[38;5;148ms\
\x1B[0m\x1B[38;5;184me\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;184ma\x1B[0m\x1B[38;5;184ms\
\x1B[0m\x1B[38;5;214m \x1B[0m\x1B[38;5;214mp\x1B[0m\x1B[38;5;214mu\x1B[0m\x1B[38;5;214mb\
\x1B[0m\x1B[38;5;208ml\x1B[0m\x1B[38;5;208mi\x1B[0m\x1B[38;5;208ms\x1B[0m\x1B[38;5;209mh\
\x1B[0m\x1B[38;5;203me\x1B[0m\x1B[38;5;203md\x1B[0m\x1B[38;5;203m \x1B[0m\x1B[38;5;203mb\
\x1B[0m\x1B[38;5;198my\x1B[0m\n\
\x1B[38;5;199mt\x1B[0m\x1B[38;5;164mh\x1B[0m\x1B[38;5;164me\x1B[0m\x1B[38;5;164m \x1B[0m\
\x1B[38;5;164mF\x1B[0m\x1B[38;5;128mr\x1B[0m\x1B[38;5;129me\x1B[0m\x1B[38;5;129me\x1B[0m\
\x1B[38;5;129m \x1B[0m\x1B[38;5;93mS\x1B[0m\x1B[38;5;93mo\x1B[0m\x1B[38;5;93mf\x1B[0m\
\x1B[38;5;93mt\x1B[0m\x1B[38;5;63mw\x1B[0m\x1B[38;5;63ma\x1B[0m\x1B[38;5;63mr\x1B[0m\
\x1B[38;5;63me\x1B[0m\x1B[38;5;33m \x1B[0m\x1B[38;5;33mF\x1B[0m\x1B[38;5;33mo\x1B[0m\
\x1B[38;5;33mu\x1B[0m\x1B[38;5;39mn\x1B[0m\x1B[38;5;39md\x1B[0m\x1B[38;5;39ma\x1B[0m\
\x1B[38;5;38mt\x1B[0m\x1B[38;5;44mi\x1B[0m\x1B[38;5;44mo\x1B[0m\x1B[38;5;44mn\x1B[0m\
\x1B[38;5;44m.\x1B[0m\n\n\
\x1B[38;5;129mT\x1B[0m\x1B[38;5;129mh\x1B[0m\x1B[38;5;129mi\x1B[0m\x1B[38;5;93ms\
\x1B[0m\x1B[38;5;93m \x1B[0m\x1B[38;5;93mp\x1B[0m\x1B[38;5;93mr\x1B[0m\x1B[38;5;63mo\
\x1B[0m\x1B[38;5;63mg\x1B[0m\x1B[38;5;63mr\x1B[0m\x1B[38;5;63ma\x1B[0m\x1B[38;5;33mm\
\x1B[0m\x1B[38;5;33m \x1B[0m\x1B[38;5;33mi\x1B[0m\x1B[38;5;33ms\x1B[0m\x1B[38;5;39m \
\x1B[0m\x1B[38;5;39md\x1B[0m\x1B[38;5;39mi\x1B[0m\x1B[38;5;38ms\x1B[0m\x1B[38;5;44mt\
\x1B[0m\x1B[38;5;44mr\x1B[0m\x1B[38;5;44mi\x1B[0m\x1B[38;5;44mb\x1B[0m\x1B[38;5;49mu\
\x1B[0m\x1B[38;5;49mt\x1B[0m\x1B[38;5;49me\x1B[0m\x1B[38;5;48md\x1B[0m\x1B[38;5;48m \
\x1B[0m\x1B[38;5;48mi\x1B[0m\x1B[38;5;48mn\x1B[0m\x1B[38;5;83m \x1B[0m\x1B[38;5;83mt\
\x1B[0m\x1B[38;5;83mh\x1B[0m\x1B[38;5;83me\x1B[0m\x1B[38;5;119m \x1B[0m\x1B[38;5;118mh\
\x1B[0m\x1B[38;5;118mo\x1B[0m\x1B[38;5;118mp\x1B[0m\x1B[38;5;154me\x1B[0m\x1B[38;5;154m \
\x1B[0m\x1B[38;5;154mt\x1B[0m\x1B[38;5;148mh\x1B[0m\x1B[38;5;184ma\x1B[0m\x1B[38;5;184mt\
\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;184mi\x1B[0m\x1B[38;5;214mt\x1B[0m\x1B[38;5;214m \
\x1B[0m\x1B[38;5;214mw\x1B[0m\x1B[38;5;214mi\x1B[0m\x1B[38;5;208ml\x1B[0m\x1B[38;5;208ml\
\x1B[0m\x1B[38;5;208m \x1B[0m\x1B[38;5;209mb\x1B[0m\x1B[38;5;203me\x1B[0m\x1B[38;5;203m \
\x1B[0m\x1B[38;5;203mu\x1B[0m\x1B[38;5;203ms\x1B[0m\x1B[38;5;198me\x1B[0m\x1B[38;5;198mf\
\x1B[0m\x1B[38;5;198mu\x1B[0m\x1B[38;5;199ml\x1B[0m\x1B[38;5;199m,\x1B[0m\n\
\x1B[38;5;93mb\x1B[0m\x1B[38;5;93mu\x1B[0m\x1B[38;5;93mt\x1B[0m\x1B[38;5;93m \
\x1B[0m\x1B[38;5;63mW\x1B[0m\x1B[38;5;63mI\x1B[0m\x1B[38;5;63mT\x1B[0m\x1B[38;5;63mH\
\x1B[0m\x1B[38;5;33mO\x1B[0m\x1B[38;5;33mU\x1B[0m\x1B[38;5;33mT\x1B[0m\x1B[38;5;33m \
\x1B[0m\x1B[38;5;39mA\x1B[0m\x1B[38;5;39mN\x1B[0m\x1B[38;5;39mY\x1B[0m\x1B[38;5;38m \
\x1B[0m\x1B[38;5;44mW\x1B[0m\x1B[38;5;44mA\x1B[0m\x1B[38;5;44mR\x1B[0m\x1B[38;5;44mR\
\x1B[0m\x1B[38;5;49mA\x1B[0m\x1B[38;5;49mN\x1B[0m\x1B[38;5;49mT\x1B[0m\x1B[38;5;48mY\
\x1B[0m\x1B[38;5;48m;\x1B[0m\x1B[38;5;48m \x1B[0m\x1B[38;5;48mw\x1B[0m\x1B[38;5;83mi\
\x1B[0m\x1B[38;5;83mt\x1B[0m\x1B[38;5;83mh\x1B[0m\x1B[38;5;83mo\x1B[0m\x1B[38;5;119mu\
\x1B[0m\x1B[38;5;118mt\x1B[0m\x1B[38;5;118m \x1B[0m\x1B[38;5;118me\x1B[0m\x1B[38;5;154mv\
\x1B[0m\x1B[38;5;154me\x1B[0m\x1B[38;5;154mn\x1B[0m\x1B[38;5;148m \x1B[0m\x1B[38;5;184mt\
\x1B[0m\x1B[38;5;184mh\x1B[0m\x1B[38;5;184me\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;214mi\
\x1B[0m\x1B[38;5;214mm\x1B[0m\x1B[38;5;214mp\x1B[0m\x1B[38;5;214ml\x1B[0m\x1B[38;5;208mi\
\x1B[0m\x1B[38;5;208me\x1B[0m\x1B[38;5;208md\x1B[0m\x1B[38;5;209m \x1B[0m\x1B[38;5;203mw\
\x1B[0m\x1B[38;5;203ma\x1B[0m\x1B[38;5;203mr\x1B[0m\x1B[38;5;203mr\x1B[0m\x1B[38;5;198ma\
\x1B[0m\x1B[38;5;198mn\x1B[0m\x1B[38;5;198mt\x1B[0m\x1B[38;5;199my\x1B[0m\x1B[38;5;199m \
\x1B[0m\x1B[38;5;199mo\x1B[0m\x1B[38;5;199mf\x1B[0m\n\
\x1B[38;5;93mM\x1B[0m\x1B[38;5;63mE\x1B[0m\x1B[38;5;63mR\x1B[0m\x1B[38;5;63mC\
\x1B[0m\x1B[38;5;63mH\x1B[0m\x1B[38;5;33mA\x1B[0m\x1B[38;5;33mN\x1B[0m\x1B[38;5;33mT\
\x1B[0m\x1B[38;5;33mI\x1B[0m\x1B[38;5;39mB\x1B[0m\x1B[38;5;39mI\x1B[0m\x1B[38;5;39mL\
\x1B[0m\x1B[38;5;38mI\x1B[0m\x1B[38;5;44mT\x1B[0m\x1B[38;5;44mY\x1B[0m\x1B[38;5;44m \
\x1B[0m\x1B[38;5;44mo\x1B[0m\x1B[38;5;49mr\x1B[0m\x1B[38;5;49m \x1B[0m\x1B[38;5;49mF\
\x1B[0m\x1B[38;5;48mI\x1B[0m\x1B[38;5;48mT\x1B[0m\x1B[38;5;48mN\x1B[0m\x1B[38;5;48mE\
\x1B[0m\x1B[38;5;83mS\x1B[0m\x1B[38;5;83mS\x1B[0m\x1B[38;5;83m \x1B[0m\x1B[38;5;83mF\
\x1B[0m\x1B[38;5;119mO\x1B[0m\x1B[38;5;118mR\x1B[0m\x1B[38;5;118m \x1B[0m\x1B[38;5;118mA\
\x1B[0m\x1B[38;5;154m \x1B[0m\x1B[38;5;154mP\x1B[0m\x1B[38;5;154mA\x1B[0m\x1B[38;5;148mR\
\x1B[0m\x1B[38;5;184mT\x1B[0m\x1B[38;5;184mI\x1B[0m\x1B[38;5;184mC\x1B[0m\x1B[38;5;184mU\
\x1B[0m\x1B[38;5;214mL\x1B[0m\x1B[38;5;214mA\x1B[0m\x1B[38;5;214mR\x1B[0m\x1B[38;5;214m \
\x1B[0m\x1B[38;5;208mP\x1B[0m\x1B[38;5;208mU\x1B[0m\x1B[38;5;208mR\x1B[0m\x1B[38;5;209mP\
\x1B[0m\x1B[38;5;203mO\x1B[0m\x1B[38;5;203mS\x1B[0m\x1B[38;5;203mE\x1B[0m\x1B[38;5;203m.\
\x1B[0m\x1B[38;5;198m \x1B[0m\x1B[38;5;198mC\x1B[0m\x1B[38;5;198mh\x1B[0m\x1B[38;5;199me\
\x1B[0m\x1B[38;5;199mc\x1B[0m\x1B[38;5;199mk\x1B[0m\x1B[38;5;199m \x1B[0m\x1B[38;5;164mt\
\x1B[0m\x1B[38;5;164mh\x1B[0m\x1B[38;5;164me\x1B[0m\n\
\x1B[38;5;63mG\x1B[0m\x1B[38;5;63mN\x1B[0m\x1B[38;5;33mU\x1B[0m\x1B[38;5;33m \
\x1B[0m\x1B[38;5;33mG\x1B[0m\x1B[38;5;33me\x1B[0m\x1B[38;5;39mn\x1B[0m\x1B[38;5;39me\
\x1B[0m\x1B[38;5;39mr\x1B[0m\x1B[38;5;38ma\x1B[0m\x1B[38;5;44ml\x1B[0m\x1B[38;5;44m \
\x1B[0m\x1B[38;5;44mP\x1B[0m\x1B[38;5;44mu\x1B[0m\x1B[38;5;49mb\x1B[0m\x1B[38;5;49ml\
\x1B[0m\x1B[38;5;49mi\x1B[0m\x1B[38;5;48mc\x1B[0m\x1B[38;5;48m \x1B[0m\x1B[38;5;48mL\
\x1B[0m\x1B[38;5;48mi\x1B[0m\x1B[38;5;83mc\x1B[0m\x1B[38;5;83me\x1B[0m\x1B[38;5;83mn\
\x1B[0m\x1B[38;5;83ms\x1B[0m\x1B[38;5;119me\x1B[0m\x1B[38;5;118m \x1B[0m\x1B[38;5;118mf\
\x1B[0m\x1B[38;5;118mo\x1B[0m\x1B[38;5;154mr\x1B[0m\x1B[38;5;154m \x1B[0m\x1B[38;5;154mm\
\x1B[0m\x1B[38;5;148mo\x1B[0m\x1B[38;5;184mr\x1B[0m\x1B[38;5;184me\x1B[0m\x1B[38;5;184m \
\x1B[0m\x1B[38;5;184md\x1B[0m\x1B[38;5;214me\x1B[0m\x1B[38;5;214mt\x1B[0m\x1B[38;5;214ma\
\x1B[0m\x1B[38;5;214mi\x1B[0m\x1B[38;5;208ml\x1B[0m\x1B[38;5;208ms\x1B[0m\x1B[38;5;208m.\
\x1B[0m\n\n\
\x1B[38;5;39mY\x1B[0m\x1B[38;5;39mo\x1B[0m\x1B[38;5;39mu\x1B[0m\x1B[38;5;38m \
\x1B[0m\x1B[38;5;44ms\x1B[0m\x1B[38;5;44mh\x1B[0m\x1B[38;5;44mo\x1B[0m\
\x1B[38;5;44mu\x1B[0m\x1B[38;5;49ml\x1B[0m\x1B[38;5;49md\x1B[0m\x1B[38;5;49m \
\x1B[0m\x1B[38;5;48mh\x1B[0m\x1B[38;5;48ma\x1B[0m\x1B[38;5;48mv\x1B[0m\
\x1B[38;5;48me\x1B[0m\x1B[38;5;83m \x1B[0m\x1B[38;5;83mr\x1B[0m\x1B[38;5;83me\
\x1B[0m\x1B[38;5;83mc\x1B[0m\x1B[38;5;119me\x1B[0m\x1B[38;5;118mi\x1B[0m\
\x1B[38;5;118mv\x1B[0m\x1B[38;5;118me\x1B[0m\x1B[38;5;154md\x1B[0m\x1B[38;5;154m \
\x1B[0m\x1B[38;5;154ma\x1B[0m\x1B[38;5;148m \x1B[0m\x1B[38;5;184mc\x1B[0m\
\x1B[38;5;184mo\x1B[0m\x1B[38;5;184mp\x1B[0m\x1B[38;5;184my\x1B[0m\x1B[38;5;214m \
\x1B[0m\x1B[38;5;214mo\x1B[0m\x1B[38;5;214mf\x1B[0m\x1B[38;5;214m \x1B[0m\
\x1B[38;5;208mt\x1B[0m\x1B[38;5;208mh\x1B[0m\x1B[38;5;208me\x1B[0m\x1B[38;5;209m \
\x1B[0m\x1B[38;5;203mG\x1B[0m\x1B[38;5;203mN\x1B[0m\x1B[38;5;203mU\x1B[0m\
\x1B[38;5;203m \x1B[0m\x1B[38;5;198mG\x1B[0m\x1B[38;5;198me\x1B[0m\x1B[38;5;198mn\
\x1B[0m\x1B[38;5;199me\x1B[0m\x1B[38;5;199mr\x1B[0m\x1B[38;5;199ma\x1B[0m\
\x1B[38;5;199ml\x1B[0m\x1B[38;5;164m \x1B[0m\x1B[38;5;164mP\x1B[0m\x1B[38;5;164mu\
\x1B[0m\x1B[38;5;164mb\x1B[0m\x1B[38;5;128ml\x1B[0m\x1B[38;5;129mi\x1B[0m\
\x1B[38;5;129mc\x1B[0m\x1B[38;5;129m \x1B[0m\x1B[38;5;93mL\x1B[0m\x1B[38;5;93mi\
\x1B[0m\x1B[38;5;93mc\x1B[0m\x1B[38;5;93me\x1B[0m\x1B[38;5;63mn\x1B[0m\
\x1B[38;5;63ms\x1B[0m\x1B[38;5;63me\x1B[0m\n\
\x1B[38;5;38ma\x1B[0m\x1B[38;5;44ml\x1B[0m\x1B[38;5;44mo\x1B[0m\x1B[38;5;44mn\
\x1B[0m\x1B[38;5;44mg\x1B[0m\x1B[38;5;49m \x1B[0m\x1B[38;5;49mw\x1B[0m\x1B[38;5;49mi\
\x1B[0m\x1B[38;5;48mt\x1B[0m\x1B[38;5;48mh\x1B[0m\x1B[38;5;48m \x1B[0m\x1B[38;5;48mt\
\x1B[0m\x1B[38;5;83mh\x1B[0m\x1B[38;5;83mi\x1B[0m\x1B[38;5;83ms\x1B[0m\x1B[38;5;83m \
\x1B[0m\x1B[38;5;119mp\x1B[0m\x1B[38;5;118mr\x1B[0m\x1B[38;5;118mo\x1B[0m\
\x1B[38;5;118mg\x1B[0m\x1B[38;5;154mr\x1B[0m\x1B[38;5;154ma\x1B[0m\x1B[38;5;154mm\
\x1B[0m\x1B[38;5;148m.\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;184mI\x1B[0m\
\x1B[38;5;184mf\x1B[0m\x1B[38;5;184m \x1B[0m\x1B[38;5;214mn\x1B[0m\x1B[38;5;214mo\
\x1B[0m\x1B[38;5;214mt\x1B[0m\x1B[38;5;214m,\x1B[0m\x1B[38;5;208m \x1B[0m\
\x1B[38;5;208ms\x1B[0m\x1B[38;5;208me\x1B[0m\x1B[38;5;209me\x1B[0m\x1B[38;5;203m \
\x1B[0m\x1B[38;5;203m<\x1B[0m\x1B[38;5;203mh\x1B[0m\x1B[38;5;203mt\x1B[0m\
\x1B[38;5;198mt\x1B[0m\x1B[38;5;198mp\x1B[0m\x1B[38;5;198m:\x1B[0m\x1B[38;5;199m/\
\x1B[0m\x1B[38;5;199m/\x1B[0m\x1B[38;5;199mw\x1B[0m\x1B[38;5;199mw\x1B[0m\
\x1B[38;5;164mw\x1B[0m\x1B[38;5;164m.\x1B[0m\x1B[38;5;164mg\x1B[0m\x1B[38;5;164mn\
\x1B[0m\x1B[38;5;128mu\x1B[0m\x1B[38;5;129m.\x1B[0m\x1B[38;5;129mo\x1B[0m\
\x1B[38;5;129mr\x1B[0m\x1B[38;5;93mg\x1B[0m\x1B[38;5;93m/\x1B[0m\x1B[38;5;93ml\
\x1B[0m\x1B[38;5;93mi\x1B[0m\x1B[38;5;63mc\x1B[0m\x1B[38;5;63me\x1B[0m\
\x1B[38;5;63mn\x1B[0m\x1B[38;5;63ms\x1B[0m\x1B[38;5;33me\x1B[0m\x1B[38;5;33ms\
\x1B[0m\x1B[38;5;33m/\x1B[0m\x1B[38;5;33m>\x1B[0m\n"
	);
}

#endif


