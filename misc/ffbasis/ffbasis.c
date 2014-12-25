/**
 * ffbasis.c
 *
 * Fallig factorial basis for the divisor function
 *
 * Linas Vepstas Decmber 2014
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "binomial.h"
#include "cache.h"

/**
 * The operator that transforms binomial (falling factorial) into a
 * Dirichlet series.
 *
 * We assume m>1 and that k >=0
 */
double E_mk(int m, int k)
{
	double em = m;
	em = (1.0 - em) / em;
	em = pow(em, ((double) k));
	return em;
}

/**
 * A vector in the kernel of E aka a_k
 * If we did this correctly, a_k is given by solving
 *    sum_k=0^infty a_k x^k = sin (2pi/(1+x))
 * This returns a_k.
 */
long double a_k(unsigned int k)
{
	unsigned int n;
	long double sum = 0.0L;

	if (0 == k) return 0.0L;

	DECLARE_LD_CACHE(a_kc);
	if (ld_one_d_cache_check(&a_kc, k))
		return ld_one_d_cache_fetch(&a_kc, k);

// WTF ?? -std=gnu should be enough??
#define M_PIl    3.141592653589793238462643383279502884L

	long double fourpi = - 4.0L * M_PIl * M_PIl;
	long double numer = - 2.0L * M_PIl;
	long double fact = 1.0L;
	for (n=0; n<3530; n++)
	{
		long double term = numer;
		long double bino = binomial (2*n+k, 2*n);
		bino /= fact;

		term *= bino;
		sum += term;
		// printf("n=%d numer=%g  bino=%g term=%g sum=%g\n", n, numer, bino, term, sum);

		if (10 < n && fabs(term/sum) < 1.0e-35) break;

		numer *= fourpi;
		fact *= (2.0L * n + 3.0L)  * (2.0L*n + 2.0L);
	}
	// printf(" ---------------------- above was k=%d\n", k);

	if (k%2 == 0) sum = - sum;

	ld_one_d_cache_store(&a_kc, sum, k);
	return sum;
}

long double a_k_regulated(unsigned int k, unsigned int p)
{
	if (0 == k) return a_k(0);
	long double sum = 0.0L;
	for (unsigned int n=0; n<=p; n++)
	{
		if (k<n) break;
		sum += a_k(k-n) * binomial(p, n);
	}
	return sum;
}

double kern(int k)
{
	if (0 == k) return a_k(0);
	return a_k(k) + a_k(k-1);
}

/**
 * It should be the case that
 *   sum_k=0^infty a_k x^k = sin (2pi/(1+x))
 * and
 *   sum_k=0^infty a_k(p) x^k = (1+x)^p sin (2pi/(1+x))
 *
 * Is that really the case?  Yes, it is, to the
 * precision that double precision can do.
 * The below computes the sum, and returns it.
 * Seems to be accurate for -0.85 < x < 0.85 or so.
 */
double topsin(double x)
{
	int k;
	double sum = 0.0;
	double xn = 1.0;
	for(k=0; k<1200; k++)
	{
		double term = xn * a_k_regulated(k, 0);
		sum += term;
		if (k>10 && fabs(term) < 1.0e-10) break;
		xn *= x;
	}
	return sum;
}


/**
 * The inverse of E
 */
double Einv_km(int k, int m)
{
	int j;
	if (1 == m)
	{
		if (0 == k) return 1.0;
		return kern(k+1) / (2.0 * M_PI);
	}

	double sum = 0.0;

	double r = (1.0 - m) / ((double) m);
	double pj = 1.0;
	for (j=1; j<=k; j++)
	{
		double term = kern(j);
		pj *= r;
		sum += pj * term;
	}

	sum /= 2.0 * M_PI * pj * m * (m-1);
	// if (m%2 == 0) sum = -sum;

	return sum;
}

double right_inv(int m, int n)
{
	int k;
	double sum = 0;
	for (k=0; k<530; k++)
	{
		double emk = E_mk(m,k);
		double ei = Einv_km(k,n);
		double term = emk * ei;
		sum += term;

		// printf("right k=%d sum=%g term=%g emk=%g einvkn=%g\n", k, sum, term, emk, ei);
		double a_k = kern(k);
		printf("right k=%d emk=%8.6g  einvkn=%6.6g   ak=%8.6g\n", k, emk, ei, a_k);
		if (k>10 && fabs(term) < 1.0e-10) break;
	}

	return sum;
}

double left_inv(int k, int j)
{
	int m;
	double sum = 0;
	for (m=1; m<1630; m++)
	{
		double ei = Einv_km(k,m);
		double emj = E_mk(m,j);
		double term = ei * emj;
		sum += term;
		printf("left m=%d sum=%g term=%g eikm=%g emj=%g\n", m, sum, term, ei, emj);
		if (m>10 && fabs(term) < 1.0e-10) break;
	}


	return sum;
}

void chk_Eright(void)
{
	int m,n;

// right_inv(3,2);
// return;
right_inv(6,1);
return;

	for (m=1; m<8; m++)
	{
		for (n=1; n<8; n++)
		{
			double sum = right_inv (m, n);

			if (m == n && fabs (sum-1.0) > 1.0e-4)
			{
				printf("Bad diag at m=%d sum=%g\n", m, sum);
			}
			if (m != n && fabs(sum) > 1.0e-5)
			{
				printf("Bad off-diag at m=%d n=%d sum=%g\n", m, n, sum);
			}
		}
	}
}

void chk_Eleft(void)
{
	int k,j;

left_inv(0,1);
return;

	for (k=0; k<8; k++)
	{
		for (j=1; j<8; j++)
		{
			double sum = left_inv (k, j);

			if (k == j && fabs (sum-1.0) > 1.0e-4)
			{
				printf("Bad diag at k=%d sum=%g\n", k, sum);
			}
			if (k != j && fabs(sum) > 1.0e-5)
			{
				printf("Bad off-diag at k=%d j=%d sum=%g\n", k, j, sum);
			}
		}
	}
}

/**
 * Check that kern is actually in the kernel of E.
 * That is, we expect that
 *     sum_k=0^\infty E_mk a_k = 0 for all m.
 * .. and it is, although rounding errors get really nasty around m=8
 * i.e. relation holds up to m=8
 */
void check_kern(int m)
{
	int k;

	printf("============ m= %d ==============\n", m);
	double sum = 0.0;
	for (k=0; k<1550; k++)
	{
		double term = E_mk(m, k) * kern(k);
		sum += term;
		printf("k=%d term=%g sum=%g\n", k, term, sum);

		if (fabs(term) < 1.0e-10 && k>10) break;
	}
}

/**
 * Consider the binomial b(s,k) = s! / (s-k)! k! for s complex
 * and k integer.  Verify that the relation
 *   1/m^s = sum_k=0^\infty E_mk b(s,k)
 * holds.  (and indeed it soes seem to for all m and s.)
 * Arguments are s and m.
 * Print a failure message if the relation fails to hold.
 */
void chk_E(long double complex s, int m)
{
	int k;
	long double complex sum = 0.0;
	for (k=0; k<800; k++)
	{
		double emk = E_mk(m, k);
		long double complex bin = cbinomial(s, k);
		sum += emk * bin;
	}

	long double complex psi = cpowl(m, -s);

	long double complex diff = sum - psi;

	// printf("its m=%d sum=%lf vs psi=%lf \n", m, creal(sum), creal(psi));
	// printf("its m=%d diff= %Lg + i %Lg  \n", m, creall(diff), cimagl(diff));
	long double adiff = cabsl(diff);

	if (1.0e-6 < adiff)
		printf("Fail at s=%f +i %f m=%d diff= %Lg \n", creal(s), cimag(s), m, adiff);
}

/**
 * Consider the binomial b(s,k) = s! / (s-k)! k! for s complex
 * and k integer.  Verify that the relation
 *   b(s,k) = sum_m=1^\infty Einv_km (1/m^s)
 * holds.
 * Arguments are s and k.
 * Print a failure message if the relation fails to hold.
 * Currently fails badly.
 */
void chk_Einv(long double complex s, int k)
{
	int m;
	long double complex bin = cbinomial(s, k);

	long double complex sum = 0.0;
	for (m=1; m<100; m++)
	{
		double ei = Einv_km(k, m);
		long double complex psi = cpowl(m, -s);
		sum += ei * psi;
		printf("chk m=%d psir=%7.5g ei=%7.5g termr=%7.5g sumr=%g want=%g\n",
			m, creal(psi), ei, creal(ei*psi), creal(sum), creal(bin));
	}


	long double complex diff = sum - bin;

	// printf("its m=%d sum=%lf vs psi=%lf \n", m, creal(sum), creal(psi));
	// printf("its m=%d diff= %Lg + i %Lg  \n", m, creall(diff), cimagl(diff));
	long double adiff = cabsl(diff);

	if (1.0e-6 < adiff)
		printf("Fail at s=%f +i %f k=%d diff= %Lg \n", creal(s), cimag(s), k, adiff);
}
/**
 * call chk_E for a variety of points on the complex plane.
 */
void chk_E_everywhere(void)
{
	int m;
	double x,y;

#define MAX 12
	for (m=1; m<MAX ; m++)
	{
		for (x= - 5.0; x < 5.0; x +=0.30345) {
			for (y= - 5.0; y < 5.0; y += 0.4089345) {
				long double complex s = x + y *I;
				chk_E(s, m);
			}
				printf("."); fflush (stdout);
		}
		printf("\ndone with m=%d\n", m);
	}
}

/**
 * Check the well-known formula
 *   log(x) = sum_k=1^\infty x^k / k
 * (It shows up in E-related calculations.)
 */
void chk_log(double x)
{
	int k;
	double y = (x-1.0) / x;
	double yp = y;
	double sum = 0.0;
	for (k=1; k<153; k++)
	{
		sum += yp / k;
		yp *= y;
	}

	printf("x=%f log(x)=%f  diff=%g\n", x, log(x), log(x)-sum);
}

void chk_log_everywhere()
{
	double x;

	for (x= 0.1; x < 15.2; x +=0.1123) {
		chk_log(x);
	}
}


void print_kern(void)
{
	int k;

	printf("#\n# The kernel series a_k\n#\n");
	for (k=0; k<248; k++)
	{
		double ak = a_k(k);
		double ak1 = a_k_regulated(k, 1);
		double ak2 = a_k_regulated(k, 2);
		double ak3 = a_k_regulated(k, 3);
		printf("%d	%g	%g	%g	%g\n", k, ak, ak1, ak2, ak3);
	}
}

void print_topsin(void)
{
	double x;
	for (x=1.0; x > -1.0; x-= 0.003)
	{
		double e = sin(2.0*M_PI/(1.0+x));
		double t = topsin(x);
		double d = t-e;
		printf("%g	%g	%g	%g\n", x, e, t, d);
	}
}

void check_kern_everywhere(void)
{
	int m;
	for (m=1; m<10; m++)
	{
		check_kern(m);
	}
}

int
main (int argc, char * argv[])
{
	// print_kern();
	print_topsin();

	// chk_Eleft();

#if RIGHT_INV
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	right_inv(m,n);
#endif
#if LEFT_INV
	int k = atoi(argv[1]);
	int j = atoi(argv[2]);
	left_inv(k,j);
#endif
#if LEFT
	double sr = atof(argv[1]);
	double si = atof(argv[2]);
	int k = atoi(argv[3]);

	long double complex s =  sr + I* si;
	chk_Einv(s, k);
#endif

#if 0
	int m = atoi(argv[1]);
	check_kern(m);

#endif
	return 0;
}

