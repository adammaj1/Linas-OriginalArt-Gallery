/*
 * eigen.c
 *
 * Obtain numeric eigenvectors for misc Redheffer-derived matrixes.
 *
 * Linas Vepstas February 2019
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>

double* divisor_matrix(int dim)
{
	double* matrix = (double*) malloc(dim*dim*sizeof(double));

	int idx = 0;
	for (int i=0; i< dim; i++)
	{
		for (int j=0; j< dim; j++)
		{
			matrix[idx] = 0.0;
			if ((j+1)%(i+1) == 0) matrix[idx] = 1.0;
			idx++;
		}
	}
	return matrix;
}

double* lapla_matrix(int dim)
{
	double* matrix = (double*) malloc(dim*dim*sizeof(double));

	int idx = 0;
	for (int i=0; i< dim; i++)
	{
		for (int j=0; j< dim; j++)
		{
			matrix[idx] = 0.0;
			if ((j+1)%(i+1) == 0) matrix[idx] = 2.0;
			if ((j+i+1)%(i+1) == 0) matrix[idx] = -1.0;
			if ((j+2)%(i+1) == 0) matrix[idx] = -1.0;
			idx++;
		}
	}
	return matrix;
}


/*
 * Get some eigenvectors
 */
void get_eigenvalues(int dim)
{
	double* matrix = lapla_matrix(dim);

	// Magic incantation to diagonalize the matrix.
	gsl_matrix_view m = gsl_matrix_view_array (matrix, dim, dim);
	gsl_vector_complex *eval = gsl_vector_complex_alloc (dim);
	gsl_matrix_complex *evec = gsl_matrix_complex_alloc (dim, dim);
	gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc (dim);
	gsl_eigen_nonsymmv (&m.matrix, eval, evec, w);
	gsl_eigen_nonsymmv_free (w);


	/* Get the eigenvector for the largest eigenvalue, which
	 * should be 1.0.  Graph it to make sure it matches what
	 * we already know it should be.
	 */
	gsl_eigen_nonsymmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_DESC);

	gsl_complex eval_0 = gsl_vector_complex_get (eval, 0);

	double x = GSL_REAL(eval_0);
	double y = GSL_IMAG(eval_0);
	double mag = sqrt(x*x+y*y);
	printf ("# eigenvalue = %20.18g + i %20.18g Magnitude=%g\n#\n", x, y, mag);

#if 0
	/* Get the eigenvector */
	gsl_vector_complex_view evec_0 = gsl_matrix_complex_column (evec, 0);

	gsl_complex z = gsl_vector_complex_get(&evec_0.vector, 0);
	double sgn = GSL_REAL(z);
	if (0 < sgn) sgn = 1.0; else sgn = -1.0;

	mag = 0.0;
	for (int j=0; j< mxi; j++)
	{
		gsl_complex z = gsl_vector_complex_get(&evec_0.vector, j);
		// printf("# %d	%g	%g\n", j, GSL_REAL(z), GSL_IMAG(z));
		double re = sgn * GSL_REAL(z);
		mag += re * re;
		vec[j] = re;
	}
	mag = sqrt(mag);
	printf("# Vector length = %20.18g\n#\n", mag);
#endif
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s dim\n", argv[0]);
		exit(1);
	}
	int dim = atoi(argv[1]);
	printf("#\n# dim=%d\n#\n", dim);

	get_eigenvalues(dim);

	return 0;
}
