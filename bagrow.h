#ifndef BAGROW_H
#define BAGROW_H
#include "glbl.h"
#include "bagcell.h"
#include "alpharec.h"

// these pixel dims for the bag area are mostly based on trial and error and just work
// all heights are cellpix pixels in height unless otherwise indicated
#define VISPIX		16			// width of visible symbol (mostly aka "the letter")
#define PVPIX		20			// width of point value
#define PVVPIX		4			// extra (deeper) height for subscripted point value
#define REMPIX	 	24			// width of remaining qt
#define GVNPIX		36			// width of given qt - needs room for leading '/'
#define BAGW		(VISPIX + PVPIX + REMPIX + GVNPIX)
#define VISFS		22			// font sizes
#define PVFS		14
#define REMFS		18
#define GVNFS		18
#define AR			FL_ALIGN_RIGHT
#define AL			FL_ALIGN_LEFT
#define ALB			(FL_ALIGN_LEFT|FL_ALIGN_BOTTOM)

struct Bagrow : Fl_Group {

	Bagcell *vis, *pv, *rem, *gvn;
	int8_t row, gvnval, remval;									// for load_game to tally individual used letters
	Bagrow(int x, int y, int w, int h, int8_t _row, optmap_t _o, Alpharec*&);
	int handle(int);
	void find_symb();
	void find_reset();

};
#endif // BAGROW_H
