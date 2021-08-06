#ifndef GAME_H
#define GAME_H

struct Tbl;

struct Game {

	Tbl				*tbl;
	char				stat, *tblname, *opp, *brd, *rck, starttm[TIMESZ], endtm[TIMESZ];
	string			fmtd;
	Fl_Text_Buffer *buffy;

	// ctor #2 for parse_game_txt. ctor #1 for new game

	Game(Tbl *tbl, cc *opp);
	Game(char _stat, Tbl *_tbl, char *_tblname, char *_opp, char *_starttm, char *_endtm, char *_brd, char *_rck, char *_notes);
	void load(bool reload);			// load game instance into UI
	void unload();						// unload back to game instance
	void rename();
	void rematch();
	void toggle();
	void clear();
	void del();
	// disk read and write are app methods
};
#endif