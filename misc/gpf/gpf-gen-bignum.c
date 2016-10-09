/*
 * Generating functions for greatest prime factors.
 * Implementation in bignums.
 *
 * Linas Vepstas - April 2016
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <mp-trig.h>
#include <mp-complex.h>

#include <binomial.h>
#include <gpf.h>
#include <prime.h>
#include "gpf-gen-bignum.h"

/*
 * Ordinary generating function for the greatest prime factor.
 */
void cpx_gpf_ordinary(cpx_t sum, cpx_t z, int prec)
{
	mpf_t zabs, gabs, epsi;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	// Not defined for |z| > 1
	mpf_sub_ui(gabs, gabs, 1);
	mpf_neg(gabs, gabs);
	if (0 > mpf_cmp(gabs, epsi)) return;

	double dist_to_circle = mpf_get_d(gabs);
	int niter = ceil (2.302585*prec / dist_to_circle);
	niter += ceil (log(niter) / dist_to_circle); // gpf bounded by n

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; n < niter ; n++)
	{
		cpx_times_ui(term, zn, gpf(n));
		cpx_add(sum, sum, term);
		cpx_mul(zn, zn, z);

#if SLOW_VERSION_NOT_USING_NITER
		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, zn);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_div(gabs, gabs, zabs);

		// if (n * zn < epsi * sum) return;
		if (0 > mpf_cmp(gabs, epsi)) return;
#endif
	}
}

/*
 * Ordinary generating function for one over the greatest prime factor.
 */
void cpx_gpf_ordinary_recip(cpx_t sum, cpx_t z, int prec)
{
	mpf_t zabs, gabs, epsi;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	// Not defined for |z| > 1
	mpf_sub_ui(gabs, gabs, 1);
	mpf_neg(gabs, gabs);
	if (0 > mpf_cmp(gabs, epsi)) return;

	double dist_to_circle = mpf_get_d(gabs);
	int niter = ceil (2.302585*prec / dist_to_circle);

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; n < niter; n++)
	{
		cpx_div_ui(term, zn, gpf(n));
		cpx_add(sum, sum, term);
		cpx_mul(zn, zn, z);
	}
}

/**
 * Exponential generating function for the greatest prime factor.
 * shifted .. aka the 'offset'th derivative.
 */
void cpx_gpf_exponential_shift(cpx_t sum, cpx_t z, int offset, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		cpx_times_ui(term, zn, gpf(n+offset));
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// The offset is the n'th derivative.  That means that
	// there is a constant term; the above loop did not handle it.
	// add that constant now.
	if (offset != 0)
	{
		cpx_add_ui(sum, sum, gpf(offset), 0);
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

void cpx_gpf_exponential(cpx_t sum, cpx_t z, int prec)
{
	cpx_gpf_exponential_shift(sum, z, 0, prec);
}

// =========================================================
/**
 * Sine generating function for the greatest prime factor.
 * Same as exponential GPF, but takes only the odd terms.
 * (and inserts an alternating sign!)
 */
void cpx_gpf_sine(cpx_t sum, cpx_t z, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, zsq, term;
	cpx_init(zn);
	cpx_init(zsq);
	cpx_init(term);
	cpx_set(zn, z);
	cpx_set(zsq, z);
	cpx_mul(zsq, zsq, z);
	cpx_neg(zsq, zsq); // zqs = -z^2

	for (int n=1; ; n+=2)
	{
		cpx_times_ui(term, zn, gpf(n));
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_mul(zn, zn, zsq);
		mpf_div_ui(fact, fact, (n+1)*(n+2));
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

/**
 * Cosine generating function for the greatest prime factor.
 * Same as exponential GPF, but takes only the even terms.
 * (and inserts an alternating sign!)
 */
void cpx_gpf_cosine(cpx_t sum, cpx_t z, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 2);  // start with 2!
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, zsq, term;
	cpx_init(zn);
	cpx_init(zsq);
	cpx_init(term);
	cpx_set(zn, z);
	cpx_mul(zn, zn, z); // start with z^2
	cpx_set(zsq, zn);
	cpx_neg(zsq, zsq); // zsq = -z^2

	for (int n=2; ; n+=2)
	{
		cpx_times_ui(term, zn, gpf(n));
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_mul(zn, zn, zsq);
		mpf_div_ui(fact, fact, (n+1)*(n+2));
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}
// =========================================================

#define NARR 250123
static int rnums[NARR];
__attribute__((constructor)) static void mkran()
{
	srandom(42);
	srandom(69);
	for (int i=0; i< NARR; i++)
	{
		rnums[i] = 2 + (i * random()) / RAND_MAX;
	}
}

static int randy(int n)
{
	if (NARR <= n) exit(1);
	return rnums[n];
}

/* Like above, but for some random numbers */
void cpx_random_exponential_shift(cpx_t sum, cpx_t z, int offset, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		// unsigned int rando = randy(n+offset);
		// unsigned int rando = pseudo_gpf(n+offset);  // XXX insane

		// How many primes less than n?
		// Pick one of them, randomly.
		unsigned int nprimes = prime_count(n);
		unsigned int rando = randy(nprimes);
		rando = get_nth_prime(rando);
		cpx_times_ui(term, zn, rando);
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// The offset is the n'th derivative.  That means that
	// there is a constant term; the above loop did not handle it.
	// add that constant now.
	if (offset != 0)
	{
		cpx_add_ui(sum, sum, randy(offset), 0);
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

// =========================================================

static int gpf_newton(int n, int order)
{
	if (0 == order) return gpf(n);
	if (1 == order) return gpf(n+1) - gpf(n);

	int tot = 0;
	int sgn = 1;
	for (int j=0; j<=order; j++)
	{
		tot += sgn * gpf(n+j) * binomial(order, j);
		sgn = -sgn;
	}
	return tot;
}

/*
 * Takes newton differences of the shifted functions
 */
void cpx_gpf_exponential_newton(cpx_t sum, cpx_t z, int order, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		cpx_times_ui(term, zn, gpf_newton(n, order));
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// The offset is the n'th derivative.  That means that
	// there is a constant term; the above loop did not handle it.
	// add that constant now.
	if (order != 0)
	{
		int sgn = -1;
		int tot = 0;
		for (int j=1; j<=order; j++)
		{
			tot += sgn * gpf(j) * binomial(order, j);
			sgn = -sgn;
		}
		cpx_add_ui(sum, sum, tot, 0);
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

// =========================================================
/**
 * Exponential generating function for the reciprocal of the
 * greatest prime factor.
 */
void cpx_gpf_exponential_recip(cpx_t sum, cpx_t z, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		cpx_div_ui(term, zn, gpf(n));
		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_div_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_div(gabs, gabs, zabs);

		// if (n * zn * fact < epsi * sum) return;
		if (0 > mpf_cmp(gabs, epsi)) break;

		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

/**
 * Exponential generating function for (gpf)^s where gpf is the
 * greatest prime factor.  This generalizes the above two functions,
 * in that the first has s=1, and the second has s=-1.
 *
 * Not done: (gpf/n)^s -- might be different/better?
 */
void cpx_gpf_exponential_s(cpx_t sum, cpx_t z, cpx_t ess, int prec)
{
	mpf_t zabs, gabs, epsi, fact;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_init (fact);
	mpf_set_ui(fact, 1);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, z);
	mpf_mul_ui(gabs, gabs, 100);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t zn, term;
	cpx_init(zn);
	cpx_init(term);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		cpx_ui_pow_cache(term, gpf(n), ess, prec);
		cpx_mul(term, term, zn);

		cpx_times_mpf(term, term, fact);
		cpx_add(sum, sum, term);

		// The following checks the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_div(gabs, gabs, zabs);

		// if (n * zn * fact < epsi * sum) return;
		if (0 > mpf_cmp(gabs, epsi)) break;

		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// Remove the leading exponential order.
	cpx_abs(gabs, z);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

/*
 * Dirichlet generating function for the greatest prime factor.
 * actually, for gpf(n)/n so as to improve convergence.
 */
void cpx_gpf_dirichlet(cpx_t sum, cpx_t ess, int prec)
{
	mpf_t zabs, gabs, epsi;
	mpf_init (gabs);
	mpf_init (zabs);
	mpf_init (epsi);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	cpx_t term;
	cpx_init(term);

	cpx_neg(ess, ess);

	for (int n=1; ; n++)
	{
		cpx_ui_pow_cache(term, n, ess, prec);
// printf("duuude %d %d %g %g term = %g %g\n", n, gpf(n), cpx_get_re(ess), cpx_get_im(ess), cpx_get_re(term), cpx_get_im(term));
		cpx_times_ui(term, term, gpf(n));
		cpx_div_ui(term, term, n);
		cpx_add(sum, sum, term);

		// The following check the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_div(gabs, gabs, zabs);

		// if (n * zn * fact < epsi * sum) return;
		if (0 > mpf_cmp(gabs, epsi)) break;
	}
}

/*
 * Pochhammer generating function for the greatest prime factor.
 * This is like the exponential generating function, but uses pochhammer
 * instead of x^n.  Actually, uses binomial coefficient ... two
 * factorials are needed for convergence.
 */
void cpx_gpf_poch(cpx_t sum, cpx_t zorig, int prec, bool rise)
{
	mpf_t zabs, gabs, fact, epsi, one;
	mpf_init (gabs);
	mpf_init (zabs);

	mpf_init (fact);
	mpf_set_ui(fact, 1);

	mpf_init (one);
	mpf_set_ui(one, 1);
	if (!rise)
		mpf_neg(one, one);

	mpf_init (epsi);
	mpf_set_ui(epsi, 1);
	mpf_div_2exp(epsi, epsi, (int)(3.321*prec));

	cpx_set_ui(sum, 0, 0);

	// falls apart if z is zero.
	cpx_abs(gabs, zorig);
	if (0 > mpf_cmp(gabs, epsi)) return;

	cpx_t z, zn, term;
	cpx_init(z);
	cpx_init(zn);
	cpx_init(term);
	cpx_set(z, zorig);
	cpx_set(zn, z);

	for (int n=1; ; n++)
	{
		cpx_times_ui(term, zn, gpf(n));
		cpx_times_mpf(term, term, fact);
		cpx_times_mpf(term, term, fact); // A second factorial!
		cpx_add(sum, sum, term);

		// The following check the loop termination condition,
		// which is that the size of the term is less than epsilon.
		cpx_abs(gabs, term);
		mpf_mul_ui(gabs, gabs, n);

		cpx_abs(zabs, sum);
		mpf_mul(zabs, zabs, epsi);

		// if (n * zn/n! < epsi * sum) return;
		if (0 > mpf_cmp(gabs, zabs)) break;

		cpx_add_mpf(z, z, one);
		cpx_mul(zn, zn, z);
		mpf_div_ui(fact, fact, n+1);
	}

	// Remove the leading exponential order.
	// Its actually exp(-2*sqrt|z|)
	// It seems to be identical for both the rising
	// and the falling factorial versions
	cpx_abs(gabs, zorig);
	mpf_sqrt(gabs, gabs);
	mpf_mul_ui(gabs, gabs, 2);
	mpf_neg(gabs, gabs);
	fp_exp(gabs, gabs, prec);

	cpx_times_mpf(sum, sum, gabs);
}

void cpx_gpf_poch_rising(cpx_t sum, cpx_t z, int prec)
{
	cpx_gpf_poch(sum, z, prec, true);
}

void cpx_gpf_poch_falling(cpx_t sum, cpx_t z, int prec)
{
	cpx_gpf_poch(sum, z, prec, false);
}
