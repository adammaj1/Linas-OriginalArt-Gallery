/*
 * Generating functions for greatest prime factors.
 *
 * April 2016
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gpf.h>
#include "gpf-gen-bignum.h"

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

double gpf_bignum_exponential(double x, double theta)
{
	cpx_t sum, z;
	cpx_init(sum);
	cpx_init(z);

	theta *= 2.0 * M_PI;
	cpx_set_d(z, x*cos(theta), x*sin(theta));

	cpx_gpf_exponential(sum, z, 20);

	mpf_t val;
	mpf_init(val);
	cpx_abs(val, sum);
	double rv = mpf_get_d(val);
	return rv;
}

double complex gpf_cpx_bignum_exponential(double x, double theta)
{
	cpx_t sum, z;
	cpx_init(sum);
	cpx_init(z);

	theta *= 2.0 * M_PI;
	cpx_set_d(z, x*cos(theta), x*sin(theta));

	cpx_gpf_exponential(sum, z, 50);

	double complex rv = cpx_get_re(sum) + I * cpx_get_im(sum);
	return rv;
}

int main(int argc, char* argv[])
{
#ifdef ORD
	for (double x=0.0; x< 1.0; x+= 0.002)
	{
		double y = gpf_ordinary(x);
		double z = gpf_exponential(x);
		printf("%g\t%g\t%g\n", x, y, z);
	}
#endif
#ifdef EXPO
	for (double x=0.0; x< 675.0; x+= 0.5)
	{
		double y = gpf_exponential(x);
		double z = y * exp(-x);
		printf("%g\t%g\t%g\n", x, y, z);
		fflush(stdout);
	}
#endif
#ifdef RATIONALS
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <r>\n", argv[0]);
		exit(1);
	}
	double dom = atof(argv[1]);
	printf("#\n# Max = %g\n#\n", dom);
	for (double x=0.0; x< dom; x+= 0.001*dom)
	{
		double w0 = gpf_bignum_exponential(x, 0.0);
		double w1_2 = gpf_bignum_exponential(x, 1.0/2.0);
		double w1_3 = gpf_bignum_exponential(x, 1.0/3.0);
		double w1_4 = gpf_bignum_exponential(x, 1.0/4.0);
		double w1_5 = gpf_bignum_exponential(x, 1.0/5.0);
		double w1_6 = gpf_bignum_exponential(x, 1.0/6.0);
		printf("%g\t%g\t%g\t%g\t%g\t%g\t%g\n", x, w0, w1_2, w1_3, w1_4, w1_5, w1_6);
		fflush(stdout);
	}
#endif
#ifdef QUADRATIC_IRRATIONALS
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <r>\n", argv[0]);
		exit(1);
	}
	double dom = atof(argv[1]);
	printf("#\n# Max = %g\n#\n", dom);
	for (double x=0.0; x< dom; x+= 0.0002*dom)
	{
		double complex w0   = gpf_cpx_bignum_exponential(x, sqrt(2.0)/2.0);
		double complex w1_2 = gpf_cpx_bignum_exponential(x, sqrt(2.0)/3.0);
		double complex w1_3 = gpf_cpx_bignum_exponential(x, sqrt(3.0)/2.0);
		double complex w1_4 = gpf_cpx_bignum_exponential(x, sqrt(3.0)/3.0);
		double complex w1_5 = gpf_cpx_bignum_exponential(x, sqrt(5.0)/2.0);
		double complex w1_6 = gpf_cpx_bignum_exponential(x, sqrt(5.0)/3.0);
		printf("%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", x,
			creal(w0), cimag(w0),
			creal(w1_2), cimag(w1_2),
			creal(w1_3), cimag(w1_3),
			creal(w1_4), cimag(w1_4),
			creal(w1_5), cimag(w1_5),
			creal(w1_6), cimag(w1_6));
		fflush(stdout);
	}
#endif

#ifdef TRANSCENDY
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <r>\n", argv[0]);
		exit(1);
	}

	double phi = 0.5 * sqrt(5.0) - 0.5;
	double dom = atof(argv[1]);
	printf("#\n# Max = %g\n#\n", dom);
	for (double x=0.0; x< dom; x+= 0.0002*dom)
	{
		double tp = 2.0 * M_PI;
		double complex w0   = gpf_cpx_bignum_exponential(x, 1.0/tp);
		double complex w1_2 = gpf_cpx_bignum_exponential(x, (1.0/3.0)/tp);
		double complex w1_3 = gpf_cpx_bignum_exponential(x, (2.0/3.0)/tp);
		double complex w1_4 = gpf_cpx_bignum_exponential(x, 0.75/tp);
		double complex w1_5 = gpf_cpx_bignum_exponential(x, phi/tp);
		double complex w1_6 = gpf_cpx_bignum_exponential(x, (5.0/7.0)/tp);
		printf("%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", x,
			creal(w0), cimag(w0),
			creal(w1_2), cimag(w1_2),
			creal(w1_3), cimag(w1_3),
			creal(w1_4), cimag(w1_4),
			creal(w1_5), cimag(w1_5),
			creal(w1_6), cimag(w1_6));
		fflush(stdout);
	}
#endif

	// for (double r=0.87; r< 0.89; r+= 0.001*0.02)
	// for (double r=3.23; r< 3.25; r+= 0.001*0.02)
	// for (double r=3.74; r< 3.75; r+= 0.001*0.02)
	// for (double r=19.95; r< 19.96; r+= 0.001*0.02)
	for (double t=0.83; t< 0.84; t+= 0.001*0.02)
	{
		// double w = gpf_bignum_exponential(r, 0.5 * 0.83295289206477);
		// double w = gpf_bignum_exponential(r, 0.5 * 0.42458721923649);
		// double w = gpf_bignum_exponential(r, 0.5 * 0.59817818048564);
		// double w = gpf_bignum_exponential(r, 0.5 * 0.768238424116);
		// printf("%g\t%g\n", r, w);
		double w = gpf_bignum_exponential(0.88022349562601, 0.5*t);
		printf("%g\t%g\n", t, w);
		fflush(stdout);
	}
}
