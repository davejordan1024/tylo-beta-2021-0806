#ifndef GLBL_H
#define GLBL_H

#include <unistd.h>			// unlink
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using std::cout;
using std::endl;
using std::string;
using std::vector;

struct opt_s { int ival; string sval; char type; bool shift; bool write; };
typedef std::map<string, opt_s> optmap_t;
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>		// move this after you know everywhere its going
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/fl_ask.H>
#include <FL/names.h>

#define APPTTL		"Tylosaurus"
#define APPVER		"beta 1"
#define OPTFL		"OPTS.txt"
#define TMPLTFL	"TMPLTS.txt"
#define GAMEFL 	"GAMES.txt"
#define TIMESZ		sizeof("21-01-19 10:42:42")		// fmt as it appears in game file
#define TIMEFMT	"%y-%m-%d %H:%M:%S"					// strtime fmt corresponding to above

#define BRDMAX		32					// maxes are somewhat arbitrary.
#define RCKMAX		32
#define OPPMAX		32
#define ALPHAMAX	37					// based on the scientific estimate of how big of a bag display will fit on my screen
#define NTSMAX		1024				// notes, max chars. not enforced except when reading & writing game file
#define GMMAX		2048
#define LBLH		36					// pixel height, label across top of window

#define EVNOTHANDLED 0
#define EVHANDLED		1

#define GSTAT_ACTV		'1'				// game statuses in game_s and corresponding format codes for menu item
#define GSTAT_ACTV_CD	"@b"
#define GSTAT_OVER		'0'
#define GSTAT_OVER_CD	"@C46@."
#define FLLRCHR			' '				// char for blank brd/rck cell (not blank tile)
#define FLLRLBL			" "
#define NTSNL				'\1'				// subststitute for \n in notes on disk


// these are multistatement warnings / fails that need to be brace enclosed in usage. the hack with the leading semicolon
// ensures that in case i forget the braces the messsage will appear instead of the app silently returning false or exiting with FAIL.

#define EGEN(msg)					; fl_alert("%s line %d: %s.", TYLOFL, linenum, msg); return false
#define ERNG(msg, min, max)	; fl_alert("%s line %d: %s is outside range of %d - %d.", TYLOFL, linenum, msg, min, max); return false
#define ELTR(msg)					; fl_alert("%s line %d letter %d: %s.", TYLOFL, linenum, ltrnum + 1, msg); return false
#define EFTL(what)				; cout << what << ": Out of memory, or other unrecoverable error.\n"; exit(EXIT_FAILURE)

#define EINIT(msg)	fl_alert("%s can't load because\n%s", APPTTL, msg), exit(EXIT_FAILURE)

#define prtevent(s)	cout << s << ' ' << fl_eventnames[e] << " col " << cell->col << " row " << cell->row << endl

#define STS(n)		(std::to_string(n))
#define I2CS(n)	(STS(n).c_str())
#define nt			(std::nothrow)
#define cc			const char

/*

namespace cb {
	void app_cb(Fl_Widget*, void*);
	void notesmod_cb(int, int nInserted, int nDeleted, int, const char*, void* v);
	void brwsr_cb(Fl_Widget*, void*);
}

*/
class App;
extern App *app;

#endif // GLBL_H
