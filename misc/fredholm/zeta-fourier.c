
/*
 * zeta-fourier.c
 *
 * Fourier transforms of zeta-function-zero based propator.
 *
 * Linas September 2008
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double *zeros;

/**
 * read in the zeros of the zeta function from a file.
 */
void read_zeros (int nzeros)
{
	int i;
	zeros = (double *) malloc (nzeros * sizeof(double));
	FILE * fh = fopen ("zeros1", "r");
	for (i=0; i<nzeros; i++)
	{
		char buff[100];
		fgets(buff, 100, fh);
		zeros[i] = atof(buff);
	}
	fclose (fh);
}

/**
 * Compute sum 1/(rho+n^2)
 */
void coeffs(int nzeros, int nmax)
{
	int i, n;

	double *b = (double *) malloc (nmax * sizeof(double));
	for (n = 0; n<nmax; n++)
	{
		b[n] = 0.0;
		for (i = 0; i<nzeros; i++)
		{
			b[n] += 1.0 / (n*n - zeros[i]*zeros[i]);
			// b[n] += 1.0 / (n*n - zeros[i]*zeros[i]/ ((double)n*n));
		}

		b[n] *= 2.0*n;
		// b[n] *= 2.0;

#if 1
		printf ("%d\t%8.6g\n", n, b[n]);
#endif
	}

#if 0
	int imax = 1200;
	for (i=0; i<imax; i++)
	{
		double x = ((double) i) / ((double) imax);
		double re = 0.0;
		double im = 0.0;
		for (n=0; n<nmax; n++)
		{
			re += b[n] * cos(2.0*M_PI*n*x);
			im += b[n] * sin(2.0*M_PI*n*x);
		}
		printf ("%8.6g\t%8.6g\t%8.6g\n", x, re, im);
	}
#endif
}


int main(int argc, char * argv[])
{

	if (argc <3)
	{
		fprintf (stderr, "Usage: %s <nzeros> <ncoeffs\n", argv[0]);
		exit(1);
	}

	int nzeros = atoi(argv[1]);
	int ncoeffs = atoi(argv[2]);

	read_zeros(nzeros);
	coeffs(nzeros, ncoeffs);

	return 0;
}
