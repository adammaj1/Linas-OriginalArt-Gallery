
/*
 * baez.c
 *
 * Baez-Duarte sum
 *
 * Linas Vepstas, February 2006
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_sf_gamma.h>

#include "binomial.h"
#include "harmonic.h"

void simple_integrand (double t, int n, double *reg, double * img)
{
	gsl_sf_result lnr, arg;

	double regr = 0.0;
	double imgr = 0.0;

	double res = 0.5;
	double ims = t;

	/* Gamma (s) */
	gsl_sf_lngamma_complex_e (res, ims, &lnr, &arg);
	regr += lnr.val;
	imgr += arg.val;

	/* Gamma (s+n+1) */
	gsl_sf_lngamma_complex_e (res+n+1.0, ims, &lnr, &arg);
	regr -= lnr.val;
	imgr -= arg.val;

	/* zeta (2s+2) */
	double rez, imz;
	riemann_zeta (2.0*res+2.0, 2.0*ims, &rez, &imz);
	double r = rez*rez + imz*imz;	

	regr -= 0.5 * log (r);
	imgr -= atan2 (imz, rez);

	*reg = regr;
	*img = imgr;
}

void reflect_integrand (double t, int n, double *reg, double * img)
{
	gsl_sf_result lnr, arg;

	double regr = 0.0;
	double imgr = 0.0;

	double res = 0.5;
	double ims = t;

	/* Gamma (s) */
	gsl_sf_lngamma_complex_e (res, ims, &lnr, &arg);
	regr += lnr.val;
	imgr += arg.val;

	/* Gamma (s+n+1) */
	gsl_sf_lngamma_complex_e (res+n+1.0, ims, &lnr, &arg);
	regr -= lnr.val;
	imgr -= arg.val;

	/* Gamma (2*s+1) */
	gsl_sf_lngamma_complex_e (2.0*res+1.0, 2.0*ims, &lnr, &arg);
	regr -= lnr.val;
	imgr -= arg.val;

	/* (2pi)^(2s-1) */
	double lp = log (2.0*M_PI);
	regr += (2.0*res-1.0)*lp;
	imgr += 2.0*ims*lp;

	/* sin (pi s) */
	double r = exp (-M_PI*ims);
	double rr = 1.0/r;
	double x = (r - rr) * cos (M_PI*res);
	double y = (r + rr) * sin (M_PI*res);

	double theta = atan2 (y, x);
	r = 0.5 * log (x*x+y*y);

	regr -= r;
	imgr -= theta;

	regr += M_LN2;

	/* zeta (2s+1) */
	double rez, imz;
	riemann_zeta (2.0*res+1.0, 2.0*ims, &rez, &imz);
	r = rez*rez + imz*imz;	

	regr -= 0.5 * log (r);
	imgr -= atan2 (imz, rez);

	*reg = regr;
	*img = imgr;
}

double
integrate (int n)
{
	double t=0.0;

	double reacc = 0.0;
	double imacc = 0.0;

	double step = 0.03;
	double lim = 6.0;
	for (t=-lim; t<=lim; t+=step)
	{
		double reg, img;
		simple_integrand (t, n, &reg, &img);

		double r = exp (reg);
		reacc += r * cos (img);
		imacc += r * sin (img);

		// printf ("%g\t%g\t%g\n", t, step*reacc, step*imacc);
	}

	imacc *= step;
	imacc *= factorial (n);
	imacc /= 4.0*M_PI;

	return imacc;
}

double sum (int n)
{
	int k;
	double acc = 0.0;

	double sgn = 1.0;
	for (k=0; k<=n; k++)
	{
		double b = binomial (n,k);
		double z = zetam1 (2*k+2);
		z = 1.0 / (1.0+z);
		acc += sgn*b*z;
		sgn = -sgn;
	}

	return acc;
}

int
main (int argc, char * argv[])
{
	int n=3;

	n = atoi (argv[1]);

	double in = integrate (n);
	double su = sum (n);

	printf ("integ=%g sum=%g r = %g\n", in, su, su/in);

	return 0;
}
