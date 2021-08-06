#ifndef TY_H
#define TY_H
#include "glbl.h"
#include "alpharec.h"
#include "rckcell.h"

namespace ty {
	bool 		check_digits(cc *d, int n);
Alpharec*	srch_keytab	(vector<Alpharec*>&, int, int);
	void		copyfrom		(cc *where);							// seems problematic casting to Menu* on the way to the function
	void		update_cnts	(Alpharec *inc, Alpharec *dec);
	bool		check_isreg	(cc *path, bool req);
	char*		getopp		(cc *prompt);
	void		tylohelp		();
	void		userhelp		();
	void		squish		();
}

#endif // TY_H
