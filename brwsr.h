#ifndef BRWSR_H
#define BRWSR_H
#include "glbl.h"
#include "app.h"
#include "game.h"

struct Brwsr : Fl_Hold_Browser {

	string lastsort;

	Brwsr(int x, int y, int h, vector<Game*>&entries, optmap_t);

	void load_first		(string);
	void load_game			(Game*);
	void add_entry			(Game*&);
	void sort				(cc*, Game*);
	int handle(int);
};

namespace brwsrsort {						// passing a member function from an array to std::sort is much to complicated
	bool by_name				(Game*, Game*);
	bool by_start				(Game*, Game*);
	bool by_end					(Game*, Game*);
	bool by_status_by_name	(Game*, Game*);
}
void brwsr_cb(Fl_Widget *brwsr, void*);
#endif // BRWSR_H
