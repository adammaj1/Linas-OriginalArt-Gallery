/*
 * fpcoef.c
 *
 * Numerical integration to obtain measure coefficients.
 * Plot dependence of coefficients w.r.t K
 *
 * Dec 2017
 * Feb 2018
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NOEMAIN
// #include "psi.c"
// #include "psibig.c"
#include "expan.c"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s npts niter\n", argv[0]);
		exit(1);
	}

	int npts = atoi(argv[1]);
	int niter = atoi(argv[2]);

	printf("#\n# npts = %d depth=%d\n#\n", npts, niter);

	int nkv=600;
	for (int i=0; i< nkv; i++)
	{
		double K = ((double) i + 0.5) / ((double) nkv);
		K = 0.5 + 0.5*K;

		find_midpoints(K, MAXN);
		// big_midpoints(K, 400, midpoints, MAXN);
		sequence_midpoints(K, MAXN);

		int siter = niter;
		if (K < 0.8) siter = 1.2*niter;
		if (K < 0.74) siter = 1.5*niter;
		if (K < 0.65) siter = 2*niter;
		if (K < 0.57) siter = 3*niter;
		if (K < 0.55) siter = 5*niter;
		double cf0 = alpha_n(K, 0, npts, siter);
		double cf1 = alpha_n(K, 1, npts, siter);
		printf ("%d %g	%g\n", i, 2.0*K, cf1/cf0);
		fflush(stdout);
	}
}
