/*
 * isqrt.c
 *
 * Return the integer square-root of an integer.
 *
 * Updates October 2016
 */

#include <math.h>

#include "isqrt.h"

/* ====================================================== */

int isqrt(int n)
{
	int res = 0;
	int bit = 1 << (8*sizeof(int)-2);
	while (bit > n) bit >>= 2;

	while (bit != 0)
	{
		int rpb = res + bit;
		if (n >= rpb)
		{
			n -= rpb;
			res = (res >> 1) + bit;
		}
		else  res >>= 1;
		bit >>= 2;
	}
	return res;
}

int integer_sqrt(int n)
{
	if (0 == n) return 0;
	int lo = 2 * integer_sqrt(n/4);
	int hi = lo+1;
	if (n < hi*hi) return lo;
	return hi;
}

static int integer_nth_helper(int n, int p, int tp)
{
	if (0 == n) return 0;
	int lo = 2 * integer_nth_helper(n/tp, p, tp);
	int hi = lo+1;

	int hipow = 1;
	for (int i=0; i<p; i++) hipow *= hi;
	if (n < hipow) return lo;
	return hi;
}

int integer_nth_root(int n, int p)
{
	return integer_nth_helper(n, p, 1<<p);
}

/* ====================================================== */

// #define TEST 1
#ifdef TEST

#include <stdio.h>

int test_isqrt (void)
{
	int have_error=0;
	int i;
	int nmax=10000;
	for (i=1; i<=nmax; i++)
	{
		int is = isqrt(i);
		if (is != integer_sqrt(i))
		{
			printf ("ERROR: in int sqrt function at n=%d\n", i);
			have_error ++;
		}
		if (is*is > i)
		{
			printf ("ERROR: in integer sqrt function at n=%d\n", i);
			have_error ++;
		}
	}

	if (0 == have_error)
	{
		printf ("PASS: tested isqrt function up to %d\n", nmax);
	}
	return have_error;
}

int main()
{
	test_isqrt();

	return 1;
}
#endif /* TEST */


/* --------------------------- END OF FILE ------------------------- */
