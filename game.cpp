#include "glbl.h"
#include "app.h"
#include "brwsr.h"
#include "game.h"
#include "tbl.h"
#include "brdcell.h"
#include "rckcell.h"
#include "bagcell.h"
#include "bagrow.h"
#include "numcell.h"
#include "alpharec.h"
#include "notes.h"

/* editor/buffer strategy:
	table ctor				: tbl->notes = new Fl_Text_Editor
	game from disk ctor	: gm->buffy = new Fl_Text_Buffer, copy _notes arg to gm->buffy->text()
	newgame ctor			: gm->buffy = new Fl_Text_Buffer
	load game				: tbl->notes->buffer(gm->buffy)
*/

// the first ctor is called by parse_game_text, which pushes onto the game vector but does *not* the load UI.
// the second is called by the new game menu item, which pushes onto the game vector and loads the UI.

Game::Game(char _stat, Tbl *_tbl, char *_tblname, char *_opp, char *_starttm, char *_endtm, char *_brd, char *_rck, char *_notes) {

	int len;

	stat = _stat;
	tbl = _tbl;
	tblname = _tblname;						// source already points to heap

	len = strlen(_opp);
	opp = new char[len + 1]();				// new char[n] ctor fills with nul iff followed by () or n is constant
	strncpy(opp, _opp, len);
	opp[len] = 0;

	len = strlen(_starttm);
	if(len >= TIMESZ)
		len = TIMESZ - 1;
	strncpy(starttm, _starttm, len);		// timestamps already declared as char[TIMESZ]
	starttm[len] = 0;

	len = strlen(_endtm);
	if(len >= TIMESZ)
		len = TIMESZ - 1;
	strncpy(endtm, _endtm, len);
	endtm[len] = 0;

	len = tbl->brdn * tbl->brdn;
	brd = new char[len]();				// brd and rck are NONterminated char arrays
	memset(brd, FLLRCHR, len);			// memset cures underflow
	memcpy(brd, _brd, len);				// memcpy cures overflow

	rck = new char[tbl->rckn]();
	memset(rck, FLLRCHR, tbl->rckn);
	memcpy(rck, _rck, tbl->rckn);

	buffy = new Fl_Text_Buffer;
	buffy->text(_notes);
}
Game::Game(Tbl *_tbl, cc* _opp) {

	stat = GSTAT_ACTV;
	tbl = _tbl;									// table ptr
	opp = (char *) _opp;						// _opp should already be pointing to heap
	time_t tm = time(NULL);											// start time
	strftime(starttm, TIMESZ, TIMEFMT, localtime(&tm));	// strftime allegedly nul terminates
	endtm[0] = 0;														// end time
	brd = new char[		tbl->brdn * tbl->brdn];				// brd and rck are NONterminated char arrays
	memset(brd, FLLRCHR, tbl->brdn * tbl->brdn);
	rck = new char[		tbl->rckn];					// rack
	memset(rck, FLLRCHR, tbl->rckn);
	buffy = new Fl_Text_Buffer;						// buffer
}
void Game::load(bool reload) {					// load game from struct (gm->) to UI labels (tbl->, bagrows->, etc); app->cur* and show/hide housekeeping

	if(! reload) {							// reload arg will be false unless load() is called by Game::clear()
		if(this == app->curgame)		// avoid reloading what is already on the screen (user clicks on browser item more than once)
			return;
		if(app->curgame) {				// will be nullptr if first load or load after delete
			app->curgame->unload();
		}
	}
	cout << "load " << tbl << ' ' << opp << ' ' << starttm << ' ' << this << endl;

	Alpharec *match;
	int	j,
			cnsn = tbl->cnsn,			// make copies of given totals for letter types, in preparation for decrementing.
			vwln = tbl->vwln,			// original numbers need to stay unchanged for next calls to load()
			tilen =tbl->tilen;

	// given and remaining count values for individual letters are stored in the bagrows, which are in display (as opposed to kystrk) order.
	// bagrow children display the values as labels. no counts are stored in the game structs.

	for(j = 0; j < tbl->alphan; j++)														// copy given to remaining in prep. for decrementing
		tbl->bagrows[j]->remval = tbl->bagrows[j]->gvnval;

	for(j = 0; j < tbl->brdn * tbl->brdn; j++) {										// loop on board letters in game struct

		Brdcell *cell = tbl->brd[j / tbl->brdn][j % tbl->brdn];

		if(match = ty::srch_keytab(tbl->alphas, tbl->alphan, brd[j])) {
			tilen--;																				// decrement total tile count
			tbl->bagrows[match->row]->remval--;											// decrement individual letter count
			switch(match->snr) {																// decrement sonority counts
				case 'c':	cnsn--;	break;
				case 'v':	vwln--;	break;
				case 'e':	cnsn--;
								vwln--;
			}
			cell->copy_label(match->uirep);	// set label in board cell
			cell->kystrk = match->kystrk;	// set kystrk member of cell
		}
		else {
			cell->label(FLLRLBL);
			cell->kystrk = FLLRCHR;
		}
		cell->color(cell->lolite);															// cell would retain incorrect color if search not cleared
	}
	for(j = 0; j < tbl->rckn; j++) {														// loop on rack letters in game struct
		if(match = ty::srch_keytab(tbl->alphas, tbl->alphan, rck[j])) {
			tilen--;
			tbl->bagrows[match->row]->remval--;
			switch(match->snr) {
				case 'c':	cnsn--;	break;												// decrement as above
				case 'v':	vwln--;	break;
				case 'e':	cnsn--;
								vwln--;
			}
			tbl->rck[j]->copy_label(match->uirep);										// set label and kystrk as above
			tbl->rck[j]->kystrk = match->kystrk;
		}
		else {
			tbl->rck[j]->label(FLLRLBL);
			tbl->rck[j]->kystrk = FLLRCHR;
		}
		tbl->rck[j]->color(tbl->rck[j]->lolite);
	}
	// labels for counts of consonants, vowels, tiles, opp, bag ================================

	static const int
		nclr	= app->opts["bag_nega"].ival,
		zclr	= app->opts["bag_zero"].ival,
		pclr	= app->opts["bag_posi"].ival;
	int clr;

	for(j = 0; j < tbl->alphan; j++) {

		Bagrow *bgrw = tbl->bagrows[j];
		int remval = bgrw->remval;

		bgrw->rem->copy_label(I2CS(remval));
		if			(remval == 0)	clr = zclr;
		else if	(remval  > 0)	clr = pclr;
		else							clr = nclr;

		for(Bagcell *obj : { bgrw->vis, bgrw->pv, bgrw->rem, bgrw->gvn })
			obj->labelcolor(clr);
	}
	tbl->cnscnt->value(cnsn);
	tbl->vwlcnt->value(vwln);
	tbl->tilecnt->value(tilen);

	if(tilen >= tbl->rckn) {
		tbl->oppcnt->text(opp, tbl->rckn);
		tbl->bagcnt->value(tilen - tbl->rckn);
	}
	else {
		tbl->oppcnt->text(opp, tilen);
		tbl->bagcnt->value(0);
	}
	tbl->notes->buffer(buffy);
	buffy->add_modify_callback(notes_cb, app);
	app->curgame = this;
	if(app->curtbl)
		app->curtbl->hide();			// must always hide noncurrent tbl
	app->curtbl = tbl;
	app->msg->setmsg1();										// update msg
//	tbl->brd[tbl->brdn / 2][tbl->brdn / 2]->take_focus();
	tbl->show();
}
void Game::unload() {								// unload possible UI edits back to game instance

	int j, k;										// copy board and rack

	cout << "unload " << opp << ' ' << starttm << ' ' << this << endl;

	for(j = 0; j < tbl->brdn;	j++)	for(k = 0; k < tbl->brdn; k++)	brd[j * tbl->brdn + k] = tbl->brd[j][k]->kystrk;
	for(j = 0; j < tbl->rckn;	j++)												rck[j] = tbl->rck[j]->kystrk;
	app->curtbl->notes->buffer(0);		// deletes callbacks

	return;
}
void Game::clear() {									// clear board and rack
	memset(brd, FLLRCHR, tbl->brdn * tbl->brdn);
	memset(rck, FLLRCHR, tbl->rckn);
	load(true);											// load will reset counts. true arg allows load to skip several steps asssociated with game switching
}
void Game::rename() {								// edit opponent name

	char *newnm = 0;
	int len;

	if(char *newnm = ty::getopp("Please enter a new name.")) {
		len = strlen(newnm);
		if(len >= OPPMAX) {
			fl_alert("%s", "Name too long.");
			return;
		}
		if(len == 0)
			return;
	}
	delete [] opp;
	opp = new char[len + 1];
	strncpy(opp, newnm, len);
	opp[len] = '\0';
	Brwsr *brwsr = app->brwsr;
	brwsr->remove(brwsr->value());
	brwsr->add_entry(app->curgame);
	brwsr->sort(NULL, app->curgame);
	Numcell *oc = app->curtbl->oppcnt;
	oc->text(opp, oc->value());			// assign new opp name and old count to oppcnt on status line. space for name is a char array of max size.
}
void Game::rematch() {								// clear board, rack, times

	if(2 == fl_choice("Clear board, rack, and timestamps?", "No", 0, "Yes")) {

		clear();
		time_t tm = time(NULL);											// start time
		strftime(starttm, TIMESZ, TIMEFMT, localtime(&tm));	// strftime allegedly nul terminates
		if(stat != GSTAT_ACTV) {
			stat = GSTAT_ACTV;
			app->nactv++;
		}
		app->brwsr->sort(NULL, app->curgame);
	}
}
void Game::toggle(){									// flip between active and inactive status
}
void Game::del() {									// delete game from memory

	if(2 != fl_choice("Delete?", "No", 0, "Yes"))
		return;

	Brwsr *brwsr			= app->brwsr;
	int lnum					= brwsr->value();
	vector<Game*>& gms	= app->games;					// needs to be a reference since vector potentially will be relocated
	Game *gm					= app->curgame;

	if(gm->stat == GSTAT_ACTV)
		app->nactv--;
	app->ngames--;
	brwsr->remove(lnum);										// clear the brwsr item. lnum == line number

	// in order: detach buffer from tbl, delete buffer from game, delete char stars from game, erase game from vector, delete game

	app->curtbl->notes->buffer(0);						// detaching avoids runtime error msgs re unimplemented cb's not being found upon deletion of buffer
	delete gm->buffy;
	for(char *p: { gm->opp, gm->brd, gm->rck })
		delete p;
	gms.erase(find(gms.begin(), gms.end(), gm));
	delete gm;
	app->curgame = nullptr;									// tell load() there is no current game
	lnum = std::min(lnum, brwsr->size());				// keep same menu line number highlighted unless it's out of bounds
	brwsr->select(lnum);										// highlight item
	((Game*) brwsr->data(lnum))->load(false);			// load() will take care of hide old, load, show new (unload is n/a), set app->cur*.
	app->setmod(true);
}