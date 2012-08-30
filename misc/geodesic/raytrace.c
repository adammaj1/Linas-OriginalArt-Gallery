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
 * The symbolic trajectory can be converted purely into S,T values, with
 * several hacky techniques.  It is tempting to apply  SL(2,Z) identities
 * such as (ST)^3=-I but this is wrong, since this identity does not preserve 
 * geodesics, but maps them in some crazy way...  Then strings of S,T can
 * be converted to binary either via several different encodings, including
 * dyadic encoding or via continued fractions.
 *
 * Linas Vepstas August 2012
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DBG(X,...)
#define DBG(X,ARGS...) printf(X, ARGS)

typedef int bool;

typedef struct 
{
	long double x;   // position
	long double y;
	long double vx;  // velocity
	long double vy;
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
	static long double rho = 0.5L * sqrtl(3.0L);

	// Calculate center of circle.  Its at y=0, x=center.
	long double center = in.x + in.y * in.vy / in.vx;
	DBG("--------\ncenter at %Lg\n", center);
	// Square of the radius of the circle
	long double radius_sq = (in.x - center)*(in.x-center) + in.y*in.y;

	// exit_code == 'S' means bottom exit
	// exit_code == 'T' means right exit
	// exit_code == 'N' means left exit
	char exit_code = 'S';
	long double x_exit = (1.0L - radius_sq + center*center) / (2.0L * center);

	// intersect is true if the geodesic and the unit circle interest.
	// If intersect, then can take sqrt to get valid intersection point.
	bool intersect = (x_exit-center)*(x_exit-center) < radius_sq;

	// bottom_entry is true, if the trajectory came in through the bottom.
	// bool bottom_entry = (-0.5 < in.x) && (in.x < 0.5);
	// long double rinsq = in.x*in.x + in.y*in.y - 1.0;
	// bool bottom_entry = (rinsq < 1.0e-14);
	bool bottom_entry = (in.code == 'S');

	// If geodesic and unit circle intersect, then it can still be
	// a side exit if the intersection point is out of bounds.
	bool side_exit = (x_exit < -0.5L) || (0.5L < x_exit);

	DBG(" code=%c sect=%d boten=%d sidex=%d xex=%Lg\n", in.code, intersect, bottom_entry, side_exit, x_exit);

	// First time through is a special case...
	if (('F' == in.code) && intersect)
	{
		// Repeat some of the exit calculations below, and see if it would
		// have gone out through the side. 
		long double tmp = -0.5L;
		if (in.vx > 0.0L) tmp = 0.5L;
		long double yex = sqrtl(radius_sq - (tmp-center)*(tmp-center));
#define FALSE 0
#define TRUE 1
		if (yex >= rho) side_exit = TRUE;
		else side_exit = FALSE;
	}

	if (!intersect || (intersect && (bottom_entry||side_exit)))
	{
		if (in.vx > 0.0L)
		{
			// Right side exit, not bottom exit
			x_exit = 0.5L;
			exit_code = 'T';
		}
		else 
		{
			// Left side exit, not bottom exit
			x_exit = -0.5L;
			exit_code = 'N';
		}
	}

	long double y_exit = sqrtl(radius_sq - (x_exit-center)*(x_exit-center));
	DBG(" %c exit x=%Lg y=%Lg\n", exit_code, x_exit, y_exit);
	if (('S' != exit_code) && (y_exit < rho))
	{
		fprintf(stderr, "Error: bad y value = %Lg for side exit\n", y_exit);
		exit(1);
	}

	long double tan_exit = -(x_exit-center)/y_exit;
	long double vx_exit = sqrtl (1.0L / (1.0L + tan_exit*tan_exit));
	long double vy_exit = sqrtl (1.0L - vx_exit*vx_exit);
	// Copy the sign bit to the velocity.
	if (tan_exit < 0.0L) vy_exit = -vy_exit;

	// If incoming velocity was left-pointing, then so is the outgoing
	// velocity.  exit code should be N or S.
	if (in.vx < 0.0L)
	{
		vx_exit = -vx_exit;
		vy_exit = -vy_exit;
		if ('T' == exit_code)
		{
			fprintf(stderr, "Bad exit code for velocity\n");
			exit(1);
		}
	}

	DBG("velc=%Lg %Lg %Lg\n", vx_exit, vy_exit, vx_exit*vx_exit+vy_exit*vy_exit);

	// Now bounce the velocity vector, based on which wall it is going
	// through.
	switch (exit_code)
	{
		case 'T':
			out->x = -0.5L;
			out->y = y_exit;

			out->vx = vx_exit;
			out->vy = vy_exit;
			break;
		case 'N':
			out->x = 0.5L;
			out->y = y_exit;

			out->vx = vx_exit;
			out->vy = vy_exit;
			break;
		case 'S':
		{
			out->x = -x_exit;
			out->y = y_exit;

			long double deno = 2.0L * center * vx_exit;
			out->vx = -vx_exit + x_exit * deno;
			out->vy = vy_exit - y_exit * deno;
			break;
		}
	}
	out->code = exit_code;

	DBG("final velc=%Lg %Lg %Lg\n", out->vx, out->vy, out->vx*out->vx+out->vy*out->vy);
	return exit_code;
}

#define SEQLEN 80
typedef struct 
{
	ray_t	tangent; // tangent vector at start point
	char raw_seq[SEQLEN+1];  // three-letter symbolic dynamics
	char seq[5*SEQLEN+1];  // two-letter symbolic dynamics
	char lr[5*SEQLEN];     // two-letter left-right sequence
} geodesic_t;

/**
 * sequence -- generate symbolic dynamics for a geodesic
 * The geodesic is specified by the tangent vector 'ray'.
 * The resulting exit codes for the fundamental domain, the
 * three letters S T and N are stored as strings in raw_seq.
 */
void sequence(ray_t in, geodesic_t* geo)
{
	in.code = 'F'; // Sanity check
	geo->tangent = in;

	DBG("==========\nvelostart %Lg %Lg\n", in.vx, in.vy);
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

/**
 * two_letter --  compute the two-letter sequence, given the raw seq.
 *
 * The idea of doing group-element equivalence is basically a bad
 * idea for geodesics; although its a diffeomorphism on the domain,
 * it is NOT an equivalence for the geodesics; it completely
 * scrambles them.
 */
void two_letter(geodesic_t *geo)
{
	int i;
	int k=0;
	for (i=0; 0x0 != geo->raw_seq[i]; i++)
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

/**
 * left_right --  compute the LR sequence, given the ST seq
 * Basically, L=TST and R = T  The algo here is probably buggy.
 * The idea of doing group-element equivalence is basically a bad
 * idea for geodesics; although its a diffeomorphism on the domain,
 * it is NOT an equivalence for the geodesics; it completely
 * scrambles them.
 */
void left_right(geodesic_t *geo)
{
	int i=0;
	int k=0;
	while(1)
	{
		if ((0 == strncmp(&geo->seq[i], "STS", 3)) ||
		    (0 == strncmp(&geo->seq[i], "TST", 3)))
		{
			geo->lr[k] = 'L';
			k++;
			i+=3;
		}
		else
		{
			geo->lr[k] = 'R';
			k++;
			i++;
		}
		if (geo->seq[i] == 0) break;
	}
	geo->lr[k] = 0;
}

/**
 * two_eliminate -- identity elimination in geodesic strings
 * For the modular group, we have S^2 = -I
 *
 * The idea of doing group-element equivalence is basically a bad
 * idea for geodesics; although its a diffeomorphism on the domain,
 * it is NOT an equivalence for the geodesics; it completely
 * scrambles them.
 */
int eliminate_two(geodesic_t *geo)
{
	int j=0;
	int k=0;
	int cnt = 0;
	while(1)
	{
		while (0 == strncmp(&geo->seq[k], "SS", 2)) { k+=2; cnt++; }
		if (0x0 == geo->seq[k]) break;
		geo->seq[j] = geo->seq[k];
		j++;
		k++;
		if (0x0 == geo->seq[k]) break;
	}
	geo->seq[j] = 0x0;

	DBG("Eliminated %d doubles\n", cnt);
	return cnt;
}

/**
 * eliminate -- identity elimination in geodesic strings
 * For the modular group, we have (ST)^3 = -I
 * The idea of doing group-element equivalence is basically a bad
 * idea for geodesics; although its a diffeomorphism on the domain,
 * it is NOT an equivalence for the geodesics; it completely
 * scrambles them.
 */
int eliminate_triple(geodesic_t *geo)
{
	int j=0;
	int k=0;
	int cnt = 0;
	while(1)
	{
		while (0 == strncmp(&geo->seq[k], "STSTST", 6)) { k+=6; cnt ++; }
		while (0 == strncmp(&geo->seq[k], "TSTSTS", 6)) { k+=6; cnt++; }
		if (0x0 == geo->seq[k]) break;
		geo->seq[j] = geo->seq[k];
		j++;
		k++;
		if (0x0 == geo->seq[k]) break;
	}
	geo->seq[j] = 0x0;
	return cnt;
}

/**
 * eliminate -- identity elimination in geodesic strings
 * For the modular group, we have (ST)^3 = -I
 * The idea of doing group-element equivalence is basically a bad
 * idea for geodesics; although its a diffeomorphism on the domain,
 * it is NOT an equivalence for the geodesics; it completely
 * scrambles them.
 */
int eliminate_five(geodesic_t *geo)
{
	int j=0;
	int k=0;
	int cnt = 0;
	while(1)
	{
		while (0 == strncmp(&geo->seq[k], "TSTST", 5))
		{ 
			k += 5;
			geo->seq[j] = 'S';
			j++;
			cnt++;
		}
		if (0x0 == geo->seq[k]) break;
		geo->seq[j] = geo->seq[k];
		j++;
		k++;
		if (0x0 == geo->seq[k]) break;
	}
	geo->seq[j] = 0x0;

	eliminate_two(geo);
	return cnt;
}

/**
 * decode_helper -- convert geodesic into real number.
 * This is a binary decoding, not a continued fraction decoding.
 */
double decode_helper(char * seq)
{
	double result = 0.0;
	double shift = 0.5;
	bool doit = TRUE;
	int k;
	for (k=0; 0x0 != seq[k]; k++)
	{
		if ('S' == seq[k])
		{
			doit = !doit;
		}
		else
		{
			if (doit) result += shift;
			shift *= 0.5;
		}
	}
	return result;
}

/**
 * decode_helper -- convert geodesic into real number.
 * This is a binary decoding, not a continued fraction decoding.
 */
double decode(geodesic_t *geo)
{
	return decode_helper(geo->seq);
}

double decode_raw(geodesic_t *geo)
{
	return decode_helper(geo->raw_seq);
}

/* Simple LR to binary decoder */
double decode_bin(geodesic_t *geo)
{
	double result = 0.0;
	double shift = 0.5;
	char * seq = geo->lr;
	int k;
	for (k=0; 0x0 != seq[k]; k++)
	{
		if ('R' == seq[k])
		{
			result += shift;
		}
		shift *= 0.5;
	}
	return result;
}

/*
 * decode_frac -- convert geodesic into real number.
 * This is a continued-fraction decoding.
 */
double decode_frac_helper(char * seq)
{
	int cnt = 0;
	int k;
	for (k=0; 0x0 != seq[k]; k++)
	{
		if ('S' == seq[k])
		{
			double rv = decode_frac_helper(&seq[k+1]);
			if (0 == cnt) cnt = 1;
			DBG("val=%g seq=%s\n", 1.0 / (cnt + rv), seq);
			return 1.0 / (cnt + rv);
		}
		else
		{
			cnt ++;
		}
	}
	if (cnt < 0.5) cnt = 1.0;
	DBG("val=%g seq=%s\n", 1.0 / ((double) cnt), seq);
	return 1.0 / ((double) cnt);
}

double decode_frac(geodesic_t *geo)
{
	return decode_frac_helper(geo->seq);
}

double decode_frac_raw(geodesic_t *geo)
{
	return decode_frac_helper(geo->raw_seq);
}

/*
 * spray -- generate symbolic dynamics for various geodesics
 *
 * Right now, its a geodesic spray starting at (0,2)
 * Delta is the step size.
 */
void spray(long double inx, long double iny, long double delta)
{
	printf("#\n# Symbolic dynamics on Fundamental Domain of SL(2,Z)\n");
	printf("#\n# ==========origin is %Lg %Lg\n#\n", inx, iny);
	ray_t in;
	geodesic_t geo;
	long double theta;
	long double offset = 0.5*M_PI;
	for (theta = 0.5L*delta + offset; theta<2.0L*M_PI+offset; theta += delta)
	{
		in.x = inx;
		in.y = iny;
		in.vx = cosl(theta);
		in.vy = sinl(theta);	
		in.code = 'F';

		sequence (in, &geo);
		two_letter(&geo);
		left_right(&geo);
#if 0
		eliminate_two(&geo);
		eliminate_triple(&geo);
		eliminate_five(&geo);
		eliminate_triple(&geo);
#endif
		double dyadic = decode(&geo);
		double dyadic_raw = decode_raw(&geo);
		double contin = decode_frac(&geo);
		double contin_raw = decode_frac_raw(&geo);
		double bin = decode_bin(&geo);
		DBG("theta=%Lg seq=%s two=%s\n", theta, geo.raw_seq, geo.seq);
		DBG("theta=%Lg res=%g\n", theta, dyadic);
		// printf("theta=%g res=%g seq=%s\n", theta, dyadic, geo.seq);

		// OK, now compute the geodesic going in the opposite direction.
		in.x = inx;
		in.y = iny;
		in.vx = cosl(theta+M_PI);
		in.vy = sinl(theta+M_PI);	
		in.code = 'F';

		sequence (in, &geo);
		two_letter(&geo);
		// eliminate_two(&geo);
		double back = decode(&geo);
		printf("%Lg	%g	%g	%g	%g	%g	%g\n", (theta-offset)/(2.0*M_PI), dyadic, dyadic_raw, contin, contin_raw, bin, back);
	}
}

int main(int argc, char * argv[]) 
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <phi> <Dy>\n", argv[0]);
		exit(1);
	}
	long double ia = strtold(argv[1], NULL);
	long double ib = strtold(argv[2], NULL);

	long double inx = sinl(ia*M_PI);
	long double iny = cosl(ia*M_PI) + ib;

	spray(inx, iny, 0.003);
}

