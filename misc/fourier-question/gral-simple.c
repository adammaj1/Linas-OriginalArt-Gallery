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

static long double delta;
static int npts;

void make_elt(int m, int n, long double *pre, long double *pim)
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

void fill_matrix(int sz)
{
	int i, j;

	for (i=-sz; i<=sz; i++)
	{
		for (j=-sz; j<=sz; j++)
		{
			long double re = 0.0L;
			long double im = 0.0L;

			make_elt(i,j, &re, &im);
			printf("W[%d, %d] = %Lg +i %Lg\n", i, j, re, im);
		}
		printf ("\n");
	}
}

void slice_row(int row, int colmin, int colmax)
{
	int j;

	for (j=colmin; j<colmax; j++)
	{
		long double re = 0.0L;
		long double im = 0.0L;

		make_elt(row, j, &re, &im);
		printf("%d	%d	%Lg	%Lg\n", row, j, re, im);
	}
}

void slice_col(int rowmin, int rowmax, int col)
{
	int i;

	for (i=rowmin; i<rowmax; i++)
	{
		long double re = 0.0L;
		long double im = 0.0L;

		make_elt(i, col, &re, &im);
		printf("%d	%d	%Lg	%Lg\n", i, col, re, im);
	}
}

main(int argc, char * argv[])
{
	npts = 123123;
	delta = 1.0L / (long double) npts;

	int row = atoi(argv[1]);
	slice_row(row, -20, 360);
	//
	int col = row;
	// slice_col(-20, 160, col);
}
