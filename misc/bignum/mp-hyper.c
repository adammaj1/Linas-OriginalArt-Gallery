/*
 * mp-hyper.c
 *
 * High-precison Hypergeometric functions, using the 
 * Gnu Multiple-precision library.
 *
 * Linas Vepstas November 2006
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include "mp-complex.h"

/* ======================================================================= */
/**
 * fp_confluent -- Confluent hypergeometric function
 * Implemented using a brute-force, very simple algo, with 
 * no attempts at optimization. Also, does not assume any 
 * precomputed constants.
 */

void 
cpx_confluent (cpx_t *em, cpx_t *a, cpx_t *b, cpx_t *z, unsigned int prec)
{
	mpf_t fact;
	cpx_t zee, z_n, term;
	cpx_t ay, be, poch_a, poch_b;

	mpf_init (fact);

	cpx_init (&zee);
	cpx_init (&z_n);
	cpx_init (&term);
	cpx_init (&ay);
	cpx_init (&be);
	cpx_init (&poch_a);
	cpx_init (&poch_b);

	/* Make copy of arguments now! */
	cpx_set (&zee, z);
	cpx_set (&z_n, &zee);
	
	cpx_set (&ay, a);
	cpx_set (&poch_a, a);
	cpx_set (&be, b);
	cpx_set (&poch_b, b);
	
	cpx_set_ui (em, 1, 0);
	mpf_set_ui (fact, 1);

	// double mex = ((double) prec) * log (10.0) / log(2.0);
	double mex = ((double) prec) * 3.321928095;
	unsigned int imax = (unsigned int) (mex +1.0);
	mpf_t maxterm, one;
	mpf_init (maxterm);
	mpf_init (one);
	mpf_set_ui (one, 1);
	mpf_div_2exp (maxterm, one, 2*imax);

	unsigned int n=1;
	while(1)
	{
		cpx_div_mpf (&term, &z_n, fact);
		cpx_mul (&term, &term, &poch_a);
		cpx_div (&term, &term, &poch_b);
		cpx_add (em, em, &term);
		
		/* Don't go no farther than this */
		cpx_mod_sq (one, &term);
		if (mpf_cmp (one, maxterm) < 0) break;
		
		n++;
		cpx_mul (&z_n, &z_n, &zee);
		mpf_mul_ui (fact, fact, n);

		mpf_add_ui (ay.re, ay.re, 1);
		mpf_add_ui (be.re, be.re, 1);
		cpx_mul (&poch_a, &poch_a, &ay);
		cpx_mul (&poch_b, &poch_b, &be);
	}
	
	mpf_clear (fact);
	mpf_clear (one);
	mpf_clear (maxterm);
	
	cpx_clear (&zee);
	cpx_clear (&z_n);
	cpx_clear (&term);
	cpx_clear (&ay);
	cpx_clear (&be);
	cpx_clear (&poch_a);
	cpx_clear (&poch_b);
}

/* =============================== END OF FILE =========================== */

