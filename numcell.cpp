#include "glbl.h"
#include "app.h"
#include "numcell.h"

Numcell::Numcell(int x, int y, int w, int h, cc *lbl, int8_t v, optmap_t o) : Fl_Box(x, y, w, h, 0) {

	box(FL_FLAT_BOX);
	color(o["stat_bg"].ival);
	labelcolor(o["stat_fg"].ival);
	labelfont(FL_COURIER);
	labelsize(o["stat_fs"].ival);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	label(fulllbl);
	text(lbl, v);
}
void Numcell::text(cc *s, int8_t j = 0) {				// string "bag" is assigned at tbl::addobjects, string game->opp at load_game()
	int valoff;
	if((valoff = strlen(s)) > NCTXTMAX)
		valoff = NCTXTMAX;
	valptr = (char *) fulllbl + valoff + 1;			// +1 for the space
	val = j;

	// no need to delete/realloc fulllbl - its declared as an array of max size

	snprintf(fulllbl, NCTXTMAX + NCDGTMAX - 1, "%.*s %d", NCTXTMAX, s, j);
}
int8_t	Numcell::incr (int8_t j)	{ snprintf(valptr, NCDGTMAX, "%d", val += j); this->redraw(); return val; }
int8_t	Numcell::value(int8_t j)	{ snprintf(valptr, NCDGTMAX, "%d", val  = j); this->redraw(); return val; }
int8_t	Numcell::value()				{ return atoi(valptr); }
