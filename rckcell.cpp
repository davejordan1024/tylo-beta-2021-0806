#include "glbl.h"
#include "app.h"
#include "rckcell.h"
#include "brdcell.h"
#include "tbl.h"
#include "alpharec.h"
#include "notes.h"

Rckcell::Rckcell(int cellpix, int x, int y, int c, optmap_t o)	: Tilecell(x, y, cellpix, -1, c, o) {}

int Rckcell::handle(int e) {

	const static int reject = FL_CTRL | FL_ALT | FL_META;
	int k = Fl::event_key();
	int s = Fl::event_state();
	Tbl *tbl = app->curtbl;

	if(e == FL_SHORTCUT) return EVNOTHANDLED;

	if(Tilecell::handle(e) == EVHANDLED) {			// returns EVNOTHANDLED only for unprocessed keydown
//		if(autor && this->col + 1 < tbl->rckn)
//			tbl->rck[this->col + 1]->take_focus();
		return EVHANDLED;
	}
	cout << "rckcell keydown event\n";

	if(! (s & reject)) {
		if(s & FL_SHIFT) {					// shift-tab is the only valid shift
			if(k == FL_Tab)
				tbl->brd[tbl->brdn / 2][tbl->brdn / 2]->take_focus();
		}
		else {									// all else has no shift, ctl, alt, or meta. dont care about capslock or numlock etc
			switch(k) {
				case FL_BackSpace	: {
					int j = this->col;
					if(j > 0) {
						j--;
						ty::update_cnts(ty::srch_keytab(tbl->alphas, tbl->alphan, tbl->rck[j]->kystrk), NULL);
						while(j < tbl->rckn - 1) {
							tbl->rck[j]->kystrk =	tbl->rck[j + 1]->kystrk;
							tbl->rck[j]->copy_label(tbl->rck[j + 1]->label());
							j++;
						}
						tbl->rck[j]->copy_label(FLLRLBL);
						tbl->rck[j]->kystrk = FLLRCHR;
						tbl->rck[this->col - 1]->take_focus();
						app->setmod(true);
					}
				}
				case FL_Down		:
				case FL_Tab			:											tbl->notes					->take_focus(); break;
				case FL_Right		:	if(this->col < tbl->rckn - 1)	tbl->rck[this->col + 1]	->take_focus(); break;
				case FL_Left		:	if(this->col > 0)     			tbl->rck[this->col - 1]	->take_focus(); break;
				case FL_Home		:											tbl->rck[0]					->take_focus(); break;
				case FL_End			:											tbl->rck[tbl->rckn - 1]	->take_focus(); break;
				case FL_Up			:	tbl->brd[tbl->brdn / 2][tbl->brdn / 2]						->take_focus(); break;
			}
		}
		return EVHANDLED;
	}
//	if((s & FL_CTRL) && ! (s & (FL_SHIFT | FL_ALT | FL_META)) && k == FL_End) {
//		ty::squish();
//		return EVHANDLED;
//	}
	return EVNOTHANDLED;
}
