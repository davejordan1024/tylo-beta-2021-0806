#include "glbl.h"
#include "app.h"
#include "rckcell.h"
#include "brdcell.h"
#include "tilecell.h"
#include "tbl.h"
#include "brwsr.h"
#include "alpharec.h"
#include "notes.h"

Brdcell::Brdcell(int cellpix, int x, int y, int8_t r, int8_t c, optmap_t o) : Tilecell(x, y, cellpix, r, c, o) {}

int Brdcell::handle(int e) {

	if(e == FL_SHORTCUT) return EVNOTHANDLED;		// check shrtct event here as opposed to in tilecell::handle()
																// otherwise every sibling is checked for its own handle
	if(Tilecell::handle(e) == EVHANDLED)			// tilecell::hamdle() returns EVNOTHANDLED only for unprocessed keyboard
		return EVHANDLED;
	cout << "brdcell keydown event\n";

	const static int reject = FL_CTRL | FL_ALT | FL_META;
	int k = Fl::event_key();
	int s = Fl::event_state();
	Tbl *tbl = app->curtbl;

	if(! (s & reject)) {
		if(s & FL_SHIFT) {					// shift-tab is the only valid shift
			if(k == FL_Tab)
				tbl->notes->take_focus();
		}
		else {									// all else has no shift, ctl, alt, or meta. dont care about capslock or numlock etc
			switch(k) {
				case FL_Right	:	if(this->col < tbl->brdn - 1) tbl->brd[this->row][this->col + 1]->take_focus();		break;
				case FL_Left	:	if(this->col > 0)					tbl->brd[this->row][this->col - 1]->take_focus();		break;
				case FL_Down	:	if(this->row < tbl->brdn - 1)	tbl->brd[this->row + 1][this->col]->take_focus();		break;
																				tbl->rck[0]->take_focus();										break;
				case FL_Up		:	if(this->row > 0)    			tbl->brd[this->row - 1][this->col    ]->take_focus();	break;
				case FL_Home	:											tbl->brd[tbl->brdn / 2][tbl->brdn / 2]->take_focus();	break;
				case FL_Tab		:											tbl->rck[0]->take_focus();										break;
			}
		}
		return EVHANDLED;
	}
	return EVNOTHANDLED;		// seems like shortcuts dont work unless this is here, despite FL_SHORTCUT case at top
}
//#define CASES_OTHER	case FL_RELEASE:	case FL_MOVE: case FL_ENTER:		case FL_LEAVE:			case FL_SHOW:		case FL_HIDE: \
								case FL_KEYUP:		case FL_DRAG: case FL_ACTIVATE:	case FL_DEACTIVATE:	case FL_NO_EVENT
//#define prtevent(s)	cout << s << ' ' << fl_eventnames[e] << " col " << this->col << " row " << this->row << endl
