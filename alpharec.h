#ifndef ALPHA_H
#define ALPHA_H
#include "glbl.h"		// int8_t

struct Alpharec {
	int kystrk;
	char	uirep[5], clprep[5], snr;
	int8_t row, pv, gvn, rem;
	Alpharec();
};

#endif // ALPHA_H
