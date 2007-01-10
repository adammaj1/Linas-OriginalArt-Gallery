
/** 
 * polylog.c
 *
 * Implement Borwein-style polylogarithm.
 * Also implement the "periodic zeta" and 
 * the Hurwitz zeta function.
 *
 * As of 22 December 2006, seems to be fully functional
 * and correct, and passes tests. The range of convergence
 * is rather limited because of precision/rounding errors.
 *
 * Linas November 2006
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_zeta.h>

#include "binomial.h"
#include "cplex.h"
#include "bernoulli.h"
#include "harmonic.h"

/* ============================================================= */
/* 
 * bee_k() 
 * Return value of sum_{j=0}^k (n j) oz^j
 *
 * where (n j) is binomial coefficient 
 */
static cplex bee_k (int n, int k, cplex oz)
{
	int j;
	cplex pz = cplex_one();
	cplex acc = cplex_zero();

	for (j=0; j<=k; j++)
	{
		cplex term = pz;
		double bin = binomial (n,j);

		term = cplex_scale (bin, term);
		acc = cplex_add (acc, term);
		pz = cplex_mul(pz, oz);
	}

	return acc;
}

/* polylog_est() 
 * Return estimate of polylog, Li_s(z) for estimator n.
 *
 * Appears to work well. Suggest n=31 for most cases,
 * should return answers accurate to 1e-16
 */
static cplex polylog_est (cplex s, cplex z, int n)
{
	int k;

	/* oz = 1/z   whereas moz = -1/z */
	cplex oz = cplex_recip(z);
	cplex moz = cplex_neg(oz);

	/* ska = [z/(z-1)]^n */
	cplex ska = z;
	ska.re -= 1.0;
	ska = cplex_mul (z, cplex_recip(ska));
	ska = cplex_pow_ui (ska,n);
	
	cplex pz = z;
	cplex acc = cplex_zero();
	cplex cacc = cplex_zero();

	for (k=1; k<=n; k++)
	{
		/* The inverse integer power */
		cplex dir = cplex_scale (log(k), s);
		dir = cplex_exp (cplex_neg(dir));

		/* Put it together */
		cplex term = cplex_mul (pz, dir);
		acc = cplex_add (acc, term);

		pz = cplex_mul(pz, z);
	}

	for (k=n+1; k<=2*n; k++)
	{
		/* The inverse integer power */
		cplex dir = cplex_scale (log(k), s);
		dir = cplex_exp (cplex_neg(dir));

		/* Put it together */
		cplex term = cplex_mul (pz, dir);
		acc = cplex_add (acc, term);

		/* The coefficient c_k of the polynomial */
		cplex ck = bee_k (n,k-n-1,moz);
		term = cplex_mul (ck, term);
		cacc = cplex_add (cacc, term);

		pz = cplex_mul(pz, z);
	}

	cacc = cplex_mul (cacc, ska);
	acc = cplex_sub (acc, cacc);

	return acc;
}

/* ============================================================= */
int nt = 31;

/**
 * periodic_zeta -- Periodic zeta function 
 *
 * F(s,q) = sum_{n=1}^infty exp(2pi iqn)/ n^s
 *        = Li_s (exp(2pi iq))
 * where 
 * Li_s(z) is the polylogarithm
 *
 * Periodic zeta function is defined as F(s,q) by Tom Apostol, chapter 12
 *
 * Uses the polylog implementation for the calculation,
 * and so is capable of returning results for Re s < 1.
 */
cplex periodic_zeta (cplex s, double q)
{
	int nterms =nt;
	cplex z;

	if ((0.0>q) || (1.0<q))
	{
		q -= floor (q);
	}
	
	if ((1.0e-10 > q) || (1.0e-10 > 1.0-q))
	{
		// XXX should be more precise with the next order
		// q correction ... 
		riemann_zeta (s.re, s.im, &z.re, &z.im);
		return z;
	}
	else if (0.25 > q) 
	{
		/* use the duplication formula to get into convergent region */
		cplex sm = cplex_neg(s);
		sm.re += 1.0;
		cplex ts = cplex_exp(cplex_scale (log(2), sm));
		cplex bt = periodic_zeta (s, 2.0*q);
		bt = cplex_mul (bt, ts);

		cplex z = periodic_zeta (s, q+0.5);
		z = cplex_sub(bt, z);
		return z;

	}
	else if (0.75 < q) 
	{
		/* use the duplication formula to get into convergent region */
		cplex sm = cplex_neg(s);
		sm.re += 1.0;
		cplex ts = cplex_exp(cplex_scale (log(2), sm));
		cplex bt = periodic_zeta (s, 2.0*q-1.0);
		bt = cplex_mul (bt, ts);

		cplex z = periodic_zeta (s, q-0.5);
		z = cplex_sub(bt, z);
		return z;

	}
	else
	{
		/* Normal case; within the convergence region */
		double ph = 2.0*M_PI*q;
		z.re = cos (ph);
		z.im = sin (ph);
		return polylog_est (s, z, nterms);
	}
}

/* ============================================================= */
/**
 * periodic_zeta_sum -- return value of periodic zeta.
 *
 * Evaluation performed by brute-force sum, thus, 
 * only practical for Re s > 2, otherwise, it will take
 * too long to converge.
 */
cplex periodic_zeta_sum (cplex s, double q)
{
	cplex sum, term;

	sum = cplex_zero ();
	s = cplex_neg (s);

	double th = 2.0*M_PI*q;
	cplex eith;
	eith.re = cos (th);
	eith.im = sin (th);

	cplex einth;
	einth = eith;
	
	int n;
	for (n=1; n<1123456789; n++)
	{
		term = cplex_ui_pow (n, s);
		term = cplex_mul (term, einth);
		sum = cplex_add (sum, term);
		einth = cplex_mul (einth, eith);

		double sz = term.re*term.re + term.im*term.im;
		// if (1.0e-28 > sz) break;
		if (1.0e-16 > sz) break;
	}

	return sum;
}

/* ============================================================= */
/**
 * periodic_beta -- Periodic beta function 
 *
 * similar to periodic zeta, but with different normalization
 *
 * beta = 2 Gamma(s+1) (2\pi)^{-s} F(s,q)
 *
 * As of 22 December, seems to be passing the tests -- 
 * that is, it gives the Bernoulli polynomials for integer s,
 * with all the right scale factors and signs, etc. Yay!
 */
cplex periodic_beta (cplex s, double q)
{
	static double log_two_pi = 0.0;
	if (0.0 == log_two_pi) log_two_pi = -log (2.0*M_PI);
	
	cplex z, tps;

	z = periodic_zeta (s,q);

	tps = cplex_scale (log_two_pi, s);

	gsl_sf_result lnr, arg;
	gsl_sf_lngamma_complex_e(s.re+1.0, s.im, &lnr, &arg);

	tps.re += lnr.val;
	tps.im += arg.val;
	
	tps = cplex_exp (tps);

	z = cplex_mul (z, tps);
	z = cplex_scale (2.0, z);
	
	return z;
}

/* ============================================================= */
/**
 * hurwitz_zeta -- Hurwitz zeta function
 *
 * Built up from the periodic beta
 * As of 22 December 2006,seems to be working, with some trouble
 * due to insufficient precision.
 */
cplex hurwitz_zeta (cplex ess, double q)
{
	static double log_two_pi = 0.0;
	if (0.0 == log_two_pi) log_two_pi = -log (2.0*M_PI);

	cplex s = cplex_neg (ess);
	s.re += 1.0;

	cplex zl = periodic_zeta (s, q);
	cplex zh = periodic_zeta (s, 1.0-q);

	cplex piss = cplex_scale (0.5*M_PI, s);
	piss = cplex_times_i(piss);
	piss = cplex_exp (piss);

	zh = cplex_mul (zh, piss);
	zl = cplex_div (zl, piss);

	cplex z = cplex_add (zl, zh);
	
	cplex tps = cplex_scale (log_two_pi, s);

	gsl_sf_result lnr, arg;
	gsl_sf_lngamma_complex_e(s.re, s.im, &lnr, &arg);

	tps.re += lnr.val;
	tps.im += arg.val;
	
	tps = cplex_exp (tps);

	z = cplex_mul (z, tps);

	return z;
}

/* ============================================================= */

/** 
 * test_zeta_values() -- compare periodic zeta to reiman zeta
 * 
 * As of 22 December 2006, this test is passing, with flying colors
 * Explores value of hurwitz zeta on s=real line, for 
 * q=1/2, where it can be compared to the Riemann zeta.
 * Passes, very nicely and cleanly, (i.e. error of order 1e-16)
 * although starts to get rough for the large negative s.
 */
void test_zeta_values (double max)
{
	cplex zl, zh;

	cplex s;
	s.im = 0.0;
	for (s.re = -2; s.re < max; s.re += 0.1)
	{
		if (1 == s.re) continue;

		zl = periodic_zeta (s, 0.5);
		
		/* sm = 1-s */
		cplex sm = cplex_neg(s);
		sm.re += 1.0;

		/* ts = 2^(1-s) */
		cplex ts = cplex_exp(cplex_scale (log(2), sm));
		
		/* ots = -1/(1-2^(1-s)) */
		cplex ots = cplex_neg(ts);
		ots.re += 1.0;
		ots = cplex_recip(ots);
		ots = cplex_neg (ots);
		
		zl = cplex_mul (zl, ots);
		
		double zeta = gsl_sf_zeta (s.re);
		
		printf ("s=%5.3g	algo=%12.10g	exact=%12.10g	diff=%6.3g\n", s.re, zl.re, zeta, zl.re-zeta);
	}
}

/* ============================================================= */

/** 
 * test_bernoulli_poly - compare periodic zeta to the Bernoulli poly's
 *
 * The Bernoulli polynomials have a fourier transform that is the 
 * periodic zeta function. 
 *
 * Test is now passijng with flying colors
 */
int test_bernoulli_poly (int n)
{
	cplex zl, zh;

	cplex s, z;
	s.im = 0.0;
	s.re = n;
	double q;
	for (q=-0.2; q<=1.2; q+=0.02)
	{
		// zl = periodic_zeta (s, q);
		// zh = periodic_zeta (s, 1.0-q);
		zl = periodic_beta (s, q);
		zh = periodic_beta (s, 1.0-q);
		if (n%2) {
			z = cplex_sub (zl,zh);
		} else {
			z = cplex_add (zl,zh);
		}
		
		double bs;
		if (0 == n%2)
		{
	 		bs = z.re;
			if (n%4 == 0) bs = -bs;
		}
		if (n%2)
		{
			bs = -z.im;
			if (n%4 == 3) bs = -bs;
		}

		// bs *= factorial (n) * pow (2.0*M_PI, -n);
		bs *= 0.5;

		/* short-circuit the above, test directly */
		cplex ess;
		ess.re = 1-n;
		ess.im = 0;
		cplex hz = hurwitz_zeta(ess,q);
		bs = -n * hz.re;
		
		// double b = q*q-q+1.0/6.0;
		double b = bernoulli_poly (n,q);
		
		printf ("q=%5.3g	bs=%13.10g	bernoulli=%13.10g	reldiff=%6.3g\n", q, bs, b, (bs-b)/b);
	}
}

/* ============================================================= */

main (int argc, char * argv[])
{
	int n;
	double en;

	if (argc != 2)
	{
		fprintf (stderr, "Usage: %s <n>\n", argv[0]);
		exit (1);
	}
	n = atoi (argv[1]);
	en = atof (argv[1]);

	// test_zeta_values (n);
	// test_bernoulli_poly (n);

// #define HURWITZ_ZETA
#ifdef HURWITZ_ZETA
	/* As of 22 December 2006, this test seems to be passing, 
	 * with decent accuracy, for anything with real part less than about 8
	 */
	cplex s;
	s.im = 0.0;
	double q=0.5;
	for (s.re = 1.05; s.re < n; s.re += 0.1)
	{
		cplex hz= hurwitz_zeta (s, q);
		
		double zeta = gsl_sf_hzeta (s.re, q);
		
		printf ("s=%5.3g	algo=%12.10g	exact=%12.10g	reldiff=%6.3g\n", s.re, hz.re, zeta, (hz.re-zeta)/zeta);
	}
#endif

#if 1
	cplex s;
	s.re = 1.5;
	s.im = en;

	printf ("#\n# periodic zeta for s=%g +i %g\n#\n", s.re, s.im);
	fflush (stdout);

	double q;
	for (q=0.002; q<1.0; q+=0.004)
	{
		// cplex hz = hurwitz_zeta (s, q);
		cplex hz = periodic_zeta_sum (s, q);
		printf ("%7.3g	%15.10g	%15.10g\n", q, hz.re, hz.im);
		fflush (stdout);
	}
#endif
}
