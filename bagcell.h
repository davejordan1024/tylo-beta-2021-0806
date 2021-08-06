#ifndef BAGCELL_H
#define BAGCELL_H

struct Bagcell : Fl_Box {

	Bagcell(int x, int y, int w, int h, int lblsz, const Fl_Align& just, optmap_t o, cc *lbl);
};
#endif // BAGCELL_H
