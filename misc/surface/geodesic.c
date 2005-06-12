
/*
 * geodesic.c
 *
 * Graphs of the lengths and energies of geodesics on the riemann 
 * surface/fundamental domain. For now,the symmetric orbits only.
 */

#include <math.h>

int get_n_of_rho (double rho)
{
	rho *= rho;
	rho = sqrt (rho-0.75);
	rho -= 0.5;
	rho = floor (rho);
	return (int) rho;
}

double geo_x (double rho)
{
	int n = get_n_of_rho (rho);

	double ex = rho*rho + n*(n+2);
	ex /= 2.0*(n+1);
	return ex;
}

double geo_length (double rho)
{
	int n = get_n_of_rho (rho);

	double nu = rho*rho + n*(n+2);
	double de = -nu;
	double cr = 2.0*rho*(n+1);
	nu += cr;
	de += cr;

	double len = log (nu/de);
	return len;
}

double geo_energy (double rho)
{
	double ex = geo_x (rho);
	double len = geo_length (rho);

	double eng = ex;
	eng /= rho*rho - ex*ex;
	eng += len / (2.0*rho);

	return eng;
}

main () 
{
	int i;

	int n = 1400;
	double rho = 1.0;
	double delta = 0.1;
	rho += delta;

	printf ("#\n# geodesic lengths\n#\n");
	printf ("# delta = %g\n#\n", delta);

	for (i=0; i<n; i++)
	{
		double len = geo_length (rho);
		double eng = geo_energy (rho);
		double ex = geo_x (rho);

		printf ("%d	%g	%g	%g\n", i, rho, len, eng);
		rho += delta;
	}
}
