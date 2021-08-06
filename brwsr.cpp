#include "glbl.h"
#include "app.h"
#include "brwsr.h"
#include "brdcell.h"
#include "game.h"
#include "msg.h"
#include "notes.h"
#include "bagrow.h"
#include "numcell.h"

Brwsr::Brwsr(int x, int y, int h, vector<Game*>&entries, optmap_t o) : Fl_Hold_Browser(x, y, o["lw_opp"].ival + o["lw_time"].ival, h) {

	callback(brwsr_cb);
	box(FL_FRAME_BOX);
	color(o["list_bg"].ival);
	textcolor(o["list_fg"].ival);
	textsize(18);
	static int cw[] = { o["lw_opp"].ival, o["lw_time"].ival, 0 };
	column_widths(cw);
	for(Game* entry : entries)
		add_entry(entry);
													// option lastsort is not used for init. the order after init will be the order on disk.
	lastsort = o["lastsort"].sval;		// but sort_brwsr will use the lastsort property when user or code requests a (re)sort.
	if(lastsort.empty())
		lastsort = "by status by name";
	end();
}
void Brwsr::add_entry(Game *&gm) {
	cc *cd = gm->stat == GSTAT_ACTV ? GSTAT_ACTV_CD : GSTAT_OVER_CD;
	gm->fmtd = (string) cd + gm->opp + column_char() + cd + gm->starttm;
	this->add(gm->fmtd.c_str(), gm);
}
void Brwsr::load_first(string which) {

	#define GMPTR ((Game *) data(lineno(itm)))

	cout << "look for a game to show first" << endl;
	if(! item_first()) {
		cout << "game list is empty" << endl;						// remaining code can assume list not empty
		return;
	}
	void *itm = NULL;

	if(! which.empty()) {
		cout << "option lastgame is " << which << endl;			// try to match option last game with a menu item text. comparison is to oppname|starttm.

		for(itm = item_first(); itm; itm = item_next(itm))
			if(! which.compare((string) GMPTR->opp + column_char() + GMPTR->starttm))
				break;
		if(! itm)															// below code is about edge/error conditions
			cout << "game not found" << endl;
	}
	else
		cout << "last game option is empty" << endl;

	if(! itm) {
		cout << "look for first active game" << endl;
		for(itm = item_first(); itm; itm = item_next(itm))
			if(GMPTR->stat == GSTAT_ACTV)
				break;
		if(! itm) {
			cout << "there are no active games" << endl;
			itm = item_first();
		}
	}
	select(lineno(itm));
	GMPTR->load(false);
	this->take_focus();
	#undef GMPTR
}
int Brwsr::handle(int e) {

	if(e == FL_SHORTCUT) return EVNOTHANDLED;

	if(e == FL_KEYBOARD && (! (Fl::event_state() & (FL_CTRL | FL_ALT | FL_META)))) {
		if(Fl::event_key() == FL_Tab) {
			if(! Fl::event_state(FL_SHIFT))
				app->curtbl->brd[app->curtbl->brdn / 2][app->curtbl->brdn / 2]->take_focus();
			else
				app->curtbl->notes->take_focus();
			return EVHANDLED;
		}
	}
	return Fl_Hold_Browser::handle(e);
}

void Brwsr::sort(cc *ordarg, Game *sel) {

	string order;
	if(ordarg)
		order.assign(ordarg);	// call is from menu cb
	else
		order.assign(lastsort);	// call is from elsewhere in code

	bool (*func)(Game*, Game*);
	if			(! order.compare("by name"				)) func = brwsrsort::by_name;
	else if	(! order.compare("by start"			)) func = brwsrsort::by_start;
	else if	(! order.compare("by end"				)) func = brwsrsort::by_end;
	else if	(! order.compare("by status by name")) func = brwsrsort::by_status_by_name;
	else {
		fl_alert("Ignoring invalid sort order \"%s\"; will sort by status by name.", order.c_str());
		func = brwsrsort::by_status_by_name;
		order = "by status by name";
	}
	lastsort = order;			// remember order for nonmenu call if/when necessary

	cout << "sort, order=" << order << endl;

//	void *resel = data(value());								// remember game address of selected item

	std::sort(app->games.begin(), app->games.end(), func);

	for(int j = 0; j < app->games.size(); j++) {			// reformat and write sorted games text back to menu

		Game *gm = app->games[j];
		cc *statfmt = (gm->stat == GSTAT_ACTV ? GSTAT_ACTV_CD : GSTAT_OVER_CD);

		text(j + 1, ((string) statfmt + gm->opp + column_char() + statfmt + gm->starttm).c_str());
		data(j + 1, gm);
		if(gm == sel) {											// reselect previously selected item
			select(j + 1, 1);
		}
	}
	app->setmod(true);
}
bool brwsrsort::by_name(Game *g1, Game *g2) {
	int cmp = strcmp(g1->opp, g2->opp);
	return cmp < 0 ? true : (cmp ? false : strcmp(g1->starttm, g2->starttm) < 0);
}
bool brwsrsort::by_start(Game *g1, Game *g2) {
	return strcmp(g1->starttm, g2->starttm) < 0;
}
bool brwsrsort::by_end(Game *g1, Game *g2) {
	return strcmp(g1->endtm, g2->endtm) < 0;
}
bool brwsrsort::by_status_by_name(Game *g1, Game *g2) {

	int cmp = g1->stat - g2->stat;

	if(cmp == 0) {
		cmp = strcmp(g1->opp, g2->opp);
		if(cmp == 0)
			cmp = strcmp(g1->starttm, g2->starttm);		// also sort by time -- there can be multiple (in)active games for the same opp
	}
	return cmp < 0;
}
void brwsr_cb(Fl_Widget *brwsr, void*) {			 // cant put cb in .h file b/c app is incomplete

	cout << "brwsr callback" << endl;
	Brwsr *b = (Brwsr*) brwsr;
	Game *gm = (Game*) b->data(b->value());

	if(b->value() != 0) {			// 0 happens when click occurs in brwsr but not on item
		gm->load(false);
		app->setmod(true);
		app->msg->setmsg1();
	}
	return;
}