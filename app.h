#ifndef APP_H
#define APP_H
#include "glbl.h"
#include "alpharec.h"
#include "menu.h"
#include "msg.h"


struct Menu;
struct Brwsr;
struct Tbl;
struct Game;
struct Msg;

struct App : Fl_Window {

	optmap_t			opts;
	Menu				*menu;
	Msg				*msg;
	Brwsr				*brwsr;
	vector<Tbl*>	tbls;
	vector<Game*>	games;
	Tbl* 				curtbl;		// might not need since also at curgame->tbl
	Game*				curgame;
	bool 				ismod = false;
	int				ntbls = 0;		// also might not need
	int				nactv = 0;
	int				ngames = 0;

	App();
	void init_opts();							// set baked-in defaults
	void read_opts();							// override from file as specified by user
	bool parse_tmplt_txt(char**, vector<Tbl*>&);
	bool parse_game_txt(char**, vector<Game*>&);
	void parse_game_area(const char *dstname, char *tok, int dstlen, int linenum, Tbl *tbl);
	void game_new(cc *tblnm);
	void save();
	void setmod(bool);
	void write_opts();
	void quit();
};
void app_cb(Fl_Widget*, void*);

#endif // APP_H