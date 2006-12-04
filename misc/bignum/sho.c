
/*
 * Simple Harmonic Oscilltor
 *
 * Linas November 2006
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include "mp-binomial.h"
#include "mp-complex.h"
#include "mp-consts.h"
#include "mp-hyper.h"
#include "mp-misc.h"
#include "mp-trig.h"

/* ======================================================== */
/* The two eigenfunctions of the simple harmonic oscillator */

void psi_1 (cpx_t psi, cpx_t lambda, cpx_t y, int prec)
{
	cpx_t a, b, z;
	cpx_init (a);
	cpx_init (b);
	cpx_init (z);

	/* b=1/2 */
	cpx_set_ui (b, 1, 0);
	mpf_div_ui (b[0].re, b[0].re, 2);

	/* a= 1/4-lambda/2 */
	cpx_set (a,b);
	cpx_sub (a, a, lambda);
	cpx_div_ui (a, a, 2);

	/* z = y^2 */
	cpx_mul (z, y, y);

	cpx_confluent (psi, a, b, z, prec);

	/* psi_1 = exp (-y^2/2) * M(a,b,z) */
	cpx_div_ui (z, z, 2);
	cpx_neg (z, z);
	cpx_exp (z, z, prec);
	cpx_mul (psi, psi, z);
	
	cpx_clear (a);
	cpx_clear (b);
	cpx_clear (z);
}

void psi_2 (cpx_t psi, cpx_t lambda, cpx_t y, int prec)
{
	cpx_t a, b, z;
	cpx_init (a);
	cpx_init (b);
	cpx_init (z);

	/* b=3/2 */
	cpx_set_ui (b, 3, 0);
	mpf_div_ui (b[0].re, b[0].re, 2);

	/* a= (3/2-lambda)/2 */
	cpx_set (a,b);
	cpx_sub (a, a, lambda);
	cpx_div_ui (a, a, 2);

	/* z = y^2 */
	cpx_mul (z, y, y);

	cpx_confluent (psi, a, b, z, prec);

	/* psi_2 = y * exp (-y^2/2) * M(a,b,z) */
	cpx_div_ui (z, z, 2);
	cpx_neg (z, z);
	cpx_exp (z, z, prec);
	cpx_mul (psi, psi, z);
	cpx_mul (psi, psi, y);
	
	cpx_clear (a);
	cpx_clear (b);
	cpx_clear (z);
}

/* ======================================================== */
/* cheap hack for the gamma function. Incorrect, but holds water.
 * Approximates gamma(z)=1 for 1<z<2.
 */
static void gamma_hack (cpx_t gam, const cpx_t const z)
{
	cpx_t reduced_gamma;
	cpx_init (reduced_gamma);
	cpx_set_ui (reduced_gamma, 1, 0);
	
	cpx_t ans;
	cpx_init (ans);
	
	double flo = mpf_get_d (z[0].re);
	if (flo > 2.0)
	{
		unsigned int intpart = (unsigned int) floor (flo-1.0);
printf ("duude intpart=%d\n", intpart);
		cpx_set (ans, z);
		mpf_sub_ui (ans[0].re, ans[0].re, intpart);
cpx_prt ("zminusi=", ans);
printf ("\n");
		cpx_poch_rising (ans, ans, intpart);
cpx_prt ("pockky=", ans);
printf ("\n");
	}
	else if (flo < 1.0)
	{
		unsigned int intpart = (unsigned int) floor (1.0-flo);
		cpx_set (ans, z);
		cpx_poch_rising (ans, ans, intpart);
		cpx_recip (ans, ans);
	}
	else
	{
		cpx_set_ui (ans, 1, 0);
	}

	cpx_set (gam, ans);
	cpx_clear (ans);
	cpx_clear (reduced_gamma);
}

void eta_1 (cpx_t eta, cpx_t lambda, cpx_t y, int prec)
{
	cpx_t pha, lmo;
	cpx_init (pha);
	cpx_init (lmo);

	psi_1 (eta, lambda, y, prec);
cpx_prt ("eta=", eta);
printf ("\n");

	mpf_t tmp;
	mpf_init (tmp);

	/* phase term, exp (i pi lambda/2) */
	cpx_set (pha, lambda);
	cpx_div_ui (pha, pha, 2);
	fp_pi (tmp, prec);
	cpx_mul_mpf (pha, pha, tmp);
	cpx_times_i (pha, pha);
	cpx_exp (pha, pha, prec);
	cpx_mul (eta, eta, pha);

	/* power of two term */
	mpf_set_ui (tmp, 2);
	cpx_set (pha, lambda);
	cpx_div_ui (pha, pha, 2);
	cpx_pow_mpf (pha, tmp, pha, prec);
	cpx_mul (eta, eta, pha);
cpx_prt ("twoeeta=", eta);
printf ("\n");
	
	/* Gamma (1/4+lambda/2) */
	mpf_set_ui (tmp, 1);
	mpf_div_ui (tmp, tmp, 2);
	cpx_add_mpf (lmo, lambda, tmp);
	cpx_div_ui (pha, lmo, 2);
cpx_prt ("half lammy=", pha);
printf ("\n");
	gamma_hack (pha, pha);
cpx_prt ("gammaup=", pha);
printf ("\n");
	cpx_mul (eta, eta, pha);

	/* Gamma (lambda + 1/2) */
cpx_prt ("lammy=", lmo);
printf ("\n");
	gamma_hack (pha, lmo);
cpx_prt ("gammadown=", pha);
printf ("\n");
	cpx_div (eta, eta, pha);

	cpx_clear (pha);
	cpx_clear (lmo);
	mpf_clear (tmp);
}

void eta_2 (cpx_t eta, cpx_t lambda, cpx_t y, int prec)
{
	cpx_t pha, lmo;
	cpx_init (pha);
	cpx_init (lmo);

	psi_2 (eta, lambda, y, prec);

	mpf_t tmp;
	mpf_init (tmp);

	/* phase term, exp (i pi (lambda-1)/2) */
	cpx_set (pha, lambda);
	cpx_div_ui (pha, pha, 2);
	fp_pi (tmp, prec);
	cpx_mul_mpf (pha, pha, tmp);
	cpx_times_i (pha, pha);
	cpx_exp (pha, pha, prec);
	cpx_times_i (pha, pha);
	cpx_neg (pha, pha);
	cpx_mul (eta, eta, pha);

	/* power of two term */
	mpf_set_ui (tmp, 2);
	cpx_div_ui (pha, lambda, 2);
	cpx_pow_mpf (pha, tmp, pha, prec);
	cpx_mul (eta, eta, pha);
	
	/* Gamma (-1/4+lambda/2) */
	mpf_set_ui (tmp, 1);
	mpf_div_ui (tmp, tmp, 2);
	cpx_sub_mpf (lmo, lambda, tmp);
	cpx_div_ui (pha, lmo, 2);
	gamma_hack (pha, pha);
	cpx_mul (eta, eta, pha);

	/* Gamma (lambda - 1/2) */
	gamma_hack (pha, lmo);
	cpx_div (eta, eta, pha);

	cpx_clear (pha);
	cpx_clear (lmo);
	mpf_clear (tmp);
}

/* ======================================================== */

void coherent (cpx_t coho, cpx_t lambda, cpx_t que, cpx_t y, int prec)
{
	cpx_t e1, e2, qn, lam;
	cpx_init (e1);
	cpx_init (e2);
	cpx_init (qn);
	cpx_init (lam);

	cpx_set (qn, que);

	eta_1 (e1, lambda, y, prec);
	eta_2 (e2, lambda, y, prec);
	cpx_add (coho, e1, e2);
	
	int n;
	for (n=1; n<40; n++)
	{
	printf ("n=%d  \n", n);
		/* lambda+n */
		cpx_add_ui (lam, lambda, n, 0);
		eta_1 (e1, lam, y, prec);
	cpx_prt ("e1=", e1);
	printf ("\n");
		eta_2 (e2, lam, y, prec);
	cpx_prt ("e2=", e2);
	printf ("\n");
		cpx_add (e1, e1, e2);
		cpx_mul (e1, e1, qn);
		cpx_add (coho, coho, e1);
		
		/* lambda-n */
		cpx_sub_ui (lam, lambda, n, 0);
		eta_1 (e1, lam, y, prec);
	cpx_prt ("neg e1", e1);
	printf ("\n");
		eta_2 (e2, lam, y, prec);
	cpx_prt ("neg e2", e2);
	printf ("\n");
		cpx_add (e1, e1, e2);
		cpx_div (e1, e1, qn);
		cpx_add (coho, coho, e1);
		
	cpx_prt ("coho", coho);
	printf ("\n");
	printf ("\n");
		
		cpx_mul (qn, qn, que);
	}

	cpx_clear (e1);
	cpx_clear (e2);	
	cpx_clear (qn);
	cpx_clear (lam);
}

/* ======================================================== */

void prt_graph (double lam, int prec)
{
	cpx_t ps1, ps2, lambda, z;
	cpx_init (ps1);
	cpx_init (ps2);
	cpx_init (lambda);
	cpx_init (z);

	cpx_set_ui (lambda, 1,0);
	mpf_set_d (lambda[0].re, lam);

	cpx_t ex;
	cpx_init (ex);
	cpx_set_ui (ex, 0,0);

	double x;
	for (x=-6.0; x<6.01; x+=0.1)
	{
		mpf_set_d (ex[0].re, x);

#if 0
		psi_1 (ps1, lambda, ex, prec);
		psi_2 (ps2, lambda, ex, prec);
#endif

		eta_1 (ps1, lambda, ex, prec);
		eta_2 (ps2, lambda, ex, prec);

		printf ("%g", x);
		fp_prt ("\t", ps1[0].re);
		fp_prt ("\t", ps2[0].re);
		printf ("\n");
	}
}

void do_coho (double lam, int prec)
{
	cpx_t coho, lambda, q, z;
	cpx_init (coho);
	cpx_init (lambda);
	cpx_init (q);
	cpx_init (z);

	cpx_set_ui (lambda, 1,0);
	mpf_set_d (lambda[0].re, lam);

	cpx_set_ui (q, 1,0);
	cpx_set_ui (z, 1,0);
	
	coherent (coho, lambda, q, z, prec);
}

/* ======================================================== */

int
main (int argc, char *argv[])
{
	int prec = 650;
	
	if (2> argc)
	{
		fprintf (stderr, "Usage: %s lambda\n", argv[0]);
		exit (1);
	} 

	double lam;
	lam = atof (argv[1]);

	prt_graph (lam, prec);
	// do_coho (lam, prec);
	
	return 0;
}
