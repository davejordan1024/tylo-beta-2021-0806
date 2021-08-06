#include "app.h"
#include "alpharec.h"
#include "glbl.h"
#include "bagrow.h"
#include "brwsr.h"
#include "brdcell.h"
#include "numcell.h"
#include "rckcell.h"
#include <iomanip>
#include <cassert>

App *app;				// extern App *app is declared in glbl.h.

int main(int argc, char *argv[]) {

	cout		<< APPTTL << ' ' << APPVER << '\n'
				<< APPTTL << " is copyright 2021 by Dave Jordan and "
				"is based in part on the work of the FLTK project (http://www.fltk.org)." << "\n"
				<< "FLTK " << FL_VERSION << ", __cplusplus " << __cplusplus << endl;

	app = new App();
	app->curtbl = nullptr;
	app->curgame = nullptr;
	if(app->ngames)
		app->brwsr->load_first(app->opts["lastgame"].sval);
	app->setmod(false);
	cout << "init done\n";
	app->show();
	Fl::run();
}
