/* 
 * mp-complex.h
 *
 * Utility routines for handling complex values
 *
 * Linas Vepstas October 2006
 */

#include <gmp.h>

#ifndef __MP_COMPLEX_H__
#define __MP_COMPLEX_H__

typedef struct {
	mpf_t re;
	mpf_t im;
} __cpx_struct;

typedef __cpx_struct cpx_t[1];

static inline void cpx_init (cpx_t z)
{
	mpf_init (z[0].re);
	mpf_init (z[0].im);
}

static inline void cpx_clear (cpx_t z)
{
	mpf_clear (z[0].re);
	mpf_clear (z[0].im);
}

static inline void cpx_set (cpx_t z, const cpx_t const y)
{
	mpf_set (z[0].re, y[0].re);
	mpf_set (z[0].im, y[0].im);
}

static inline void cpx_set_ui (cpx_t z, unsigned int x, unsigned int y)
{
	mpf_set_ui (z[0].re, x);
	mpf_set_ui (z[0].im, y);
}

static inline void cpx_set_d (cpx_t z, double x, double y)
{
	mpf_set_d (z[0].re, x);
	mpf_set_d (z[0].im, y);
}

static inline void cpx_set_mpf (cpx_t z, const mpf_t x, const mpf_t y)
{
	mpf_set (z[0].re, x);
	mpf_set (z[0].im, y);
}

static inline void cpx_add (cpx_t sum, const cpx_t const a, const cpx_t const b)
{
	mpf_add (sum[0].re, a[0].re, b[0].re);
	mpf_add (sum[0].im, a[0].im, b[0].im);
}

static inline void cpx_add_ui (cpx_t sum, const cpx_t const a, unsigned int rb, unsigned int ib)
{
	mpf_add_ui (sum[0].re, a[0].re, rb);
	mpf_add_ui (sum[0].im, a[0].im, ib);
}

static inline void cpx_add_mpf (cpx_t sum, const cpx_t const a, const mpf_t b)
{
	mpf_add (sum[0].re, a[0].re, b);
}

static inline void cpx_sub (cpx_t dif, const cpx_t const a, const cpx_t const b)
{
	mpf_sub (dif[0].re, a[0].re, b[0].re);
	mpf_sub (dif[0].im, a[0].im, b[0].im);
}

static inline void cpx_sub_ui (cpx_t sum, const cpx_t const a, unsigned int rb, unsigned int ib)
{
	mpf_sub_ui (sum[0].re, a[0].re, rb);
	mpf_sub_ui (sum[0].im, a[0].im, ib);
}

static inline void cpx_sub_mpf (cpx_t sum, const cpx_t const a, const mpf_t b)
{
	mpf_sub (sum[0].re, a[0].re, b);
}

static inline void cpx_neg (cpx_t neg, const cpx_t const a)
{
	mpf_neg (neg[0].re, a[0].re);
	mpf_neg (neg[0].im, a[0].im);
}

/**
 * cpx_times_i -- z = a*i
 */
static inline void cpx_times_i (cpx_t z, const cpx_t const a)
{
	mpf_t tmp;
	mpf_init (tmp);
	mpf_set (tmp, a[0].re);
	mpf_neg (z[0].re, a[0].im);
	mpf_set (z[0].im, tmp);
	mpf_clear (tmp);
}

/**
 * cpx_mul -- prod = a * b
 */
static inline void cpx_mul (cpx_t prod, const cpx_t const a, const cpx_t const b)
{
	mpf_t pre, pim, tmp;
	mpf_init (pre);
	mpf_init (pim);
	mpf_init (tmp);
	
	mpf_mul (tmp, a[0].im, b[0].im);
	mpf_mul (pre, a[0].re, b[0].re);
	mpf_sub (pre, pre, tmp);
	
	mpf_mul (tmp, a[0].im, b[0].re);
	mpf_mul (pim, a[0].re, b[0].im);
	mpf_add (pim, pim, tmp);
	
	mpf_set (prod[0].re, pre);
	mpf_set (prod[0].im, pim);

	mpf_clear (pre);
	mpf_clear (pim);
	mpf_clear (tmp);
}

/**
 * cpx_mul_mpf -- prod = a * b
 */
static inline void cpx_mul_mpf (cpx_t prod, const cpx_t const a, const mpf_t b)
{
	mpf_mul (prod[0].re, a[0].re, b);
	mpf_mul (prod[0].im, a[0].im, b);
}

/**
 * cpx_mul_mpf -- prod = a * b
 */
static inline void cpx_mul_ui (cpx_t prod, const cpx_t const a, unsigned int b)
{
	mpf_mul_ui (prod[0].re, a[0].re, b);
	mpf_mul_ui (prod[0].im, a[0].im, b);
}

/**
 * cpx_recip -- recip = 1/z
 */
static inline void cpx_recip (cpx_t recip, const cpx_t const z)
{
	mpf_t mag,tmp;
	mpf_init (mag);
	mpf_init (tmp);
	mpf_mul (mag, z[0].re, z[0].re);
	mpf_mul (tmp, z[0].im, z[0].im);
	mpf_add (mag, mag, tmp);
	mpf_ui_div (mag, 1, mag);
	mpf_mul (recip[0].re, z[0].re, mag);
	mpf_mul (recip[0].im, z[0].im, mag);
	mpf_neg (recip[0].im, recip[0].im);
	
	mpf_clear (mag);
	mpf_clear (tmp);
}

/**
 * cpx_div -- ratio = a/b
 */
static inline void cpx_div (cpx_t ratio, const cpx_t const a, const cpx_t const b)
{
	cpx_t recip;
	cpx_init (recip);

	cpx_recip (recip, b);
	cpx_mul (ratio, a, recip);
	cpx_clear (recip);
}

/**
 * cpx_div_mpf -- ratio = a/b
 */
static inline void cpx_div_mpf (cpx_t ratio, const cpx_t const a, const mpf_t b)
{
	mpf_div (ratio[0].re, a[0].re, b);
	mpf_div (ratio[0].im, a[0].im, b);
}

static inline void cpx_div_ui (cpx_t ratio, const cpx_t const a, unsigned int b)
{
	mpf_div_ui (ratio[0].re, a[0].re, b);
	mpf_div_ui (ratio[0].im, a[0].im, b);
}

/**
 * cpx_mod_sq -- modulus squared of z
 */
static inline void cpx_mod_sq (mpf_t mod, const cpx_t const z)
{
	mpf_t tmp;
	mpf_init (tmp);
	
	mpf_mul (tmp, z[0].im, z[0].im);
	mpf_mul (mod, z[0].re, z[0].re);
	mpf_add (mod, mod, tmp);
	mpf_clear (tmp);
}

#endif /* __MP_COMPLEX_H__ */
