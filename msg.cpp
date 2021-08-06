#include "glbl.h"
#include "msg.h"
#include "app.h"
#include "game.h"

Msg::Msg(int x, int y, int w, int h, optmap_t o) : Fl_Box(FL_FLAT_BOX, x, y, w, h, "Tylosaurus is kindly and will not eviscerate you!") {
	color(o["msg_bg"].ival);
	labelcolor(o["msg_fg"].ival);
	align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
}
void Msg::setmsg1() {

	Game *gm = app->curgame;

	string msgstr = STS(app->nactv) + "/" + STS(app->ngames) + "     " + gm->opp + "     " + gm->starttm;
	if(gm->endtm[0])
		msgstr += "     " + (string) gm->endtm;
	copy_label(msgstr.c_str());
}

