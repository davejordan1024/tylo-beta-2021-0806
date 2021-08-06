#ifndef MSG_H
#define MSG_H
#include "glbl.h"
#include "app.h"

struct Msg : Fl_Box {
	Msg(int x, int y, int w, int h, optmap_t);
	void setmsg1();
};
#endif // MSG_H
