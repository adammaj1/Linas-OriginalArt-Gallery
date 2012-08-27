/*
 * raytrace.c
 *
 * Explore geodesics on the fundamental domain of the modular group
 * by ray-tracing.  Arbitrary-precision version need to make sure we 
 * get the right number of digits.  Geodesics are circles with origin
 * on the imaginary axis.  The path that a geodesic takes is a double-
 * sides string of S, T, N values. These form a symbolic dynamics
 * encoding of the trajectory.  Ostensible interest is to gather insight
 * into connectin between Ornstein isomorphism theorem and the Cantor
 * set.
 *
 * The symbolic trajectory can be converted purely into S,T values, via
 * the SL(2,Z) identity (ST)^3=I.  Then strings of S,T can be converted
 * to binary either via dyadic encoding or via Minkowski ? encoding
 * (i.e. via continued fraction).
 *
 * Linas Vepstas August 2012
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DBG(X,...)
// #define DBG(X,ARGS...) printf(X, ARGS)

typedef int bool;

typedef struct 
{
	double x;   // position
	double y;
	double vx;  // velocity
	double vy;
	char code;
} ray_t;

/* 
 * bounce -- ray-trace a geodesic through the fundamental domain.
 *
 * Return S or T or N depending on whether the ray exited on the bottom,
 * or on the right, or on the left of the fundamental domain bounded by
 * two vertical lines from +/- 1/2 +i rho where rho= srt(2)/2
 * and a circular arc stretching between them, of radius r=1.
 *
 * The 'in' ray is the incoming ray.  The 'out' ray is the outgoing ray,
 * except that its already been 'reflected' at the boundary, such that
 * the unit geodesic velocity poits into the domain, i.e. so that it can
 * be used as 'in' without any further changes.
 */
char bounce (const ray_t in, ray_t* out)
{
	static double rho = 0.5 * sqrt(3.0);

	// Calculate center of circle.  Its at y=0, x=center.
	double center = in.x + in.y * in.vy / in.vx;
	DBG("--------\ncenter at %g\n", center);
	// Square of the radius of the circle
	double radius_sq = (in.x - center)*(in.x-center) + in.y*in.y;

	// exit_code == 'S' means bottom exit
	// exit_code == 'T' means right exit
	// exit_code == 'N' means left exit
	char exit_code = 'S';
	double x_exit = (1.0 - radius_sq + center*center) / (2.0 * center);

	// intersect is true if the geodesic and the unit circle interest.
	// If intersect, then can take sqrt to get valid intersection point.
	bool intersect = (x_exit-center)*(x_exit-center) < radius_sq;

	// bottom_entry is true, if the trajectory came in through the bottom.
	// bool bottom_entry = (-0.5 < in.x) && (in.x < 0.5);
	// double rinsq = in.x*in.x + in.y*in.y - 1.0;
	// bool bottom_entry = (rinsq < 1.0e-14);
	bool bottom_entry = (in.code == 'S');

	// If geodesic and unit circle intersect, then it can still be
	// a side exit if the intersection point is out of bounds.
	bool side_exit = (x_exit < -0.5) || (0.5 < x_exit);

	// First time through is a special case...
	if (('F' == in.code) && intersect && !side_exit && (0.0 < in.vy))
	{
#define TRUE 1
		bottom_entry = TRUE;
	}

	if (!intersect || (intersect && (bottom_entry||side_exit)))
	{
		if (in.vx > 0.0)
		{
			// Right side exit, not bottom exit
			x_exit = 0.5;
			exit_code = 'T';
		}
		else 
		{
			// Left side exit, not bottom exit
			x_exit = -0.5;
			exit_code = 'N';
		}
	}

	double y_exit = sqrt(radius_sq - (x_exit-center)*(x_exit-center));
	DBG(" %c exit x=%g y=%g\n", exit_code, x_exit, y_exit);
	if (('S' != exit_code) && (y_exit < rho))
	{
		fprintf(stderr, "Error: bad y value = %g for side exit\n", y_exit);
		exit(1);
	}

	double tan_exit = -(x_exit-center)/y_exit;
	double vx_exit = sqrt(1.0 / (1.0 + tan_exit*tan_exit));
	double vy_exit = sqrt (1.0 - vx_exit*vx_exit);
	// Copy the sign bit to the velocity.
	if (tan_exit < 0.0) vy_exit = -vy_exit;

	// If incoming velocity was left-pointing, then so is the outgoing
	// velocity.  exit code should be N or S.
	if (in.vx < 0.0)
	{
		vx_exit = -vx_exit;
		vy_exit = -vy_exit;
		if ('T' == exit_code)
		{
			fprintf(stderr, "Bad exit code for velocity\n");
			exit(1);
		}
	}

	DBG("velc=%g %g %g\n", vx_exit, vy_exit, vx_exit*vx_exit+vy_exit*vy_exit);

	// Now bounce the velocity vector, based on which wall it is going
	// through.
	switch (exit_code)
	{
		case 'T':
			out->x = -0.5;
			out->y = y_exit;

			out->vx = vx_exit;
			out->vy = vy_exit;
			break;
		case 'N':
			out->x = 0.5;
			out->y = y_exit;

			out->vx = vx_exit;
			out->vy = vy_exit;
			break;
		case 'S':
		{
			out->x = -x_exit;
			out->y = y_exit;

			double deno = 2.0 * center * vx_exit;
			out->vx = -vx_exit + x_exit * deno;
			out->vy = vy_exit - y_exit * deno;
			break;
		}
	}
	out->code = exit_code;

	DBG("final velc=%g %g %g\n", out->vx, out->vy, out->vx*out->vx+out->vy*out->vy);
	return exit_code;
}

#define SEQLEN 20
typedef struct 
{
	ray_t	tangent; // tangent vector at start point
	int seqlen;
	char raw_seq[SEQLEN+1];  // three-letter symbolic dynamics
	char seq[5*SEQLEN+1];  // two-letter symbolic dynamics
} geodesic_t;

/*
 * sequence -- generate symbolic dynamics for various geodesics
 *
 * Right now, its a geodesic spray starting at (0,2)
 */
void sequence(ray_t in, geodesic_t* geo)
{
	in.code = 'F'; // Sanity check
	geo->tangent = in;
	geo->seqlen = SEQLEN;

	DBG("==========\nstart %g %g\n", in.vx, in.vy);
	int i;
	for (i=0; i<SEQLEN; i++)
	{
		ray_t out;
		bounce(in, &out);
		in = out;
		geo->raw_seq[i] = out.code;
	}
	geo->raw_seq[SEQLEN] = 0;
}

/*
 * two_letter --  compute the two-letter sequence, given the raw seq.
 */
void two_letter(geodesic_t *geo)
{
	int i;
	int k=0;
	for (i=0; i<geo->seqlen; i++)
	{
		switch(geo->raw_seq[i])
		{
			case 'S':
			case 'T':
				geo->seq[k] = geo->raw_seq[i];
				k++;
				break;
			case 'N':
				if ((0==k) || ((0<k) && ('T' == geo->seq[k-1])))
				{
					geo->seq[k] = 'S';
					k++;
				}
				geo->seq[k] = 'T'; k++;
				geo->seq[k] = 'S'; k++;
				geo->seq[k] = 'T'; k++;
				geo->seq[k] = 'S'; k++;
				break;
		}
	}
	geo->seq[k] = 0;
}

/*
 * spray -- generate symbolic dynamics for various geodesics
 *
 * Right now, its a geodesic spray starting at (0,2)
 */
void spray()
{
	ray_t in;
	geodesic_t geo;
	double theta;
	double delta = 0.1;
	for (theta = 0.5*delta; theta<2.0*M_PI; theta += delta)
	{
		in.x = 0.0;
		in.y = 2.0;
		in.vx = cos(theta);
		in.vy = sin(theta);	
		in.code = 'F';

		DBG("==========\nstart %g %g\n", in.vx, in.vy);
		sequence (in, &geo);
		two_letter(&geo);
printf("theta=%g seq=%s two=%s\n", theta, geo.raw_seq, geo.seq);
	}
}

int main(int argc, char * argv[]) 
{
	spray();
}




