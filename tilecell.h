#ifndef tilecell
#define tilecell
#include "glbl.h"

struct Tilecell : Fl_Box {

	char kystrk;
	int savecolor, lolite;
	int8_t row, col;

	Tilecell(int x, int y, int w_h, int8_t r, int8_t c, optmap_t);
	int handle(int);
};

#endif
