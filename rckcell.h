#ifndef RCKCELL_H
#define RCKCELL_H
#include "tilecell.h"

struct Rckcell : Tilecell {

	Rckcell(int cellpix, int x, int y, int c, optmap_t);
	int handle(int);
};

#endif // RCKCELL_H
