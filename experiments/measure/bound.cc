
/*
 * A self-bound for recursive expression for the Minkowski measure
 * This is the same old question mark, just a different API.
 * Specifically, since the thing is nicely, recursively defined, its more
 * tractable.
 *
 * Linas Vepstas September 2015
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "question.h"

void print_levs(int lev, int i)
{
	unsigned __int128 p, q, pm, qm;

	// Golden ratio
	double phi = 0.5 * (1.0 + sqrt(5.0));

#define NUML 10
	for (int ill = 0; ill <= NUML; ill++)
	{
		int level = lev - NUML + ill;

		// Growth rate
		double grow = pow(0.5 * phi *phi, level) * 0.2 * phi * phi * phi;
		double norm = 1.0 / (double)(1<<level);

		int j = i >> (NUML-ill);
		int k = j;

#if 0
		stern_brocot_tree(j, level, pmid, qmid);
		double low_end = ((double) pmid) / (double) qmid;
		if (y < low_end) { k = j-1; printf ("duuude down\n"); exit(1); }

		stern_brocot_tree(j+1, level, pmid, qmid);
		double hi_end = ((double) pmid) / (double) qmid;
		if (hi_end < y) { k = j+1; printf ("duuude up\n"); exit(1); }
#endif

		stern_brocot_tree128(k, level, p, q);
		stern_brocot_tree128(k+1, level, pm, qm);
		double delta = norm * q * qm;
		delta /= grow;

		printf("%g\t", delta);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s level\n", argv[0]);
		exit(1);
	}

	int lev = atoi(argv[1]);

	unsigned __int128 pmid, qmid;

	for (int i=0; i< (1<<lev); i++)
	{
		stern_brocot_tree128(i, lev, pmid, qmid);
		double y = ((double) pmid) / (double) qmid;
		printf("%d\t%g\t", i, y);
		print_levs(lev, i);
		printf("\n");

		stern_brocot_tree128(i+1, lev, pmid, qmid);
		y = ((double) pmid) / (double) qmid;
		printf("%d\t%g\t", i+1, y);
		print_levs(lev, i);
		printf("\n");
	}
}
