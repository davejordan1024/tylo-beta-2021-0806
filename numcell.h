#ifndef NUMCELL_H				// label for this class is arbitrary text followed by a mathematically manipulatable integer represented as text.
#define NUMCELL_H				// It bypasses the several problems using an output box, the worst of which is that you cant set the alignment
									// or the font of its value(). Also there is no output box that uses int internally, only double or nonnumeric.
									// total max will equal this #define + space + next #define + nul.

#define NCTXTMAX 27			// compiler complains if number of digitd isn't 4
#define NCDGTMAX 4

struct Numcell : Fl_Box {

	int8_t val;
	char *valptr, fulllbl[NCTXTMAX + NCDGTMAX + 2] = { 0 };

	Numcell(int x, int y, int w, int h, cc *lbl, int8_t v, optmap_t);
	void		text(cc*, int8_t);
	int8_t	value(int8_t);
	int8_t	value();
	int8_t	incr(int8_t);
};

#endif // NUMCELL_H
