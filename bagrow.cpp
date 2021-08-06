#include "bagrow.h"
#include "app.h"
#include "brdcell.h"
#include "rckcell.h"

Bagrow::Bagrow(int x, int y, int w, int h, int8_t _row, optmap_t o, Alpharec* &alpha) : Fl_Group(x, y, w, h) {

	current(this);

	color(o["bag_bg"].ival);
	box(FL_FLAT_BOX);

	row = _row;					// might not need
	gvnval = alpha->gvn;
	remval = alpha->rem;

	vis	= new Bagcell(x,				y, VISPIX,	h,				VISFS,	AR,	o, alpha->uirep);
	pv		= new Bagcell(x += VISPIX,	y, PVPIX,	h + PVVPIX,	PVFS,		ALB,	o, I2CS(alpha->pv));
	rem	= new Bagcell(x += PVPIX,	y, REMPIX,	h,				REMFS,	AR,	o, I2CS(remval));
	gvn	= new Bagcell(x += REMPIX,	y, GVNPIX,	h,				GVNFS,	AL,	o, ("/" + STS(gvnval)).c_str());

	current(this->parent());
}
int Bagrow::handle(int e) {
	if(e == FL_PUSH) {
		this->find_symb();
		return EVHANDLED;
	}
//	return this->parent()->handle(e);
	return EVNOTHANDLED;
}
void Bagrow::find_symb() {

	string		symb	= this->vis->label();
	static bool	found = false;
	static int	clr	= app->opts["find_hi"].ival;
	Tbl			*tbl	= app->curtbl;
	int8_t		brdn	= tbl->brdn,
					rckn	= tbl->rckn,
					j, k;

	if(found) {
		found = false;
		Bagrow::find_reset();
	}
	else {
		for(j = 0; j < brdn; j++)
			for(k = 0; k < brdn; k++)
				if(! symb.compare(tbl->brd[j][k]->label())) {
					tbl->brd[j][k]->labelcolor(clr);
					tbl->brd[j][k]->redraw();
					found = true;
				}
		for(j = 0; j < rckn; j++)
			if(! symb.compare(tbl->rck[j]->label())) {
				tbl->rck[j]->labelcolor(clr);
				tbl->rck[j]->redraw();
				found = true;
			}
	}
}
void Bagrow::find_reset() {

	static int clr = app->opts["cell_fg"].ival;
	Tbl *tbl = app->curtbl;
	int	brdn = tbl->brdn,
			rckn = tbl->rckn,
			j, k;

	for(j = 0; j < brdn; j++)	for(k = 0; k < brdn; k++)	{ tbl->brd[j][k]	->labelcolor(clr); tbl->brd[j][k]->redraw(); }
	for(j = 0; j < rckn; j++)										{ tbl->rck[j]		->labelcolor(clr); tbl->rck[j]	->redraw(); }
}