#ifndef NOTES_H
#define NOTES_H
#include "glbl.h"

struct Notes : Fl_Text_Editor {

	Notes(int x, int y, int w, int h, optmap_t o);
	int handle(int);
};
void notes_cb(int, int nInserted, int nDeleted, int, const char*, void* v);
#endif // NOTES_H
