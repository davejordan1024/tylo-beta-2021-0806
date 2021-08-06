#include "glbl.h"
#include "app.h"
#include "tbl.h"
#include "brdcell.h"
#include "rckcell.h"
#include "numcell.h"
#include "bagcell.h"
#include "bagrow.h"
#include "notes.h"
#include "alpharec.h"

Tbl::Tbl(optmap_t o) : Fl_Group(0, 0, 0, 0) {

	begin();
	hide();									// it is not enough to show() the desired table, all others must be hidden
	box(FL_FLAT_BOX);
	color(o["tbl_bg"].ival);			// color(FL_RED);
	notes = new Notes(0, 0, 0, 0, o);
	end();									// without end(), begin()'s scope continues indefinitely
}
//  for the remaining Tbl:: methods, tbl->begin() was set by caller =============================================================
//int handle(int) { cout << "grp hndl\n" ; }

void Tbl::mk_brd_ui(int rightx, int brdw, int brdy, int cellpix, optmap_t o) {

	int		brdx = this->x() + (rightx - this->x() - brdw) / 2,		// "this" is the tbl. brdw is brdn * cellpix
				brdn = this->brdn,
				thisx, j, k;
	Brdcell	***&brd = this->brd;		// &brd makes valgrind shut up even though reference changes nothing

	try {
		brd = new Brdcell**[brdn];
		for(j = 0; j < brdn; j++) {
			brd[j] = new Brdcell*[brdn];
			thisx = brdx;
			for(k = 0; k < brdn; k++) {
				brd[j][k] = new Brdcell(cellpix, thisx, brdy, j, k, o);
				thisx += cellpix;
			}
			brdy += cellpix;
		}
	}
	catch(...) {
		{ EFTL("mk_tbl_ui"); }
	}
	if(this->clrs) {
		for(j = 0; j < brdn; j++)
			for(k = 0; k < brdn; k++)
				brd[j][k]->color(brd[j][k]->lolite = clrs[j * brdn + k]);		// 2 assignments
		delete [] clrs;
	}
	return;
}
void Tbl::mk_status_line(int x, int y, int w, int cellpix, optmap_t o) {
	try {
		oppcnt = new Numcell(x, y, w * 3/4, cellpix, "opp", 0, o);		// 3/4: proportion of width for opponent
		x += oppcnt->w();
		bagcnt = new Numcell(x, y, w * 1/4, cellpix, "bag", 0, o);
		bagcnt->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	}
	catch(...) {
		{ EFTL("mk_status_line"); }
	}
}
void Tbl::mk_rck_ui(int rightx, int rckw, int rcky, int cellpix, optmap_t o) {

	int rckx = this->x() + (rightx - this->x() - rckw) / 2;
	try {
		this->rck = new Rckcell*[this->rckn];
		for(int j = 0; j < this->rckn; j++)
			this->rck[j] = new Rckcell(cellpix, rckx + j * cellpix, rcky, j, o);
	}
	catch(...) {
		{ EFTL("mk_rck_ui"); }
	}
}
void Tbl::mk_bag(int x, int y, int w, int h, optmap_t o) {

	vector<Alpharec*>thisalphas = this->alphas;		// assignment of one vector to another makes a deep copy

	std::sort(thisalphas.begin(), thisalphas.end(), [](Alpharec *a1, Alpharec *a2) { return a1->row < a2->row; });

	this->bagrows = new Bagrow*[this->alphan];

	for(int j = 0; j < this->alphan; j++) {								// 1 iter for every letter in alphabet + blank
		this->bagrows[j] = new Bagrow(x, y, w, h, j, o, thisalphas[j]);
		y += h;
	}
	// the three totals below the bag

	cnscnt = new Numcell(x, y += 4,	BAGW, h, "cnsnts ", cnsn, o);		// 4 pixels padding
	vwlcnt = new Numcell(x, y += h,	BAGW, h, "vowels ", vwln, o);
	tilecnt= new Numcell(x, y += h,	BAGW, h, "tiles  ", tilen, o);
}
