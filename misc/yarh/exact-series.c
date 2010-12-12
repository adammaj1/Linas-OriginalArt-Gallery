/*
 * exact-series.c
 *
 * Solution via series
 *
 * Linas Vepstas December 2010
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "binomial.h"

/* Integral of (x^(s-1))/(x+alpha) dx
 */
long double complex eff (long double complex s, long double alpha, long double x)
{
	int k;
	long double complex term, sum;
	long double opxa = 1.0L + x/alpha;
	long double opxak = opxa;

	sum = 0.0L;

	for (k=1; k<155456123; k++)
	{
		term = cbinomial(s,k);
		// printf("k=%d bino =%g +I %g\n", k, creal(term), cimag(term));
		term *= opxak;
		term /= (long double) k;
		if (k%2)
		{
			sum -= term;
		}
		else
		{
			sum += term;
		}

		// printf("k=%d term=%g +I %g\n", k, creal(term), cimag(term));
		double tm = cabs(term);
		if (tm < 1.0e-20) break;

		opxak *= opxa;
	}

	long double xa = x+alpha;
	if (xa <= 0.0L)
	{
		fprintf(stderr, "Error: unexpected sign for x+a=%Lg\n", xa);
		exit(1);
	}
	term = logl(-xa);
	sum += term;

	term = s * logl(-alpha);
	term = cexpl(term);

	sum *= term;

	return sum;
}

/* Integral of (x^s) dx = (x^(s+1))/(s+1)
 */
long double complex effo (long double complex s, long double x)
{
	long double complex term = logl(x);
	term *= (s+1.0L);
	term = cexpl(term);
	term /= (s+1.0L);

	return term;
}

/* Integral of (x^(s-1))/(x+alpha) dx
 * however, converges differently
 */
long double complex special_eff (long double complex s, long double alpha, long double x)
{
	int n;
	long double complex term, sum;
	long double rat, nrat;

	rat = alpha / x;
	nrat =1.0L;

	sum = 0.0L;
	for (n=0; n<57123456; n++)
	{
		term = nrat / (s-1.0L - (long double) n);
		sum += term;

		double tm = cabs(term);
		if (tm < 1.0e-20) break;

		nrat *= rat;
	}

	term = logl(x);
	term *= (s-1.0L);
	term = cexpl(term);
	sum *= term;

	return sum;
}

/* Integral of s_12 */

long double complex gral_s12(long double complex s, unsigned int a1max, unsigned int a2max)
{
	unsigned int na1, na2;
	long double complex term;
	long double complex sum = 0.0L;
	
	for (na1=1; na1<a1max; na1++)
	{
		for (na2=1; na2<a2max; na2++)
		{
			long double a1 = na1;
			long double a2 = na2;
			long double xlo = a2 / (1.0L + a1 * a2);
			long double xhi = (1.0L + a2) / (1.0L + a1 + a1 * a2);

			long double b = a1 - a2;
			long double c = - (a1 * a2 + 1.0L) * b;
			long double d = 1.0L + a2 * b;
			long double a = 1.0L - a1 * b;
			long double greb = d / c;

			if ((1 == na1) && (2 == na2))
			{
				printf("special case \n");
				term = special_eff(s, greb, xhi);
				sum += term;

				term = special_eff(s, greb, xlo);
				sum -= term;
			}

			/* special case, where c=d=0, so general formula does not apply */
			else if (na1 == na2)
			{
				// printf("duude c=0 for a1=%d\n", na1);

				term = effo(s, xhi);
				sum += term;

				term = effo(s, xlo);
				sum -= term;
			}

			// General case
			else
			{
				term = eff(s, greb, xhi);
				sum += term;

				term = eff(s, greb, xlo);
				sum -= term;
			}
		}
	}
	return sum;
}

int main (int argc, char * argv[])
{
	int amax = atoi(argv[1]);

	long double complex ess = 0.5 + I*12.0;

	long double complex ans = gral_s12(ess, amax, amax);
printf("ans= %g %g\n\n", creal(ans), cimag(ans));

	return 0;
}