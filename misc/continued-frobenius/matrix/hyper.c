/*
 * Eigenvalue finding
 * Find eigenvalues of the simple hyperbolic
 * composition operator
 *
 * Call lapack from C code.
 *
 * Get Schur factorization,
 * G = OTO^ where O is orthogonal (O^ == O transpose)
 * T is triangular.
 *
 * Use LAPACK xGEEV routine
 * specifically, complex double precision:
 * ZGEEV
 *
 * Linas Vepstas December 2005
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lapack.h"

int 
getworkdim (int dim, 
        double *matrix,  /* double precision complex */
        double *eigenvalues,
		  double *left_vec, double *right_vec,
		  double *workspace_a, double *workspace_b)
{
	char jobvl = 'V';
	char jobvr = 'V';
	int workdim;
	int info;

	workdim = -1;

	zgeev_ (&jobvl, &jobvr, &dim, matrix, &dim, 
	       eigenvalues, 
			 left_vec, &dim, right_vec, &dim, 
			 workspace_a, &workdim, workspace_b, &info);

	return (int) (workspace_a[0]+0.5);
}

int 
get_complex_eigen (int dim, 
        double *matrix,  /* double precision complex */
        double *eigenvalues,
		  double *left_vec, double *right_vec,
		  int workdim, double *workspace_a, double *workspace_b)
{

	char jobvl = 'V';
	char jobvr = 'V';
	int info;

	zgeev_ (&jobvl, &jobvr, &dim, matrix, &dim, 
	       eigenvalues, 
			 left_vec, &dim, right_vec, &dim, 
			 workspace_a, &workdim, workspace_b, &info);

}

/* --------------------------------------------------- */

complex double in (int n)
{
	complex double ii = I;

	if (0 > n)
	{
		n = -n;
		ii = -I;
	}
	if (n%4 == 0) return 1.0;
	if (n%4 == 1) return 0.0 + ii;
	if (n%4 == 2) return -1.0;
	if (n%4 == 3) return 0.0 -ii;
	return 0.0;
}

complex double nin (int n)
{
	complex double ii = -I;

	if (0 > n)
	{
		n = -n;
		ii = I;
	}
	if (n%4 == 0) return 1.0;
	if (n%4 == 1) return 0.0 + ii;
	if (n%4 == 2) return -1.0;
	if (n%4 == 3) return 0.0 -ii;
	return 0.0;
}

complex double nn (int n)
{
	if (0 > n) n = -n;

	if (n%2 == 0) return 1.0;
	return -1.0;
}

complex double compose (int m, int n)
{
	complex double ret;

	if (m == n)
	{
		if (0 == m) return (complex double) 1.0;
		ret = 0.25 + (in(n) - 1.0)/ (0.0 + 3.0*M_PI*I*n);
		return ret;
	}
	if (m == 2*n)
	{
		ret = 0.25 + (nin(n) - 1.0)/ (0.0 + 6.0*M_PI*I*n);
		return ret;
	}
	if (2*m == n)
	{
		ret = 0.5 - (nin(n) - 1.0)/ (0.0 + 3.0*M_PI*I*n);
		return ret;
	}

	ret = 3.0 * nn(n+m) * (n-m) - in(m)*(n-2.0*m) - in(n) * (2.0*n-m);
	ret *= nn(m+n) * n;
	ret /= 2 * M_PI * I * (2*n-m)*(n-m)*(n-2.0*m);

	return ret;
}
  
/* --------------------------------------------------- */

int main (int argc, char * argv[]) 
{
	double *mat;
	double *val;
	double *lev;
	double *rev;
	double *work;
	int dim;
	int workdim;
	int i,j, k;
	
	dim = 9;
	int nc = - (dim-1)/2;

	printf ("#\n#\n");
	printf ("# Eigenvectors of hyperbolic composition \n");
	printf ("# Numerically solved to rank=%d\n", dim);
	printf ("#\n#\n");

	mat = (double *) malloc (2*dim*dim*sizeof (double));
	val = (double *) malloc (2*dim*sizeof (double));
	lev = (double *) malloc (2*dim*dim*sizeof (double));
	rev = (double *) malloc (2*dim*dim*sizeof (double));
	workdim = 8*dim*dim;
	work = (double *) malloc (workdim*sizeof (double));

	/* Insert values for the GKW operator at x=1 (y=1-x) */
	for (i=0; i<dim; i++)
	{
		for (j=0; j<dim; j++)
		{
			/* Note transposed matrix'ing for FORTRAN */
			complex double v = compose (i+nc,j+nc);
			mat[i+j*dim] = creal(v);
			mat[i+j*dim+1] = cimag(v);
			// printf ("duude m(%d %d) = %g + i %g \n", i+nc, j+nc, creal (v), cimag (v));
		}
	}


	int wd = getworkdim (dim, mat, val, lev, rev, work);
	printf ("# recommended dim=%d actual dim=%d\n#\n", wd, workdim);
	// workdim = wd;
	
#if 0
	work = (double *) realloc (work, workdim*sizeof (double));
	geteigen (dim, mat, ere, eim, lev, rev, workdim, work);

	/* ---------------------------------------------- */
	/* print the eigenvalues */
	for (i=0; i<dim; i++)
	{
		printf ("# eigen[%d]=%20.15g +i %g\n", i, ere[i], eim[i]);
	}
	printf ("\n\n");
	
	int prtdim = 6;
	for (i=0; i<prtdim; i++)
	{
		for (j=0; j<prtdim; j++)
		{
			printf ("# right %d'th eigenvector[%d]=%g (normalized=%g)\n", 
			            i,j, rev[j+i*dim], rev[j+i*dim]/rev[i*dim]);
		}
		printf ("#\n");
	}
	
	for (i=0; i<prtdim; i++)
	{
		for (j=0; j<prtdim; j++)
		{
			printf ("# left %d'th eigenvector[%d]=%g (normalized=%g)\n", 
			            i,j, lev[j+i*dim], lev[j+i*dim]/lev[i*dim]);
		}
		printf ("#\n");
	}
	
	/* ---------------------------------------------- */
	/* Verify i'th eigenvector */
	for (i=0; i<prtdim; i++)
	{
		/* The j'th element of the i'th eigenvector */
		for (j=0; j<prtdim; j++)
		{
			double sum = 0.0;
			for (k=0; k<dim; k++)
			{
				sum += ache_mp(j,k) * rev[k+i*dim];
			}
			sum /= rev[j+i*dim];
			printf ("# %d'th eigenvec validation [%d]=%g\n", i, j, sum);
		}
		printf ("#\n");
	}

	/* ---------------------------------------------- */
	/* Print graphable data */
	double y;
	for (y=1.0; y>=0.0; y-=0.02)
	{
		double x = 1.0-y;
		printf ("%g", x);
		// validate that the zeroth eignevec is 1/(1+x)
		// printf ("\t%g", 1.73205/(1.0+x));
		for (i=0; i<prtdim; i++)
		{
			double yn = 1.0;
			double sum = 0.0;
			double fact = 1.0;
			/* The j'th element of the i'th eigenvector */
			for (j=0; j<dim; j++)
			{
				// Factorial not needed, ache already has the factorial folded in.
				// See, for example, the derives of seroth eigenvec. 
				// sum += yn * rev[j+i*dim] / fact;
				sum += yn * rev[j+i*dim];
				// printf ("duuude j=%d fact=%g yn=%g\n", j, fact, yn);
				yn *= y;
				fact *= j+1;
			}
			printf("\t%g", -sum);
		}
		printf ("\n");
	}
#endif
}
