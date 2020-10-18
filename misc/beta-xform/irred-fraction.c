/*
 * irred-fraction.c
 *
 * Find integer sequence for the golden polynomials.
 * Relate it to the continued-fraction representation.
 * See irred.c for additional utilities.
 *
 * February 2018, October 2020
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/* Return the n'th golden polynomial. It can be constructed from
 * the bit string of (2n+1).
 */
double beta(unsigned long n, double x)
{
	double acc = 0.0;
	double xn = 1.0;
	unsigned long bitstr = 2*n+1;
	while (bitstr)
	{
		if (bitstr%2 == 1) acc += xn;
		xn *= x;
		bitstr >>= 1;
	}
// printf("duuude n=%d x=%20.16g beta=\n", n, x, xn-acc);
	return xn - acc;
}

/* Use midpoint bisection to find the single, unique
 * positive real zero of the n'th golden polynomial.
 */
double find_zero(unsigned long n, double lo, double hi)
{
	double mid = 0.5 * (lo+hi);
	if (1.0e-15 > hi-lo) return mid;
	double fmid = beta(n, mid);
	if (0.0 < fmid) return find_zero(n, lo, mid);
	return find_zero(n, mid, hi);
}

/* Return length of bitstr, length in bits */
int len(unsigned long n)
{
	int len=0;
	unsigned long bitstr = 2*n+1;
	while (bitstr) { len++; bitstr >>= 1; }
	return len;
}

/** Helper array, needed for finding gold midpoints */
double* zero = NULL;
void setup_gold(int nmax)
{
	zero = (double*) malloc(nmax*sizeof(double));
	for (int i=0; i< nmax; i++) zero[i] = -1.0;
}

// Return true if this is a valid polynomial
bool zero_is_bracketed(int n, double gold)
{
	// Its valid only if it is in the middle.
#define EPS 2.0e-15
	// printf("---------\ngold=%g\n", gold);
	bool ork = true;
	int nhl = n;
	int nh = nhl >> 1;
	while (nh)
	{
		// printf("duuude n=%d nhl=%d nh=%d znh=%g go=%g comp=%d bad=%d\n",
		// n, nhl, nh, zero[nh], gold, 0 == nhl%2, zero[nh] <= gold);
		if (0 == nhl%2 && zero[nh] < gold+EPS) {ork = false; break; }
		nhl = nh;
		nh >>= 1;
	}

	return ork;
}

/**
 * Find the single, unique real zero of the n'th golden polynomial.
 * If the value of `n` does not really correspond to a golden
 * polynomial, return zero.
 */
double find_gold(int n)
{
	double gold = zero[n];
	if (gold < -0.5)
	{
		gold = find_zero(n, 1.0, 2.0);
		zero[n] = gold;
	}

	if (zero_is_bracketed(n, gold)) return gold;
	return 0.0;
}

/*
 * Given the continue-fraction representation, return the
 * corresponding sequence number.
 */
long sequence_from_cf(int cfrac[], int len)
{
	if (1 == len) return 1 << cfrac[0];

	if (3 < len) return -1; // unknown.

	long leader = sequence_from_cf(cfrac, len-1);

	long follower = 2*leader + 1;
	follower *= 1 << (cfrac[0] + cfrac[len-1]);

	return follower;
}

/*
 * Iterate on the continued fraction.
 * i.e. generate sequences
 * maxdepth == number of doubling steps
 * maxlength == max length of fraction.
 * maxn == cutoff fir highest known n
 */
void iterate_cf(int cfrac[], int len, int maxdepth, int maxlength, long maxn)
{
}

int main(int argc, char* argv[])
{
	int nmax = (1<<21) + 1;

	setup_gold(nmax);

	for (int n=0; n<nmax; n ++)
		find_gold(n);

}
