#ifndef TBL_H
#define TBL_H

#include "alpharec.h"

struct Brdcell;
struct Rckcell;
struct Numcell;
struct Bagrow;
struct Notes;

struct Tbl : Fl_Group {

	vector<Alpharec*>	alphas;

	Brdcell	***brd;
	Rckcell	**rck;
	Numcell	*oppcnt, *bagcnt, *cnscnt, *vwlcnt, *tilecnt;
	Bagrow	**bagrows;
	Notes		*notes;
	int8_t	brdn, rckn, alphan, cnsn = 0, vwln = 0, tilen = 0;
	int		*clrs;
	char		*name;

	Tbl(optmap_t);
	void mk_brd_ui			(int x, int y, int w, int h, optmap_t o);
	void mk_status_line	(int x, int y, int w, int h, optmap_t o);
	void mk_rck_ui			(int x, int y, int w, int h, optmap_t o);
	void mk_bag				(int x, int y, int w, int h, optmap_t o);

	static int alphasort(const void *rec1, const void *rec2) {								// for qsort
		return ((Alpharec*) rec1)->kystrk - ((Alpharec*) rec2)->kystrk;
	}
	void parse_alpha_txt(char **, Alpharec*&, int);
//	int handle(int);
};
#endif // TBL_H
