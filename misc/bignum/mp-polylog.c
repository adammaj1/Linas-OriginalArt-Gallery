
/** 
 * mp-polylog.c
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

#include "mp-binomial.h"
#include "mp-complex.h"
//#include "mp-bernoulli.h"
//#include "mp-harmonic.h"

/* 
 * bee_k() 
 * Return value of sum_{j=0}^k (n j) oz^j
 *
 * where (n j) is binomial coefficient 
 */
static void bee_k (cpx_t bee, int n, int k, cpx_t oz)
{
	int j;
	cpx_t pz, z, term;
	mpz_t bin;
	mpf_t binom;

	mpz_init (bin);
	mpf_init (binom);
	cpx_init (z);
	cpx_init (pz);
	cpx_init (term);

	/* Make a copy if input arg now! */
	cpx_set (z, oz);
	cpx_set_ui (pz, 1,0);
	cpx_set_ui (bee, 0,0);

	for (j=0; j<=k; j++)
	{
		cpx_set (term, pz);

		i_binomial (bin, n,j);
		mpf_set_z (binom, bin);
		cpx_mul_mpf (term, term, binom);
		cpx_add (bee, bee, term);
		cpx_mul(pz, pz, z);
	}

	mpz_clear (bin);
	mpf_clear (binom);
	cpx_clear (z);
	cpx_clear (pz);
	cpx_clear (term);
}

#if 0
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
	ska = cplex_mult (z, cplex_recip(ska));
	ska = cplex_pow (ska,n);
	
	cplex pz = z;
	cplex acc = cplex_zero();
	cplex cacc = cplex_zero();

	for (k=1; k<=n; k++)
	{
		/* The inverse integer power */
		cplex dir = cplex_scale (log(k), s);
		dir = cplex_exp (cplex_neg(dir));

		/* Put it together */
		cplex term = cplex_mult (pz, dir);
		acc = cplex_add (acc, term);

		pz = cplex_mult(pz, z);
	}

	for (k=n+1; k<=2*n; k++)
	{
		/* The inverse integer power */
		cplex dir = cplex_scale (log(k), s);
		dir = cplex_exp (cplex_neg(dir));

		/* Put it together */
		cplex term = cplex_mult (pz, dir);
		acc = cplex_add (acc, term);

		/* The coefficient c_k of the polynomial */
		cplex ck = bee_k (n,k-n-1,moz);
		term = cplex_mult (ck, term);
		cacc = cplex_add (cacc, term);

		pz = cplex_mult(pz, z);
	}

	cacc = cplex_mult (cacc, ska);
	acc = cplex_sub (acc, cacc);

	return acc;
}

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
		bt = cplex_mult (bt, ts);

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
		bt = cplex_mult (bt, ts);

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

	z = cplex_mult (z, tps);
	z = cplex_scale (2.0, z);
	
	return z;
}

/* ============================================================= */
/**
 * hurwitz_zeta -- Hurwitz zeta function
 *
 * Built up from the periodic beta
 * Borken at the mopment
 *
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

	zh = cplex_mult (zh, piss);
	zl = cplex_div (zl, piss);

	cplex z = cplex_add (zl, zh);
	
	cplex tps = cplex_scale (log_two_pi, s);

	gsl_sf_result lnr, arg;
	gsl_sf_lngamma_complex_e(s.re, s.im, &lnr, &arg);

	tps.re += lnr.val;
	tps.im += arg.val;
	
	tps = cplex_exp (tps);

	z = cplex_mult (z, tps);

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
		
		zl = cplex_mult (zl, ots);
		
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
#include <stdlib.h>

#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_zeta.h>

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
	s.re = en;
	s.im = 0.5;

	double q;
	for (q=0.02; q<1.0; q+=0.04)
	{
		cplex hz = hurwitz_zeta (s, q);
		printf ("%7.3g	%15.10g	%15.10g\n", q, hz.re, hz.im);
	}
#endif
}
#endif
