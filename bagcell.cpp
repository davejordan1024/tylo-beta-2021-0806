#include "glbl.h"
#include "app.h"
#include "bagcell.h"

// label font sizes are different for different bagcell subtypes. they are specified in bagrow.cpp.

Bagcell::Bagcell(int x, int y, int w, int h, int lblfs, const Fl_Align& just, optmap_t o, cc *lbl) : Fl_Box(x, y, w, h) {
	box(FL_FLAT_BOX);						labelfont(FL_COURIER);
	labelsize(lblfs);						color(o["bag_bg"].ival);
	labelcolor(o["bag_posi"].ival);	align(just | FL_ALIGN_INSIDE);
	copy_label(lbl);
}
