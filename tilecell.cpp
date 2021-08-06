#include "app.h"
#include "glbl.h"
#include "tilecell.h"

Tilecell::Tilecell(int x, int y, int w_h, int8_t r, int8_t c, optmap_t o) : Fl_Box(x, y, w_h, w_h, 0) {

	col = c; row = r;
	kystrk = FLLRCHR;
	box(FL_FLAT_BOX);
	labelsize(o["cell_fs"].ival);
	labelfont(FL_COURIER);
	color(lolite = o["cell_bg"].ival);
	labelcolor(o["cell_fg"].ival);
	align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
	savecolor = 0;
};
int Tilecell::handle(int e) {

	const static int hilite = app->opts["cell_hi"].ival;
	const static bool autor = app->opts["autoright"].ival;

	switch(e) {
		case FL_KEYBOARD : {			// shortcut event is checked by child widget before this handle is called

			const static int reject = FL_CTRL | FL_ALT | FL_META;	// shift is ok b/c i dont want to trip up the user by rejecting upper case ltrs

			if(! (Fl::event_state() & reject)) {

				cout << "Tilecell keydown\n";

				Tbl *tbl = app->curtbl;
				const int k = toupper(Fl::event_key());
				Alpharec *dec = NULL, *inc = NULL;

				// srch_keytab takes the int keystroke and returns a pointer to a struct containing (among other things)
				// the corresponding (possibly multibyte) UI symbol

				if(dec = ty::srch_keytab(tbl->alphas, tbl->alphan, k)) {					// if match, decrement count for letter entered
					inc = ty::srch_keytab(tbl->alphas, tbl->alphan, this->kystrk);		// and increment count for any overwrite
					ty::update_cnts(inc, dec);
					this->kystrk = k;
					this->copy_label(dec->uirep);

					if(this->row == -1 && autor && this->col + 1 < tbl->rckn)
						tbl->rck[this->col + 1]->take_focus();

					app->ismod = true;
					return EVHANDLED;
				}											// if keystroke is delete, nothing is decremented, deleted letter is incremented
				else if(k == FL_Delete) {
					if(inc = ty::srch_keytab(tbl->alphas, tbl->alphan, this->kystrk)) {
						ty::update_cnts(inc, NULL);
						this->kystrk = FLLRCHR;
						this->copy_label(FLLRLBL);
						app->ismod = true;
						return EVHANDLED;
					}
				}
			}
			return EVNOTHANDLED;					// back to brdcell or rckcell ->handle() for further processing
		}
		case FL_PUSH:		this->take_focus();									break;
		case FL_FOCUS:		this->color(hilite);			this->redraw();	break;
		case FL_UNFOCUS:	this->color(this->lolite);	this->redraw();	break;
	}
	return EVHANDLED;
}
