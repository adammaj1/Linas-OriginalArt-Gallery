
/*
 * von_mangoldt.c
 *
 * Graph of Chebyshev (sum of von Mangoldt) function.
 * Ordinary x-y plot; output is ascii list of x-y values
 *
 * Linas Vepstas July 2006
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "moebius.h"

long double chebyshev (int n)
{
	long double acc = 0.0;

	int i=1;
	for (i=1; i<=n; i++)
	{
		acc += mangoldt_lambda (i);
	}

	return acc;
}

int main ()
{
	int i;

	int nmax = 1000000;
	
	long double sum = 0.0;
	long double mert = 0.0;
	long double smooth = 0.0;
	int scale = 100;

	long double offset = log(M_PI);
	// long double offset = 0.0;
	for (i=1; i<nmax; i++)
	{
		// sum = chebyshev (i);
		sum += mangoldt_lambda(i);
		smooth += sum;
		// mert += moebius_mu(i);

		if (0 == i%scale) 
		{
			long double lead = sum - i+offset;
			long double sec = i;
			sec = smooth-0.5L*sec*sec;
			// long double lead = sum/mert;
			printf ("%d	%24.18Lg\t%24.18Lg\n", i, lead, sec);
			fflush (stdout);
			// if (i > scale*300) scale *= 2;
		}
	}
}
