/**
 * stirling.c
 *
 * Stirling numbers of the first and second kind
 *
 * Linas Vepstas <linasvepstas@gmail.com> Dec 2014
 */

#include <assert.h>

#include <math.h>
#include <stdlib.h>

#include "stirling.h"

long long int stirling_first (int n, int k)
{
	if (0 == n && 0 == k) return 1LL;
	if (0 == n && 0 < k) return 0LL;
	if (0 == k && 0 < n) return 0LL;
	if (n == k) return 1LL;

	assert(0 < k && 0 < n && k <= n);
	return ((long int) (1-n)) * stirling_first(n-1, k) + stirling_first(n-1, k-1);
}

long long int stirling_second (int n, int k)
{
	if (0 == n && 0 == k) return 1LL;
	if (0 == n && 0 < k) return 0LL;
	if (0 == k && 0 < n) return 0LL;
	if (1 == k) return 1LL;
	if (k == n) return 1LL;

	assert(0 < k && 0 < n && k <= n);
	return ((long int) (k)) * stirling_second(n-1, k) + stirling_second(n-1, k-1);
}


// #define TEST 1
#if TEST

#include <stdio.h>

int main()
{
	int n,k;
	for (n=0; n<10; n++) {
		for (k=0; k<=n; k++) {
			// printf("First: %d %d %lld\n", n, k, stirling_first(n,k));
			printf("Second: %d %d %lld\n", n, k, stirling_second(n,k));
		}
		printf("---\n");
	}


	return 0;
}

#endif
