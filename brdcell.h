#ifndef BRDCELL_H
#define BRDCELL_H
#include "tilecell.h"

struct Brdcell : Tilecell {

	Brdcell(int cellpix, int x, int y, int8_t r, int8_t c, optmap_t);
	int handle(int);

};

#endif // BRDCELL_H
