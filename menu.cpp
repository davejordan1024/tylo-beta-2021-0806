#include "menu.h"

Menu::Menu(int x, int y, int w, int h, vector<Tbl*>tbls, optmap_t) : Fl_Menu_Bar(x, y, w, h) {

	for(Tbl *tbl : tbls)
		add(((string) "&Game/&new/" + tbl->name).c_str(), 0, new_mcb, this);

	add("Game/save",					FL_CTRL +			's',	save_mcb,	this);
	add("Game/rematch",				FL_CTRL +			'm',	rematch_mcb,this);
	add("Game/clear",					FL_CTRL +			'0',	clear_mcb,	this);
	add("Game/over",					FL_CTRL +			'o',	toggle_mcb,	this);
	add("Game/rename",				FL_CTRL +			'n',	rename_mcb,	this);
	add("Game/delete",				FL_CTRL +			'd',	del_mcb, 	this);
	add("Game/quit app",				FL_CTRL + 			'q',	quit_mcb,	this);
	add("&Edit/copy rack",			FL_CTRL + 			'c',	copy_mcb,	this);	// shared callback
	add("Edit/copy bag",				FL_CTRL + 			'b',	copy_mcb,	this);
	add("Edit/justify rack",		FL_CTRL + FL_End,			squish_mcb,	this);
	add("&Sort/by name",				0,								sort_mcb,	this);	// shared
	add("Sort/by start",				0,								sort_mcb,	this);
	add("Sort/by end",				0,								sort_mcb,	this);
	add("Sort/by status by name",	0,								sort_mcb,	this);
	add("&Help/help",					0,								thelp_mcb,	this);
	add("Help/custom",				FL_CTRL +			'i',	uhelp_mcb,	this);

	textcolor(FL_WHITE);
	color(0x2e3436 << 8);
	box(FL_FLAT_BOX);
}

void Menu::new_mcb		(MCB_ARGS)	{	app->game_new(((Menu *)w)->text()); }
void Menu::save_mcb		(MCB_ARGS)	{	app->save(); }
void Menu::rematch_mcb	(MCB_ARGS)	{	app->curgame->rematch(); }
void Menu::clear_mcb		(MCB_ARGS)	{	app->curgame->clear(); }
void Menu::toggle_mcb	(MCB_ARGS)	{	app->curgame->toggle(); }
void Menu::rename_mcb	(MCB_ARGS)	{	app->curgame->rename(); }
void Menu::del_mcb		(MCB_ARGS)	{	app->curgame->del(); }
void Menu::squish_mcb	(MCB_ARGS)	{	ty::squish(); }
void Menu::copy_mcb		(MCB_ARGS)	{	ty::copyfrom(((Menu *)w)->text()); }
void Menu::thelp_mcb		(MCB_ARGS)	{	ty::tylohelp(); }
void Menu::uhelp_mcb		(MCB_ARGS)	{	ty::userhelp(); }
void Menu::sort_mcb		(MCB_ARGS)	{	app->brwsr->sort(((Menu *)w)->text(), NULL); }

void Menu::quit_mcb		(MCB_ARGS)	{

	if(app->ismod) {
		switch(fl_choice("You are about to exit. Do you want to save first?", "No", "Yes", "Don't exit")) {
			case 1: app->save();	// fallthru
			case 0: app->quit();	// quit() saves options
		}
	}
	else
		if(0 == fl_choice("You are about to exit. Games are already saved.", "Okay", "Don't exit", 0))
			app->quit();
	return;		// dont exit
}
