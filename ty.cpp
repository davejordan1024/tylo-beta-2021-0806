#include "ty.h"
#include "app.h"
#include "bagcell.h"
#include "bagrow.h"
#include "numcell.h"

bool ty::check_digits(cc *d, int n) {	// verify that d is all digits and is composed of no more than n digits
	for(int j = 0; j < n; j++)
		if(d[j] && ! isdigit(d[j]))
			return false;
	return true;
}
void ty::squish() {				// move rck letters left until there are no empty slots and set focus to rck % number of nonempty spots
	Rckcell **rck = app->curtbl->rck;
	int j, k = 0, n = app->curtbl->rckn;

	for (j = 0; j < n; j++)
		if(strcmp(rck[j]->label(), FLLRLBL)) {
			rck[k]->kystrk = rck[j]->kystrk;
			rck[k++]->copy_label(rck[j]->label());
		}
	rck[k % n]->take_focus();
	while(k < n) {
		rck[k]->kystrk = FLLRCHR;
		rck[k++]->copy_label(FLLRLBL);
	}
}
void ty::update_cnts(Alpharec *inc, Alpharec *dec) {			// does not refer to any cells having user interaction

	int qt, change = 1;
	Bagrow *thisrow;
	Bagcell *bagrem;
	Tbl *tbl = app->curtbl;

	const static int
		nclr	= app->opts["bag_nega"].ival,
		zclr	= app->opts["bag_zero"].ival,
		pclr	= app->opts["bag_posi"].ival;

	for(Alpharec *p : { inc, dec }) {
		if(p) {
			thisrow = tbl->bagrows[p->row];
			bagrem = thisrow->rem;
			bagrem->copy_label(I2CS(qt = atoi(bagrem->label()) + change));

			int clr;

			if			(qt > 0)	clr = pclr;
			else if	(qt < 0)	clr = nclr;
			else					clr = zclr;

			for(Bagcell *c : { thisrow->vis, thisrow->pv, thisrow->rem, thisrow->gvn }) {
				c->labelcolor(clr);
				c->redraw();			// rem[row] actually changed its label content and so redraws without being told to. so maybe it'll redraw twice.
			}

			switch(p->snr) {
				case 'c':	tbl->cnscnt->incr(change); break;
				case 'v':	tbl->vwlcnt->incr(change); break;
				case 'e':	tbl->cnscnt->incr(change);
								tbl->vwlcnt->incr(change); break;
								// default:	anything else is fatal in read_tmplts()
			}
			int tot = tbl->tilecnt->incr(change);
			if(tot >= tbl->rckn)
				tbl->oppcnt->value(tbl->rckn);
			else
				tbl->oppcnt->incr(change);
			tbl->bagcnt->value(tot - tbl->oppcnt->value());
		}
		change = -1;
	}
}
void ty::copyfrom(cc *src) {	// arg is either "rack" or "bag". copy to clipboard, then save all games to disk, because why not?

	cout << "copy to clipboard\n";

	Tbl *tbl = app->curtbl;
	int j, k, n;
	string clp_str;

	if(! strcmp(src, "copy rack")) {		// loop thru rack, ignoring empty slots

		Alpharec *match;

		n = tbl->rckn;
		for(j = 0; j < n; j++) {
			if(tbl->rck[j]->kystrk == FLLRCHR)
				continue;
			if(match = ty::srch_keytab(tbl->alphas, tbl->alphan, tbl->rck[j]->kystrk))	// find keystroke in keytab, append corresponding clip symbol
				clp_str += match->clprep;
		}
	}
	else if(! strcmp(src, "copy bag")) {

		// append rem copies of each letter to clp_str. num of copies (rem) comes from bagrows. offset into bagrows comes fom alphas, as does
		// the clipboard representation of the letter. cant use string.append(n, char) b/c clprep is not char type.
		// all the alpha & bag vars are from tmplt file and are already sanitized.

		for(j = 0; j < tbl->alphan; j++) {
			for(k = 0; k < tbl->bagrows[tbl->alphas[j]->row]->remval; k++)
				clp_str += tbl->alphas[j]->clprep;
		}
	}
	else
		fl_alert("%s: internal error, invalid source", src);
	Fl::copy(clp_str.c_str(), clp_str.size(), 1, Fl::clipboard_plain_text);
	if(app->ismod) app->save();
}
bool ty::check_isreg(const char *path, bool req) {		// arg true: file must exist and be regular; false: must be reg if exists

	//#if __cplusplus>=201703		// conditional include does not work inside function
	//#include <filesystem>
	//	exists	= std::filesystem::exists(path);
	//	modeok	= std::filesystem::is_regular_file(std::filesystem::status(path));

	bool exists, modeok;
	exists = ! access(path, F_OK);							// access returns 0 if file exists
	struct stat s;
	stat(path, &s);
	modeok = S_ISREG(s.st_mode);


	if(exists) {
		if(modeok)
			return true;
		else {
			fl_alert("%s is the wrong mode.", path);
			return false;
		}
	}
	else {
		if(req) {
			fl_alert("Required file \"%s\" doesn't exist.", path);
			return false;
		}
		else
			return true;	// doesnt exist and isnt required
	}
}
char* ty::getopp(cc *prompt) {

	char *ret = NULL;

	while(true) {
		cc *rsp;
		if(rsp = fl_input(prompt)) {				// fl_input returns NULL on cancel btn, close btn, escape, F4
			int rsplen = strlen(rsp);
			if(rsplen < OPPMAX) {
				if(rsplen > 0) {						// it IS quite possible for len to be 0
					ret = new char[rsplen + 1];
					strncpy(ret, rsp, rsplen);
					ret[rsplen] = '\0';
				}
				else
					cout << "rsplen == 0\n";
				break;
			}
			else
				fl_alert("That name is too long, please try again.");
		}
		else
			break;
	}
	return ret;
}
void ty::tylohelp(){}
void ty::userhelp(){}

Alpharec* ty::srch_keytab(vector<Alpharec*>&alpha, int btm, int ltr) {

	int top = 0, mid;

	btm--;
	while(top <= btm) {

		mid = top + (btm - top) / 2;

		if			(alpha[mid]->kystrk < ltr)	top = mid + 1;
		else if	(alpha[mid]->kystrk > ltr)	btm = mid - 1;
		else		return alpha[mid];
	}
	return NULL;
}
