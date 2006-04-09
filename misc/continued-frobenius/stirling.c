
/* 
 * stirling.c
 *
 * recursive stirling numbers of first and second kind 
 * 
 * Linas April 2006
 */

#include <stdio.h>
#include <stdlib.h>

#include "binomial.h"

unsigned int stirling_first (unsigned int n, unsigned int k)
{
	if (0==k)
	{
		if (0==n) return 1;
		return 0;
	}

	if (n<k) return 0;

	unsigned int s = stirling_first (n-1, k-1);
	if (n-1 >= k)
	{
		s += (n-1) * stirling_first (n-1, k);
	}

	return s;
}

int sb_sum (unsigned int n, unsigned int m)
{
	unsigned int k;
	int s, sg;
	
	sg = 1;
	if (n%2) sg = -1;

	s = 0;
	for (k=m; k<=n; k++)
	{
		s += sg * binomial (k,m) * stirling_first (n, k);
		sg = -sg;
	}

	return s;
}

int
main () 
{
	int n, k;

	for (n=0; n<10; n++)
	{
		for (k=0; k<=n; k++)
		{
			// unsigned int s = stirling_first (n,k);
			int s = sb_sum (n,k);
			printf ("duude (%d %d)  = %d\n", n, k, s);
		}
	}

	return 0;
}
