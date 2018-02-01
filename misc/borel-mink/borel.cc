
/**
 * borel.cc
 *
 * Borel transform, or Riesz–Herglotz transform on the Minkowski measure
 *
 * Linas Vepstas February 2018
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "question.h"

// Goddamnd fucked up C++ complex numbers
#define COMPLEX std::complex<double>

/**
 * The  Riesz–Herglotz transform on the minkowski measure.
 * `level` is the iteration level to recurse to.
 */
COMPLEX riesz_mink(COMPLEX z, int level)
{
	unsigned __int128 p, q, pm, qm, pmid, qmid;

	// Golden ratio
	double phi = 0.5 * (1.0 + sqrt(5.0));

	// Growth rate
	double grow = pow(0.5 * phi * phi, level) * 0.2 * phi * phi * phi;

	COMPLEX acc = 0.0;
	double quest = 0.0;

	double norm = 1.0 / (double)(1<<level);
	for (int i=0; i< (1<<level); i++)
	{
		stern_brocot_tree128(i, level, &p, &q);
		stern_brocot_tree128(i+1, level, &pm, &qm);

		// a and b are the upper and lower endpoints of the interval
		double a = ((double) p) / (double) q;
		double b = ((double) pm) / (double) qm;
		stern_brocot_tree128(2*i+1, level+1, &pmid, &qmid);

		// The midpoint of the interval.
		double y = ((double) pmid) / (double) qmid;

		// unsigned long det = pm * q - p * qm;

		// measure: the value of the measure at the midpoint.
		double measure = norm * q * qm;
		measure /= grow;

		// The integral of the masure: should be the question mark.
		// up to a scale factor
		quest += (b-a)* measure;

		// theta ranges -pi to pi
		double theta = (2.0*y - 1.0) * M_PI;
		COMPLEX it = I*theta;
		COMPLEX eit = exp(it);

		// The kernel
		COMPLEX krn = (eit + z) / (eit - z);

		acc += krn * measure * (b-a);

		printf("%d	%g	%g	%g	%g	%g\n", i, y, measure, quest, real(acc), imag(acc));
	}

	// Normalize
	acc /= quest;

	acc /= 2.0 * M_PI;

	return acc;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s level\n", argv[0]);
		exit(1);
	}

	int level = atoi(argv[1]);

	COMPLEX z = 0.0;
	COMPLEX g = riesz_mink(z, level);
	g *= 2.0*M_PI;

	printf("# its %g + i%g\n", real(g), imag(g));
}
