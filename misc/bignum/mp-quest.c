/*
 * mp-quest.c
 *
 * High-precison Minkowski Question mark, Stern-Brocot Tree, etc.
 * using the Gnu Multiple-precision library.
 *
 * Copyright (C) 2010 Linas Vepstas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 */

#include <gmp.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp-quest.h"

void question_inverse (mpf_t qinv, const mpf_t x, unsigned int prec)
{
	int i, istart, n;
	unsigned long idx, last_idx;
	mpf_t mantissa;
	mpz_t bits;

	mpf_init(mantissa);
	mpz_init(bits);

	/* Get the number of binary bits from prec = log_2 10 * prec */
	int nbits = (int) floor (3.321 * prec);
	nbits -= 3;

   int *bitcnt = (int *) malloc ((nbits+1) * sizeof(int));
	memset (bitcnt, 0, (nbits+1) * sizeof(int));

	mpf_mul_2exp(mantissa, x, nbits);
	mpz_set_f(bits, mantissa);

	/* Count the number of contiguous bits */
	idx = 0;
	last_idx = 0;
	n = 0;
	// printf("duude nbits=%d\n", nbits);
	while (n <= nbits)
	{
		if (n%2 == 0)
		{
   		idx = mpz_scan0(bits, idx);
		}
		else
		{
   		idx = mpz_scan1(bits, idx);
		}
		if (ULONG_MAX == idx)
		{
			bitcnt[n] = nbits - last_idx;
			n++;
			break;
		}
		bitcnt[n] = idx - last_idx;
		last_idx = idx;
		n++;
	}

	/* Compute the corresponding continued fraction */
	mpf_set_ui(qinv, 0);
	istart = 2;  /* skip over trailing zeroes */
	if (0 != bitcnt[0])
	{
		istart = 0;
	}
	for (i = istart ; i<n; i++)
	{
		if (n-1 == i)
			mpf_add_ui(qinv, qinv, bitcnt[i] + 1);
		else
			mpf_add_ui(qinv, qinv, bitcnt[i]);
		mpf_ui_div(qinv, 1, qinv);
	}

   free (bitcnt);
}

/* ================================================================ */

#ifdef RUN_TEST
int main (int argc, char * argv[])
{
	int n, d;
	double qid;
	mpf_t qi, x;
	int prec, nbits;

	d = atoi(argv[1]);

	prec = 50;

	/* Set the precision (number of binary bits) */
	nbits = 3.3*prec;
	mpf_set_default_prec (nbits);

	mpf_init(qi);
	mpf_init(x);

	// n = 2*171;
	// d = 3*171;
	for (n=0; n<=d; n++)
	{
		double xd = ((double) n) / ((double) d);
		mpf_set_ui (x, n);
		mpf_div_ui (x, x, d);

		question_inverse(qi, x, prec);
		qid = mpf_get_d(qi);
		printf("%d	%f	%f\n", n, xd, qid);
	}
	
	return 0;
}
#endif

/* =============================== END OF FILE =========================== */
