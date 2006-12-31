/*
 * mp-consts.h
 *
 * High-precison constants, using the 
 * Gnu Multiple-precision library.
 * Uses caching so as to speed up subsequent derefs.
 *
 * Linas Vepstas July 2005
 */

#include <gmp.h>

/**
 * fp_half_sqrt_three - return sqrt(3)/2 = 0.86602...
 */
void fp_half_sqrt_three (mpf_t sqt);

/**
 * fp_e - return e=2.718281828...
 * @prec - number of decimal places of precision
 *
 * Uses simple, brute-force summation
 */
void fp_e (mpf_t e, unsigned int prec);

/**
 * fp_pi - return pi=3.14159... 
 * @prec - number of decimal places of precision
 *
 * Uses simple, brute-force Machin formula
 */
void fp_pi (mpf_t pi, unsigned int prec);
void fp_two_pi (mpf_t pi, unsigned int prec);
void fp_pi_half (mpf_t pihalf, unsigned int prec);
void fp_sqrt_two_pi (mpf_t sqtpi, unsigned int prec);
void fp_log_two_pi (mpf_t ltpi, unsigned int prec);

/**
 * fp_e_pi - return e^pi 
 * @prec - number of decimal places of precision
 *
 * Uses simple, low-brow formula
 */
void fp_e_pi (mpf_t e_pi, unsigned int prec);

/**
 * fp_log2 - return log(2)=0.693147181...
 * @prec - number of decimal places of precision
 *
 * Uses simple, brute-force summation
 */
void fp_log2 (mpf_t l2, unsigned int prec);

/**
 * fp_euler - return Euler-Mascheroni const
 * @prec - number of decimal places of precision
 *
 */
void fp_euler_mascheroni (mpf_t gam, unsigned int prec);

/**
 * fp_zeta_half - return zeta(1/2)
 * @prec - number of decimal places of precision
 *
 * Current algorithm is terribly slow.
 */
void fp_zeta_half (mpf_t zeta, unsigned int prec);

