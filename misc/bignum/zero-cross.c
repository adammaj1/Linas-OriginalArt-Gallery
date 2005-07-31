
/*
 * zero-cross.c
 *
 * Get zero crossing fits for the data.
 *
 * The result of this is that the zero-crossing second derive appears to
 * be exactly pi/2.  So we're going now.
 *
 * Linas July 2005
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

main ()
{
	int c;
	char buf[4000];
	double var[4000];

	/* read in floating point values in the first column */
	int disc = 1;	
	int i =0;
	int n=0;
	while( (c=getchar()) != EOF)
	{
		if (disc && c != '\t') continue;
		disc = 0;
		if (c == '\t') continue;

		buf[i] = c;

		if ( c == '\n')
		{
			buf[i] = 0;
			disc = 1;
		   i=-1;	

			var[n] = atof(buf);
			// printf ("%d	%g\n", n, var[n]);
			n++;
		}
		i++;
	}

	/* now interpolate */
	double cross[100];
	int nmax = n;
	int icross = 0;

	double prev = var[0];
	n=1;
	while (n < nmax)
	{
		double hur = var[n];
		if (prev*hur <= 0.0)
		{
			// printf ("%d %d %g  %g\n", icross, n, prev, hur);

			double x = hur - prev;
			x = prev / x;
			x = fabs (x);
			x += n-1;
			double guess = 0.25*M_PI*(icross)*(icross);
			guess += 2.0 + (2.553+0.25*M_PI) * icross;
			printf ("crossings -- %d %g\t guess=%g\t err=%g\n", icross, x, guess, x-guess);
			cross[icross] = x;
			icross ++;
		}
		prev = hur;
		n++;
	} 

	/* now take diffs */
	int icmax = icross;
	for (i=1; i<icmax; i++)
	{
		double d = cross[i] - cross[i-1];
		double guess = 0.5*M_PI*i;
		guess += 2.553;
		printf ("first-order differences -- %d	%g\t\t guess=%g\t err=%g\n", i, d, guess, d-guess);
	}


	double avg = 0.0;
	int acnt = 0;
	for (i=1; i<icmax-1; i++)
	{
		double d = 2.0 * cross[i] - cross[i-1] - cross[i+1];
		printf ("second-order differences -- %d	%g\n", i, d);
		avg += d;
		acnt ++;
	}

	avg /= acnt;
	printf ("average value = %g\n", avg);
}
