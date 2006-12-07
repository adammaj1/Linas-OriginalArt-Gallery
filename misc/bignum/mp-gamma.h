/*
 * mp-gamma.h
 *
 * Compute gamma function for various complex arguments
 *
 * Linas Vepstas December 2006
 */

#include <gmp.h>

/**
 * fp_lngamma -- compute log of gamma for real argument
 *
 * Uses simple, quickly converging algo-- A&S 6.1.33
 */
void fp_lngamma (mpf_t gam, mpf_t ex, int prec);
