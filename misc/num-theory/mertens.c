
/*
 * mertens.c
 *
 * Graph of merten function.
 * Ordinary x-y plot; output is ascii list of x-y values
 *
 * Linas Vepstas July 2006
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "moebius.h"

int mertens (int n)
{
	int acc = 0.0;

	int i=1;
	for (i=1; i<=n; i++)
	{
		acc += moebius_mu (i);
	}

	return acc;
}

int main ()
{
	int i;

	int nmax = 10000000;
	
	int mert = 0;
	int scale = 1;
	for (i=1; i<nmax; i++)
	{
		// mert = mertens (i);
		mert += moebius_mu(i);

		if (0 == i%scale) 
		{
			printf ("%d	%d\n", i, mert);
			fflush (stdout);
			if (i > scale*300) scale *= 2;
		}
	}
}
