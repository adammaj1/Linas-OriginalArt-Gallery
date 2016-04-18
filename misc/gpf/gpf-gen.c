/*
 * Generating functions for greatest prime factors.
 *
 * April 2016
 */

#include <math.h>
#include <stdio.h>

#include <gpf.h>

/*
 * Ordinary generating function for the greatest common factor.
 */
double gpf_ordinary(double x)
{
	double sum = 0;
	double xn = x;

	if (x < 1.0e-16) return x;

	for (int n=1; ; n++)
	{
		sum += gpf(n) * xn;
		xn *= x;
		if (n*xn < 1.0e-16*sum) break;
	}

	return sum;
}

/*
 * Exponential generating function for the greatest common factor.
 */
double gpf_exponential(double x)
{
	double sum = 0;
	double xn = x;

	if (x < 1.0e-16) return x;

	for (int n=1; ; n++)
	{
		sum += gpf(n) * xn;
		xn *= x / ((double) n);

		if (n*xn < 1.0e-16*sum) break;
	}

	return sum;
}

int main()
{
#ifdef ORD
	for (double x=0.0; x< 1.0; x+= 0.002)
	{
		double y = gpf_ordinary(x);
		double z = gpf_exponential(x);
		printf("%g\t%g\t%g\n", x, y, z);
	}
#endif
	for (double x=0.0; x< 675.0; x+= 0.5)
	{
		double y = gpf_exponential(x);
		double z = y * exp(-x);
		printf("%g\t%g\t%g\n", x, y, z);
		fflush(stdout);
	}
}
