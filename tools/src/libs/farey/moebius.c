
/*
 * moebius.c
 * 
 * Return the moebius function of an integer.
 * not intended for large integers, works only for small integers
 * due to poor-mans factorization algo.
 *
 * Linas Vepstas Jaunuary 2005
 */

#include <math.h>
#include <malloc.h>

#include "moebius.h"
#include "cache.h"

static int *sieve = NULL;
static int sieve_size = 0;
static int sieve_max = 0;

#define INIT_PRIME_SIEVE(N) \
	if (!sieve || sieve[sieve_max]*sieve[sieve_max] <(N)) {\
		init_prime_sieve(N); \
	}

/* Initialize and fill in a prime-number sieve */
static void
init_prime_sieve (int prod)
{
	int n, j;
	int nstart;
	int pos;
	
	if (sieve && sieve[sieve_max]*sieve[sieve_max] > prod) return;
		
	int max = 1000.0+sqrt (prod);

	if (!sieve)
	{
		sieve = (int *) malloc (8192*sizeof (int));
		sieve_size = 8192;
		sieve_max = 2;
		sieve[0] = 2;
		sieve[1] = 3;
		sieve[2] = 5;
	}
	pos = sieve_max+1;
	nstart = sieve[sieve_max] + 2;

	/* dumb algo, brute-force test all odd numbers against known primes */
	for (n=nstart; n<=max; n+=2)
	{
		for (j=1; ; j++)
		{
			int p = sieve[j];
			if (0 == n%p)
			{
				break;
			}
			if (p*p > n)
			{
				/* If we got to here, n must be prime; save it, move on. */
				sieve[pos] = n;
				pos ++;
				if (pos >= sieve_size)
				{
					sieve_size += 8192;
					sieve = (int *)realloc (sieve, sieve_size * sizeof (int));
				}
				break;
			}
		}
	}
	sieve_max = pos-1;

#if 0
	for (j=0; j<pos; j++)
	{
		printf ("its %d %d\n", j, sieve[j]);
	}
#endif
}

/* ====================================================== */

int moebius_mu (int n)
{
	if (1 >= n) return 1;
	if (3 >= n) return -1;
	
	INIT_PRIME_SIEVE(n);

	/* Implement the dumb/simple moebius algo */
	int cnt = 0;
	int i;
	for (i=0; ; i++)
	{
		int k = sieve[i];
		if (0 == n%k)
		{
			cnt ++;
			n /= k;

			/* If still divisible, its a square */
			if (0 == n%k) return 0;
		}

		if (1 == n) break;
		if (k*k > n) 
		{
			cnt ++;
			break;
		}
	}

	if (0 == cnt%2) return 1;
	return -1;
}

/* ====================================================== */

long double mangoldt_lambda (int n)
{
	if (1 >= n) return 0;
	
	INIT_PRIME_SIEVE(n);

	/* Implement the dumb/simple factorization algo */
	int i;
	for (i=0; ; i++)
	{
		int k = sieve[i];
		if (0 == n%k)
		{
			n /= k;
			while (0 == n%k) n /= k;

			if (1 == n) return logl ((long double)k);
			return 0.0L;
		}
		if (k*k > n) return logl ((long double) n);
	}

	return 0;
}

/* ====================================================== */

long double mangoldt_lambda_cached (int n)
{
	DECLARE_LD_CACHE (mangoldt_cache);
	if(ld_one_d_cache_check (&mangoldt_cache, n))
	{
		return ld_one_d_cache_fetch(&mangoldt_cache, n);
	}
	else
	{
		long double val = mangoldt_lambda(n);
		ld_one_d_cache_store (&mangoldt_cache, val, n);
		return val;
	}
}

/* ====================================================== */

int mertens_m (int n)
{
	int i;
	int acc = 0;
	for (i=1; i<=n; i++)
	{
		acc += moebius_mu (i);
	}
	return acc;
}

/* ====================================================== */
/* count the number of prime factors of n */

int liouville_omega (int n)
{
	int i;
	int acc = 2;

	if (1 >= n) return 1;
	if (2 >= n) return 2;

	INIT_PRIME_SIEVE(n);

	i=0;
	while (1)
	{
		int d = sieve[i];
		if (0 == n%d)
		{
			acc ++;
			n /= d;
		}
		else
		{
			i++;
		}
		if (d*d > n) break;
	}

	return acc;
}

int liouville_lambda (int n)
{
	int omega = liouville_omega (n);

	if (0 == omega%2) return 1;
	return -1;
}

/* ====================================================== */

// #define TEST 1
#ifdef TEST

int test_moebius(void)
{
	int n;

	int have_error = 0;
	int nmax = 40000;
	for (n=1; n<nmax; n++)
	{
		/* Perform a Dirichlet sum */
		int sum = 0;
		int d;
		for (d=1; ; d++)
		{
			if (2*d > n) break;
			if (n%d) continue;
			sum += moebius_mu (d);
			// printf ("%d divides %d and sum=%d\n", d, n, sum);
		}
		if (1 != n) sum += moebius_mu (n);
		if (0 != sum)
		{
			printf ("ERROR for moebius mu at n=%d sum=%d \n", n, sum);
			have_error ++;
		}
	}
	if (0 == have_error)
	{
		printf ("PASS: tested moebius function w/ dirichlet up to %d\n", nmax);
	}
	return have_error;
}

int test_omega(void)
{
	int n;

	int have_error = 0;
	int nmax = 40000;
	for (n=1; n<nmax; n++)
	{
		/* Perform a Dirichlet sum */
		int sum = 0;
		int d;
		for (d=1; ; d++)
		{
			if (2*d > n) break;
			if (n%d) continue;
			sum += liouville_lambda (d);
			// printf ("%d divides %d and sum=%d\n", d, n, sum);
		}
		if (1 != n) sum += liouville_lambda (n);
		
		if (0 == sum) continue;

		int ns = sqrt (n);
		if (ns*ns != n)
		{
			printf ("ERROR at liouville lambda at n=%d sum=%d \n", n, sum);
			have_error ++;
		}
	}
	if (0 == have_error)
	{
		printf ("PASS: tested liouville omega function w/ dirichlet up to %d\n", nmax);
	}
	return have_error;
}

int main()
{
	test_omega ();
	test_moebius ();
}
#endif
