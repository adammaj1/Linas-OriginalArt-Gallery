
/*
 * series.c
 *
 * Graphs of maclaurin series of totient and other number-theoretic
 * arithmetic series.  These are ordinary x-y plots; output is 
 * ascii list of x-y values
 *
 * Linas Vepstas December 2004
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "gcf.h"
#include "modular.h"
#include "moebius.h"
#include "totient.h"

long double totient_series (long double x)
{
	long double acc = 0.0;

	long double xp = 1.0;
	int n=1;
	while (1)
	{
		long double term = xp * totient_phi (n);
		acc += term;

		if (term < 1.0e-20*acc) break;
		xp *= x;
		n++;
	}

	return acc;
}

// return the limit as the totient sum goes to x-> 1
void limit (void)
{
	long double p = 0.5L;
	long double prev = 0.0;
	int i=1;
	while (1)
	{
		long double x = 1.0L - p;

		long double y = totient_series (x);
		y *= p*p;

		long double guess = y + (y-prev)/3.0L;
		printf ("%d	%Lg	%26.18Lg	%26.18Lg\n", i, x, y,  guess);

		p *= 0.5L;
		i++;
		prev = y;
	}
}

long double divisor_series (long double x)
{
	long double acc = 0.0;

	long double xp = 1.0;
	int n=1;
	while (1)
	{
		long double term = xp * divisor (n);
printf ("duude n=%d div=%d\n", n, divisor(n));
		acc += term;

		if (term < 1.0e-20*acc) break;
		xp *= x;
		n++;
	}

	return acc;
}

long double c_divisor_series (long double x)
{
	long double complex acc = 0.0;

	long double complex xi = x*I;
	long double complex xp = x*I;
	int n=1;
	while (1)
	{
		long double complex term = xp * divisor (n);
		acc += term;

		if (cabsl(term) < 1.0e-20*cabsl(acc)) break;
		xp *= xi;
		n++;
	}

	return cabsl (acc);
}

long double c_erdos_series (long double x)
{
	long double complex acc = 0.0;

	long double complex xi = x*I;
	long double complex xp = x*I;
	while (1)
	{
		long double complex term = xp / (1.0L-xp);
		acc += term;

		if (cabsl(term) < 1.0e-20*cabsl(acc)) break;
		xp *= xi;
	}

	return cabsl(acc);
}

long double erdos_series (long double x)
{
	long double acc = 0.0;

	long double xp = x;
	while (1)
	{
		// long double term = xp / (1.0L-xp);
		long double term = 1.0L/(xp * (xp-1.0L));
		acc += term;

		if (term < 1.0e-20*acc) break;
		xp *= x;
	}

	return acc;
}

long double z_erdos_series (long double x)
{
	long double acc = 0.0;

	long double tk = 0.5L;
	long double xp = x;
	while (1)
	{
		long double term = xp / (1-tk);
		acc += term;

		if (term < 1.0e-20*acc) break;
		xp *= x;
		tk *= 0.5L;
	}

	return acc;
}

long double moebius_series (long double x)
{
	long double acc = 0.0;

	long double xp = 1.0;
	int n=1;
	while (1)
	{
		long double term = xp * moebius_mu (n);
		acc += term;

		if (xp < 1.0e-18) break;
		xp *= x;
		n++;
	}

	return acc;
}

long double mangoldt_series (long double y)
{
	long double acc = 0.0;

	long double x = expl (-y);
	long double xp = x*x;
	int n=2;
	while (1)
	{
		long double term = xp * (mangoldt_lambda_cached(n) - 1.0);
		acc += term;
		// printf ("pnt=%d term=%Lg acc=%Lg\n", n, term, acc);

		if (fabs(term) < 1.0e-16*fabs(acc)) break;
		xp *= x;
		n++;
	}

	return -acc;
}

long double mangoldt_idx_series (long double y)
{
	long double acc = 0.0L;

	long double x = expl (-y);
	long double ox = x/(1.0L-x);
	// printf ("y=%Lg  x=%Lg ox=%Lg\n", y,x,ox);
	long double last_xp = 0.0;
	int n=1;
	unsigned int pnt;
	unsigned int last_pnt=2;
	while (1)
	{
		pnt = mangoldt_lambda_index_point (n);
		long double xp = expl (-y*pnt);
		long double term = mangoldt_lambda_indexed(n);
		term = xp * term;
		// printf ("index=%d pnt=%d term=%Lg acc=%Lg\n", n, pnt, term, acc);
		acc += term;
		if (fabs(term) < 1.0e-19*fabs(acc)) break;

		if (2 == pnt)
		{
			acc -= xp;
			last_xp = xp;
		}
		else
		{
			// term = expl(-y*(pnt-last_pnt));
			term=1.0L;
			int i;
			for (i=0; i<pnt-last_pnt; i++)
			{
				term *= x;
			}
			term = 1.0L - term;
			term *= last_xp*ox;
			acc -= term;
			//printf ("---dex=%d last_xp=%Lg term=%Lg acc=%Lg\n", n, last_xp, term, acc);
			last_xp = xp;
		}
		last_pnt = pnt;
		n++;
	}
	fprintf (stderr, "last index=%d pnt=%d\n", n, pnt);

	return -acc;
}

int main ()
{
	int i;

	int nmax = 2410;

	long double tp = 0.5;
	// for (i=1; i<=nmax; i++)
	for (i=nmax; i>0; i--)
	{
		long double x = ((double) i)/((double) nmax);

// #define TOTIENT_SERIES
#ifdef TOTIENT_SERIES
		long double y = totient_series (x);
		y *= (1.0L-x)*(1.0L-x);
		long double z = 0.607927101 * sin (0.5*M_PI*x);

		long double r = 2.0L*(y/z - 1.0L);
		printf ("%d	%Lg	%26.18Lg	%26.18Lg	%26.18Lg\n", i, x, y, z,r);
#endif


#define DIVISOR_SERIES
#ifdef DIVISOR_SERIES
		x = 1.0L-tp;
		long double y = divisor_series (x);
		// y *= (1.0L-x)*(1.0L-x);
		y *= tp;

		printf ("%d	%Lg	%26.18Lg\n", i, x, y);
#endif

// #define C_DIVISOR_SERIES
#ifdef C_DIVISOR_SERIES
		long double y = c_divisor_series (x);
		long double z = c_erdos_series (x);

		printf ("%d	%Lg	%26.18Lg	%26.18Lg	%26.18Lg\n", i, x, y, z, y-z);
#endif

// #define ERDOS_SERIES
#ifdef ERDOS_SERIES
		long double y = z_erdos_series (x);
		y *= (1.0L-x);

		printf ("%d	%Lg	%26.18Lg\n", i, x, y);
#endif

// #define MOEBIUS_SERIES
#ifdef MOEBIUS_SERIES
		long double y = moebius_series (x);

		printf ("%d	%Lg	%26.18Lg\n", i, x, y);
		fflush (stdout);
#endif

// #define MANGOLDT_SERIES
#ifdef MANGOLDT_SERIES
		x *= 0.00002;
		// long double y = mangoldt_series (x);
		long double y = mangoldt_idx_series (x);
		// y -= 0.337877;
		// y += 0.898*x;

		printf ("%d	%Lg	%26.18Lg\n", i, x, y);
		fflush (stdout);
#endif


		tp *= 0.5L;
	}
}
