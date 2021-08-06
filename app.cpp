#include <regex>			// read_opts()
#include "app.h"
#include "bagrow.h"		// BAGW
#include "brwsr.h"
#include "game.h"
#include "notes.h"

// chars in line in tmplt file: 3 (brdn|) + 3 (rckn|) + 3 (alphan|) + 33 (opponent name|) + alphamax * 20 (alpha descriptions) + brdmax^2*4+1 (colors|)
#define TMPLTMAX	(3 + 3 + 3 + 33 + ALPHAMAX * 20 + (BRDMAX * BRDMAX * sizeof("ffff") - 1) + 1)
#define TOKBAD		(tok == nullptr || ! *tok)

App::App() : Fl_Window(0, 0, 0, 0) {

	begin();
	init_opts();
	read_opts();
	curtbl = nullptr;
	curgame = nullptr;
	color(opts["app_bg"].ival);
	callback(app_cb);								// cb's function is to intercept escape and the close button on the titlebar

	cout << "read template file\n"; // ============================================================
	if(! ty::check_isreg(TMPLTFL, true)) {
		fl_alert("%s is either missing or is not a regular file. Can't continue.", TMPLTFL);
		exit(EXIT_FAILURE);
	}
	std::ifstream tmpltfl(TMPLTFL);
	if(! tmpltfl) {
		fl_alert("%s is not openable. Can't continue.", TMPLTFL);
		exit(EXIT_FAILURE);
	}
	string tmplt_line;
	bool ok;
	while(getline(tmpltfl, tmplt_line)) {				// pushes onto vector tbl and vector alphas
		char *line = (char *) tmplt_line.c_str();
		ok = parse_tmplt_txt(&line, tbls);
		if(! ok) break;

		// sort alphas so srch_keytab can work in parse_game_txt, coming up next, and throughout rest of session.
		// a temporary deep copy will be made and "unsorted" back to physical order by make_bag_ui, for display purposes.

		vector<Alpharec*>& thisalphas = tbls.back()->alphas;
		std::sort(thisalphas.begin(), thisalphas.end(), [](Alpharec *a1, Alpharec *a2) { return a1->kystrk < a2->kystrk; });
		cout << "   " << tbls.back() << ' ' << tbls.back()->name << endl;
	};
	tmpltfl.close();

	if(! ok)						// if not ok msg was already displayed
		exit(EXIT_FAILURE);
	if(! (ntbls = tbls.size())) {
		fl_alert("There is no table info in %s.", TMPLTFL);
		exit(EXIT_FAILURE);
	}
	cout << ntbls << " tables\n";

	cout << "read game file\n"; // ========================================
	if(! ty::check_isreg(GAMEFL, false))	{				// missing is okay
		fl_alert("%s is not a regular file.", GAMEFL);
		exit(EXIT_FAILURE);
	}
	std::ifstream gamefl(GAMEFL);
	if(! gamefl) {
		fl_alert("%s is not creatable and/or openable.", GAMEFL);
		exit(EXIT_FAILURE);
	}
	string game_line;

	while(getline(gamefl, game_line)) {
		char *line = (char *) game_line.c_str();
		if(parse_game_txt(&line, games)) {					// check sanity (including verify tbl name against known names), push onto games vector
			if(games.back()->stat == GSTAT_ACTV)				// count of active games
				nactv++;
			cout << "   " << games.back() << ' ' << games.back()->opp << ' ' << games.back()->starttm << endl;
		}
	}
	gamefl.close();
	ngames = games.size();		// count of all games - 0 is ok
	cout << ngames << " games\n";
	cout << "create browser\n"; // ========================================================

	if(! (brwsr = new nt Brwsr(0, 0, 0, games, opts)))
		{ EFTL("new browser"); }

	cout << "set object coords and sizes\n"; // ========================================

	{	int8_t	pad = opts["pad"].ival,
					cellpix = opts["cellpix"].ival;

		// app width is based on brwsr width and the biggest brdn. maxvrtpx is based on biggest alphan and has 3 extra cell heights to fit 3 bag totals
		// underneath. maxvrtpx does not include menu or msg height, which are constant across all tables.

		int maxbrdpx = (*std::max_element(tbls.begin(), tbls.end(), [] (Tbl *t1, Tbl *t2) { return t1->brdn < t2->brdn;		}))->brdn * cellpix;
		int maxvrtpx =((*std::max_element(tbls.begin(), tbls.end(), [] (Tbl *t1, Tbl *t2) { return t1->alphan < t2->alphan;	}))->alphan + 3) * cellpix;
		int uservert = opts["minpix"].ival;
		if(uservert > maxvrtpx)					// user can specify even more height if desired, e.g. for game browser
			maxvrtpx = uservert;

		int	brwsrw	= brwsr->w(),			// browser width is based on options for its two column widths
				tblx		= pad + brwsrw,
				tbly		= 2 * LBLH,
				tblw		= 2 * pad + maxbrdpx + 2 * pad + BAGW,
				tblh		= maxvrtpx,
				appx		= opts["appx"].ival,
				appy		= opts["appy"].ival,
				appw		= pad + brwsrw + tblw,
				apph		= 2 * LBLH + tblh + pad;

		resize(appx, appy, appw, apph);

		cout << "create menu & message area\n";
		if(! (menu = new nt Menu(0, 0, appw, LBLH, tbls, opts)))		{ EFTL("new menu"); }
		if(! (msg = new nt Msg(pad, menu->h(), appw, LBLH, opts)))	{ EFTL("new message"); }
		brwsr->resize(pad, msg->y() + msg->h(), brwsrw, maxvrtpx);

		// remainder of table objects ======================================
		int j = 0;
		for(Tbl *t : tbls) {
			j++;
			t->resize(tblx, tbly, tblw, tblh);
			t->begin();
			int y = msg->y() + msg->h() + maxbrdpx - t->brdn * cellpix;
			cout << "create board #" << j << endl;
			t->mk_brd_ui(appw - BAGW, t->brdn * cellpix, y, cellpix, opts);						// change to (rightx, brdy, brdpix, cellpix)
			y += maxbrdpx + pad;
			cout << "create status line #" << j << endl;;
			t->mk_status_line(tblx + 2 * pad, y, maxbrdpx, cellpix, opts);
			y += cellpix + pad;
			cout << "create rack #" << j << endl;;
			t->mk_rck_ui(appw - BAGW, t->rckn * cellpix, y, cellpix, opts);
			y += cellpix + pad;
			t->notes->resize(tblx + 2 * pad, y, maxbrdpx, apph - pad - y);
			y = msg->y() + msg->h();
			cout << "create bag #" << j << endl;;
			t->mk_bag(appw - BAGW, y, BAGW, cellpix, opts);		// includes 3 totals below bag
			t->end();
			t->parent()->begin();
		}
	}
	end();
}
void App::init_opts() {

	#define FL_DY FL_DARK_YELLOW
	#define DFLT_SRT "by status by name"
	//					key,			{ ival, 	sval, 	type,	shift, 	write }
	opts.insert( { "appx",		{ 500,		"",	'i',	false,	true  }});	// app x coordinate at startup
	opts.insert( { "appy",		{ 100,		"",	'i',	false,	true  }});	// app y coordinate at startup
	opts.insert( { "app_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// app background color
	opts.insert( { "msg_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// message area background color
	opts.insert( { "msg_fg",	{ FL_WHITE,	"",	'i',	false,	false }});	// message font color
	opts.insert( { "msg_fs",	{ 14,			"",	'i',	false,	false }});	// message font size
	opts.insert( { "list_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// game list background color
	opts.insert( { "list_fg",	{ FL_WHITE,	"",	'i',	false,	false }});	// game list font color
	opts.insert( { "list_fs",	{ 14,			"",	'i',	false,	false }});	// game list font size
	opts.insert( { "lw_opp",	{ 150,		"",	'i',	false,	false }});	// opponent column width in pixels
	opts.insert( { "lw_time",	{ 80,			"",	'i',	false,	false }});	// timestamp column width in pixels

	opts.insert( { "cellpix",	{ 24,			"",	'i',	false,	false }});	// board/rack cell size in pixels
	opts.insert( { "minpix",	{ 0,			"",	'i',	false,	false }});	// min height for list and bag
	opts.insert( { "cell_bg",	{ 0x2e3436,	"",	'I',	true,		false }});	// board/rack background color
	opts.insert( { "cell_fg",	{ FL_WHITE,	"",	'i',	false,	false }});	// board/rack font color
	opts.insert( { "cell_fs",	{ 24,			"",	'i',	false,	false }});	// board/rack font size
	opts.insert( { "cell_hi",	{ FL_DY,		"",	'i',	false,	false }});	// board/rack current cell color

	opts.insert( { "clr_tw",	{ 0xF07162,	"",	'I',	true, 	false	}});	// triple word color
	opts.insert( { "clr_dw",	{ 0xEA201B,	"",	'I',	true, 	false	}});	// double word color
	opts.insert( { "clr_tl",	{ 0xA8C269,	"",	'I',	true, 	false	}});	// tiple letter color
	opts.insert( { "clr_dl",	{ 0x53A3D4,	"",	'I',	true, 	false	}});	// double letter color
	opts.insert( { "clr_nm",	{ 0x2e3436,	"",	'I',	true, 	false	}});	// no-multiplier color

	opts.insert( { "stat_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// status line background color
	opts.insert( { "stat_fs",	{ 14,			"",	'i',	false,	false }});	// status line font size
	opts.insert( { "stat_fg",	{ FL_WHITE,	"",	'i',	false,	false }});	// status line font color

	opts.insert( { "notes_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// notes area background color
	opts.insert( { "notes_fs",	{ 18,			"",	'i',	false,	false }});	// notes font size
	opts.insert( { "notes_fg",	{ FL_WHITE,	"",	'i',	false,	false }});	// notes font color
	opts.insert( { "notes_cu",	{ FL_WHITE,	"",	'i',	false,	false }});	// notes cursor color

	opts.insert( { "bag_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// bag area background color
	opts.insert( { "bag_posi",	{ FL_WHITE,	"",	'i',	false,	false }});	// bag letter: some remaining
	opts.insert( { "bag_zero",	{ 0x2e3436,	"",	'I',	true,		false	}});	// bag letter: none remaining
	opts.insert( { "bag_nega",	{ FL_RED,	"",	'i',	false,	false }});	// bag letter: < 0 remaining
	opts.insert( { "pad",		{ 8,			"",	'i',	false,	false }});	// pixels separating app components
	opts.insert( { "find_hi",	{ FL_RED,	"",	'i',	false,	false }});	// found letter font color
	opts.insert( { "autoright",{ 1,			"",	'i',	false,	false }});	// rack: auto advance to next cell
	opts.insert( { "tbl_bg",	{ FL_BLACK,	"",	'i',	false,	false }});	// color for game-specific widget area
	opts.insert( { "lastgame",	{ -1,			"",	's',	false,	true  }});	// last game shown before exit
	opts.insert( { "lastsort",	{ -1,	DFLT_SRT,	's',	false,	true  }});	// last commanded sort before exit (but not used during next init)

	#undef DFLT_SRT
	#undef FL_DY
}
void App::read_opts() {

	using std::regex;
	using std::smatch;
	using std::regex_match;

	cout << "read " << OPTFL << endl;

	ty::check_isreg(OPTFL, false);
	std::fstream optfs(OPTFL, std::ios::in);
	if(! optfs) {
		fl_alert("%s is nonexistent or unopenable.", OPTFL);
		return;
	}
	string optline;

	while(getline(optfs, optline)) {		// search each line in the file to see if there is a match to the option name starting at line[0]

		if(optline.empty())
			continue;

		optmap_t::const_iterator iter = opts.begin();

		while(iter != opts.end() && optline.compare(0, iter->first.length(), iter->first))
			iter++;
		if(iter == opts.end()) {
			fl_alert("%s does not match any option name", optline.c_str());
			continue;	// next getline()
		}
		string keyfnd = iter->first;
		string rhs = string(optline, keyfnd.length());	// rhs starts right after option name. regex_match does not like string() to be passed directly
		smatch matches;											// if they exist, match[0] is all of piece2, [1] is the value

		if(regex_match(rhs, matches, regex("^[ \t=]+(.*?)[ \t]*$")) && matches.size() > 1) {	// trim/isolate value part of option

			opt_s &scnd = opts[keyfnd];			// must use reference
			if(scnd.type != 's')
				scnd.ival = std::stoul(matches[1], nullptr, 0);
			else
				scnd.sval.assign(matches[1]);
			scnd.write = true;
		}
		else
			fl_alert("%s is a malformed option", optline.c_str());
	}
	optfs.close();

	for(optmap_t::/*nonconst*/iterator iter = opts.begin(); iter != opts.end(); iter++)		// shift all 'I' type vals whatever their source
		if(iter->second.type == 'I')
			iter->second.ival <<= 8;
cout << "read opts " << opts["lastgame"].sval << endl;
	return;
}
bool App::parse_tmplt_txt(char **tblsep, vector<Tbl*> &tbls) {

	#define TYLOFL TMPLTFL

	// a large amount of parsing and sanity checking of a line of text describing a table, its associated alphabet specs and, if present, the colors to
	// use for its cells when/after they are instantiated. No FLTK widgets other than the table itself are instantiated here. If this func does not
	// trigger any ERR*, the table pointer will be pushed onto vector<Tbl*>&tbls. Else, depending on the error, app will either display a user friendly
	// msg widget, return(), and exit, or write a msg to stdout and exit immediately.

	static int linenum;
	int len;
	Tbl *tbl;
	char *tok;

	linenum++;

	len = strlen(*tblsep);
	if(len == 0)
		return true;											// let blank lines slide
	if(len > TMPLTMAX)										{ ERNG("line length", 0, (int) TMPLTMAX); }

	if(! (tbl = new nt Tbl(opts)))								{ EFTL("new table"); }

	tok = strsep(tblsep, "|");
	if(TOKBAD)													{ EGEN("missing table name."); }
	len = strlen(tok);
	if(! (tbl->name = new nt char[len + 1]()))		{ EFTL("new table name"); }		// new char[n] only works if followed by () or n is constant
	strncpy(tbl->name, tok, len);
	tok = strsep(tblsep, "|");
	if(TOKBAD)													{ EGEN("missing board size."); }
	tbl->brdn = atoi(tok);
	if(tbl->brdn < 2 || tbl->brdn > BRDMAX)			{ ERNG("board size", 2, BRDMAX); }

	tok = strsep(tblsep, "|");
	if(TOKBAD)													{ EGEN("missing rack size."); }
	tbl->rckn = atoi(tok);
	if(tbl->rckn < 2 || tbl->rckn > BRDMAX)			{ ERNG("rack size", 2, BRDMAX); }

	tok = strsep(tblsep, "|");
	if(TOKBAD)													{ EGEN("missing alphabet size."); }
	tbl->alphan = atoi(tok);

	if(tbl->alphan < 1 || tbl->alphan > ALPHAMAX)	{ ERNG("alphabet size", 1, ALPHAMAX); }

	int8_t ltrnum;

	for(ltrnum = 0; ltrnum < tbl->alphan; ltrnum++) {	// parse alphabet: kbd|clipbrd|ui|point val|given qt|sonority (snr)

		Alpharec *thisrec = new Alpharec();

		thisrec->row = ltrnum;		// the rows will be sorted by keystroke for srch_keytab() but later displayed in order encountered here.

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of keystroke."); }
		if(strlen(tok) != 1)								{ ELTR("invalid keystroke length."); }
		thisrec->kystrk = tok[0];

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of clipboard symbol."); }
		if(strlen(tok) > 4)								{ ELTR("clipboard rep > 4 bytes."); }
		strncpy(thisrec->clprep, tok, 5);

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of UI rep."); }
		if(strlen(tok) > 4)								{ ELTR("UI rep > 4 bytes."); }
		strcpy(thisrec->uirep, tok);

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of point value"); }
		if(! ty::check_digits(tok, 2))						{ ELTR("point value is invalid"); }
		thisrec->pv = atoi(tok);

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of given quantity"); }

		if(! ty::check_digits(tok, 2))						{ ELTR("given quantity is invalid"); }
		thisrec->gvn = thisrec->rem = atoi(tok);

		if(! (tok = strsep(tblsep, "|")))			{ EGEN("end of line instead of letter type."); }
		if(strlen(tok) > 1)								{ ELTR("letter type is too long."); }

		switch(thisrec->snr = tolower(tok[0])) {	// count number of consonants & vowels. some letters can be 'e' for either
			case 'c':	tbl->cnsn	+= thisrec->gvn;	break;
			case 'v':	tbl->vwln	+= thisrec->gvn;	break;
			case 'e':	tbl->cnsn	+= thisrec->gvn;
							tbl->vwln	+= thisrec->gvn;	break;
			default:											{ ELTR("invalid letter type"); }
		}
		tbl->tilen += thisrec->gvn;					// count total number of tiles separately so 'e's dont overcontribute
		tbl->alphas.push_back(thisrec);
	}	// end alpha loop
	if(ltrnum < tbl->alphan)							{ EGEN("mismatch between stated alphabet size and number of letters encountered."); }

	if(! (tok = strsep(tblsep, "|")))				// does template specify color? ideally there are either 0 or brdn^2 color specifiers.
		tbl->clrs = nullptr;								// but i accept anything in between
	else {
		int nclrs = tbl->brdn * tbl->brdn;

		if(! (tbl->clrs = new nt int[nclrs]()))	{ EFTL("color array."); }

		static int	dl = opts["clr_dl"].ival,			// color for double letter, triple word, etc
						tl = opts["clr_tl"].ival,
						dw = opts["clr_dw"].ival,
						tw = opts["clr_tw"].ival,
						bg = opts["app_bg"].ival,			// app background
						nm = opts["clr_nm"].ival;			// "no multiplier". a catchall if needed.
		bool stop = false;
		int j;

		for(j = 0; j < nclrs; j++) {
			switch(tok[j]) {
				case '.'		:	tbl->clrs[j]	= nm;	break;
				case 'd'		:	tbl->clrs[j]	= dl;	break;
				case 't'		:	tbl->clrs[j]	= tl;	break;
				case 'D'		:	tbl->clrs[j]	= dw;	break;
				case 'T'		:	tbl->clrs[j]	= tw;	break;
				case '\0'	:	stop = true;			break;
				default		:	tbl->clrs[j]	= bg;	break;
			}																	// if # of colors not exact, warn but accept
			if(stop) {
				fl_alert("%s line %d: number of colors < board size.", TMPLTFL, linenum);
				break;
			}
		}
		if(tok[j])
			fl_alert("%s line %d: number of colors > board size", TMPLTFL, linenum);
	}		// end else-has-colors block
	tbls.push_back(tbl);
	return true;
	#undef TYLOFL
}
bool App::parse_game_txt(char **gmsep, vector<Game*> &games) {

	#define TYLOFL GAMEFL

	Tbl *tbl;
	char *tok, stat, *tblname, *opp, *starttm, *endtm, *brd, *rck, *notes;
	static int linenum = 1;

	if(strlen(*gmsep) > GMMAX)	{ EGEN("too many characters"); }
	tok = strsep(gmsep, "|");
	if(TOKBAD)						{ EGEN("missing game status"); }
	switch(tok[0]) {
		case GSTAT_OVER: break;
		case GSTAT_ACTV: break;
		default:
			fl_alert("Invalid status for game #%d, changing to active.", linenum);
			tok[0] = GSTAT_ACTV;
	}
	stat = tok[0];

	tok++;
	if(TOKBAD)											// test for \0 important. test for nullptr unnecessary
		{ EGEN("missing template name"); }
	int k;
	for(k = 0; k < ntbls; k++) {
		if(! strcmp(tok, tbls[k]->name)) {
			tbl = tbls[k];						// put ptr to table into game struct
			tblname = tbl->name;
			break;
		}
	}
	if(k == ntbls)
		{ EGEN("unknown template name"); }

	tok = strsep(gmsep, "|");
	if(TOKBAD)
		{ EGEN("missing opponent name"); }
	opp = tok;

	if(tok = strsep(gmsep, "|")) {			// start time is allowed to be anything except nullptr or too long. strlen of 0 is not specifically reported
		if(strlen(tok) != TIMESZ - 1)
			fl_alert("%s line %d: start time is the wrong length", GAMEFL, linenum);
	}
	else
		{ EGEN("end of line instead of start time"); }
	starttm = tok;

	if(tok = strsep(gmsep, "|")) {			// end time is allowed to be anything except nullptr or too long. strlen of 0 is a given for game in progress
		if(strlen(tok) >= TIMESZ)
			fl_alert("%s line %d: end time is too long", GAMEFL, linenum);
	}
	else
		{ EGEN("end of line instead of end time"); }
	endtm = tok;

	// rack, board mem ================================================================================
	if(tok = strsep(gmsep, "|"))
		parse_game_area("rack", tok, tbl->rckn, linenum, tbl);
	else
		{ EGEN("end of line instead of rack letters"); }
	rck = tok;

	if(tok = strsep(gmsep, "|"))
		parse_game_area("board", tok, tbl->brdn * tbl->brdn, linenum, tbl);
	else
		{ EGEN("end of line instead of board letters"); }
	brd = tok;

	tok = strsep(gmsep, "|");					// notes length obsoleted
	if(! tok)										// buffer included in ctor for Game
		{ EGEN("end of line instead of notes"); }
	notes = tok;

	games.push_back(new Game(stat, tbl, tblname, opp, starttm, endtm, brd, rck, notes));
	linenum++;			// static within function
	return true;
	#undef TYLOFL
}
void App::parse_game_area(const char *objname, char *tok, int objlen, int linenum, Tbl *tbl) {

	// still parsing game file. "game area" refers to board or rack collectively

	int toklen = strlen(tok);

	if(toklen < objlen)
		fl_alert("%s line %d: missing %s letter(s).", GAMEFL, linenum, objname);
	else if(toklen > objlen)
		fl_alert("%s line %d: extra %s letters will be ignored.", GAMEFL, linenum, objname);	// let it slide

	int errs = 0;
	for(int j = 0; j < objlen; j++) {
		if(tok[j] != FLLRCHR)
			if(! ty::srch_keytab(tbl->alphas, tbl->alphan, tok[j])) {
				tok[j] = FLLRCHR;
				errs++;
			}
	}
	if(errs)
		fl_alert("%s line %d: %s: %d invalid letters were replaced by space.", GAMEFL, linenum, objname, errs);
}
void App::game_new(cc *tblnm) {

	int j;

	cout << "new " << tblnm << endl;

	for(j = 0; j < tbls.size(); j++) {												// tried iterator, did not like b/c itr itself is a pointer
		if(! strcmp(tbls[j]->name, tblnm)) {										// found the tbl ptr
			char *opp;
			if(opp = ty::getopp("Please enter the opponent's name.")) {		// should return ptr to heap or nullptr
				Game *gm = new Game(tbls[j], opp);
				games.push_back(gm);
				setmod(true);
				ngames++;
				nactv++;
				brwsr->add_entry(gm);				// load() will set app->cur*
				brwsr->sort(NULL, gm);
				gm->load(false);
			}
			break;
		}
	}
	if(j == tbls.size())
		fl_alert("table %s not found.", tblnm);	// should never happen
}
void App::setmod(bool b) {			// set app->ismod to true or false and change app title to reflect
	if(b)
		label("* " APPTTL " " APPVER " *");
	else
		label(APPTTL " " APPVER);
	ismod = b;
}
void App::save() {

	using std::fstream;

	if(ismod == false) return;			// remember ismod needs to refer to entire app
	curgame->unload();					// and curgame refers to 1 game
	ty::check_isreg(GAMEFL, true);	// can abort
	unlink(GAMEFL ".bak");
	if(rename(GAMEFL, GAMEFL ".bak"))
		fl_alert("Could not make a backup of " GAMEFL ".");
		// do you want to ...
	fstream fs(GAMEFL, fstream::out | fstream::trunc);

	if(! fs) {
		cout << "open game file for writing failed" << endl;
		return;
	}
	int j;
	for(j = 1; j <= brwsr->size(); j++) {			// save in brwsr order so that sorting during next init is unnecessary

		Game *gm = (Game *) brwsr->data(j);
		int ntslen = gm->buffy->length();
		if(ntslen > NTSMAX) {
			fl_alert("Truncating notes for %s %s at the maximum of %d characters", gm->opp, gm->starttm, NTSMAX);
			ntslen = NTSMAX;
		}
		string nts;											// buffer rquires a bit of extra handling
		nts.assign(gm->buffy->text(), ntslen);		// truncate to sane length
		for(char c : nts)
			if(c == '\n')									// convert atual newlines to symbolic newlines
				c = NTSNL;
		fs	<<				gm->stat 		// no delim
			<<				gm->tbl->name
			<<	'|' <<	gm->opp
			<<	'|' <<	gm->starttm
			<<	'|' <<	gm->endtm
			<<	'|' <<	string(gm->rck, gm->tbl->rckn)
			<<	'|' <<	string(gm->brd, gm->tbl->brdn * gm->tbl->brdn)
			<<	'|' <<	string(nts, ntslen)
			<<	'\n'
		;
	}
	fs.close();
	cout << j - 1 << " games written" << endl;
	setmod(false);
}
void App::write_opts() {

	using std::fstream;
	cout << "write opts" << endl;

	fstream optfs(OPTFL, fstream::out | fstream::trunc);

	if(! optfs) {
		fl_alert("Failed to open %s, could not save options.", OPTFL);
		return;
	}
	opts["lastgame"].sval = ((string) curgame->opp + brwsr->column_char() + curgame->starttm);

	for(optmap_t::iterator iter = opts.begin(); iter != opts.end(); iter++) {

		string key = iter->first;
		opt_s &val = opts[key];		// must use reference

		if(val.write) {
			optfs << key << '\t';
			if(val.type == 's')
				optfs << val.sval;
			else
				optfs << (val.ival >> (val.type == 'I') * 8);
			optfs << '\n';
		}
	}
	optfs.close();
}

void App::quit() {
	write_opts();
	hide();
}
void app_cb(Fl_Widget*, void*) {		// cannot be defined as member function else everything it refers to has to be either another static member
												// or a non appclass member
	cout << "app callback" << endl;

	if(! (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)) {	// ignore escape
		if(app->ismod) app->save();												// save without asking. the menu cb quit_mcb() does ask.
		app->quit();
	}
}
