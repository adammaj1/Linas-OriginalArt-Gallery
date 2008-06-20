/*
 * Compute matrix elements of fourier of minkowski question mark 
 * by means of brute-force integration. This seems to be a viable
 * quick-n-dirty way of getting these.
 *
 * Linas June 2008
 */

#define M_PIl    3.1415926535897932384626433832795029L  /* pi */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gral-simple.h"

static long double delta;
static int npts;

void set_npts(int n)
{
	npts = n;
	delta = 1.0L / ((long double) n);
}

void make_g_plain_elt(int m, int n, long double *pre, long double *pim)
{
	long double x;
	long double re = 0.0L;
	long double im = 0.0L;
	int i;

	for (i=0; i<npts; i++)
	{
		x = ((long double) i)*delta;
		x = n*x/(1.0L+x) - m*x;
		x *= 2.0L * M_PIl;
		re += cosl(x);
		im += sinl(x);
	}

	re *= delta;
	im *= delta;

	*pre = re;
	*pim = im;
}

void make_full_elt(int m, int n, long double *pre, long double *pim)
{
	long double x;
	long double re = 0.0L;
	long double im = 0.0L;
	int i;

	for (i=0; i<npts; i++)
	{
		x = ((long double) i)*delta;
		x = n/(2.0L-x) - m*x;
		x *= 2.0L * M_PIl;
		re += cosl(x);
		im += sinl(x);

		x = ((long double) i)*delta;
		x = n*(1.0L-x)/(2.0L-x) - m*x;
		x *= 2.0L * M_PIl;
		re -= cosl(x);
		im -= sinl(x);
	}

	re *= delta;
	im *= delta;

	*pre = re;
	*pim = im;
}

