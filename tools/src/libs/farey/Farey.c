
/* 
 * FUNCTION:
 * Convert real numbers to Farey numbers, and back.
 * Coded in psuedo-OO style for easy conversion to C++
 *
 * HISTORY:
 * Linas Vepstas January 16 1994
 * Linas Added stuff April 1996
 */

#ifdef LINUX
#include <signal.h>
#endif /* LINUX */

#include "Farey.h"
#include <stdio.h>
#include <math.h>

#define REAL (self->real)
#define INTPART (self->intpart)
#define FRACPART (self->fracpart)
#define NUM (self->num)
#define DENOM (self->denom)
#define TINUED_FRAC (self->tinued_frac)
#define NTERMS (self->nterms)
#define CUTOFF (self->cutoff)

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

/* ------------------------------------------------------------ */
struct Farey * CreateFarey ()
{
   struct Farey * self;
   self = (struct Farey *) malloc (sizeof (struct Farey));
   CUTOFF = 0x7fffffff;

#ifdef LINUX
   /* block floating point exceptions */
   sigsetmask (0x180);
#endif /* LINUX */
   return (self);
}

/* ------------------------------------------------------------ */

void SetReal (struct Farey *self, double val) 
{
   RealToContinuedFraction (self, val);
}

/* ------------------------------------------------------------ */

double GetReal (struct Farey *self)
{
   ContinuedFractionToReal (self);
   return (REAL);
}

/* ------------------------------------------------------------ */

double GetFarey (struct Farey *self)
{
   return ContinuedFractionToFarey (self);
}

/* ------------------------------------------------------------ */

void RatioToContinuedFraction (struct Farey *self, int numer, int deno)
{
   double tmp;
   int i;
   unsigned int n, d, m;

   REAL = ((double) numer) / ((double) deno);
   INTPART = numer / deno;
   if (0.0 > REAL) INTPART--;
   numer -= deno * INTPART;
   NUM = numer;
   DENOM = deno;

   /* Next, compute the continued fraction of the ratio */
   for (i=0; i<32; i++) TINUED_FRAC[i] = 0;
   n = NUM;
   d = DENOM;
   if (n != 0) {
      for (i=0; i<32; i++) {
         m = d/n;     
         TINUED_FRAC [i] = m;
         /* printf (" term %d  denom = %d num = %d termval = %d ",i, d, n, m); */
         d -= m*n;
         /* printf ("rem = %d \n", d); */
         m = d;
         d = n;
         n = m;
   
         /* if (d/32>n)  */
         if (d/CUTOFF>n) {
            n = 0;   /* clamp for "virtually zero" */
            if (TINUED_FRAC [i] == 1) {
               if (i != 0) {
                  TINUED_FRAC [i] = 0;
                  TINUED_FRAC [i-1] ++;
                  i--;
               } else {
                  INTPART ++;
               }
            }
         }
   
         /* Check for termination of the expansion, and 
          * make sure that the continued fraction always has 
          * an even number of terms */
         if (n == 0) {
            if (i%2 == 0) {
               TINUED_FRAC [i] -= 1;
               TINUED_FRAC [i+1] = 1;
            }
            break;
         }
      }
   }

   /* lets count the number of terms */
   for (i=0; i<32; i++) if (TINUED_FRAC[i] == 0) break;
   NTERMS = i;
}

/* ------------------------------------------------------------ */

void RealToContinuedFraction (struct Farey *self, double val)
{
   double tmp;
   unsigned int n, d, m;

   REAL = val;
   /* first, compute the integer and fractional parts of the real 
    * number x. Next, express the fractional part as a ratio of integers. */
   INTPART = (int) val;
   if (0.0 > val) INTPART--;
   FRACPART = REAL - (double) INTPART;
   d = 0x7fffffff;
   tmp = (double) d;
   tmp *= FRACPART;
   n = (int) tmp;

   /* avoid rounding problems for floating point numbers */
   CUTOFF = 54321;
   RatioToContinuedFraction (self, n, d);
   CUTOFF = 0x7fffffff;

   INTPART = (int) val;
   if (0.0 > val) INTPART--;
}

/* ------------------------------------------------------------ */

#ifdef ANSI_C
void PrintContinuedFraction (struct Farey *self)
#else
void PrintContinuedFraction (self)
struct Farey *self;
#endif
{
   int i;
   printf (" ratio %d over %d is continued fraction of %d terms\n", NUM, DENOM, NTERMS);
   for (i=0; i<NTERMS; i++) { 
      printf (" term %d is %d \n", i, TINUED_FRAC[i]);
   }
}

/* ------------------------------------------------------------ */

#ifdef ANSI_C
double ContinuedFractionTerms (struct Farey *self)
#else
double ContinuedFractionTerms (self)
struct Farey *self;
#endif
{
   return ((double) NTERMS);
}

/* ------------------------------------------------------------ */
/* compute Farey Number from continued fraction.
 * Algorithm used here does base2 arithmetic by bit-shifting. 
 * For generalized algorithm, see below. 
 */

#ifdef ANSI_C
double ContinuedFractionToFarey (struct Farey *self)
#else
double ContinuedFractionToFarey (self)
struct Farey *self;
#endif
{
   double tmp;
   int first_term;
   int i, j, k;
   unsigned int f, o;

   /* twidle the first bit */
   if (NUM != 0) {
      first_term = TINUED_FRAC[0];
      TINUED_FRAC[0] -= 1;
   }
   
   /* 
   printf ("\n\n"); 
   for (i=0; i<5; i++) { 
      printf (" term %d is %d \n", i, TINUED_FRAC[i]);
   }
   printf ("\n\n");
   */

   /* now, build the binary rep of the Farey number */
   f = 0;
   o = 0x80000000;
   j = 0;
   for (i=0; i<32;) {
      if (j%2==0) {
         o >>= TINUED_FRAC[j];
         /* printf (" tin = %d, o= 0x%x \n", TINUED_FRAC[j], o); */
      } else {
         for (k=0; k<TINUED_FRAC[j]; k++) {
            f |= o;
            o >>= 1;
         }
      }
      /* printf (" tin = %d f = 0x%x \n", TINUED_FRAC[j], f); */
      i += TINUED_FRAC[j];
      j++;
      if (TINUED_FRAC[j] == 0) break;
   }

   /* finally, convert the farey number to floating point rep */
   tmp = 1.0 / (double) 0x7fffffff;
   tmp *= 0.5;   /* since 0xffffffff hangs up on minus sign */
   tmp *= (double) f;

   tmp += (double) INTPART;

   /* reset the first term */
   TINUED_FRAC[0] = first_term;

   return (tmp);
}

/* ------------------------------------------------------------ */
/* converts continued fraction back to a real number */

double ContinuedFractionToReal (struct Farey *self)
{
   int i;
   double tmp;

   tmp = (double) INTPART;
   REAL = tmp;
   if (NTERMS == 0) return (tmp);

   /* now, work backwards and reconstruct the fraction. */
   tmp = 1.0 / ((double) TINUED_FRAC[NTERMS-1]);
   for (i=NTERMS-2; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
      tmp = 1.0 / tmp;
   }

   REAL += tmp;
   return (REAL);

}

/* ------------------------------------------------------------ */
/* converts continued fraction into real number, with a numerator of z.
 */

double ContinuedFractionToZReal (struct Farey *self, double z)
{
   int i;
   double tmp;
   double znum;

   znum = (double) INTPART;
   if (NTERMS == 0) return (znum);

   /* now, work backwards and reconstruct the fraction. */
   tmp = z / ((double) TINUED_FRAC[NTERMS-1]);
   for (i=NTERMS-2; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
      /* not normally needed, will this help mystery crash? */
      if (tmp == 0.0) { tmp = 1.0e30; } else { tmp = z / tmp; }
   }

   /* get rid of last z, to normalize */
   tmp /= z;

   znum += tmp;
   return (znum);

}

/* ------------------------------------------------------------ */
/* converts continued fraction into real number, with a numerator of z.
 * inverts the whatever.
 */

double ContinuedFractionToInvZReal (struct Farey *self, double z)
{
   int i;
   double tmp;
   double znum;

   znum = (double) INTPART;
   if (NTERMS == 0) return (znum);

   /* now, work backwards and reconstruct the fraction. */
   tmp = z * ((double) (TINUED_FRAC[NTERMS-1]));
   for (i=NTERMS-2; i>=0; i--) {
      tmp += 1.0 / ((double) (TINUED_FRAC[i]));
      /* not normally needed, will this help mystery crash? */
      if (tmp == 0.0) { tmp = 1.0e30; } else { tmp = z / tmp; }
   }

   /* get rid of last z, to normalize */
   tmp /= z;

   znum += tmp;
   return (znum);

}

/* ------------------------------------------------------------ */
/* I've defined an e-real to be the result of the continued fraction
 * where each term is damped by an exponential. Read the code. */

double ContinuedFractionToEReal (struct Farey *self, double t)
{
   int i, n;
   double tmp;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
      tmp = exp (- ((double) i) * t) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an e-fraction to be the result of the continued fraction
 * where each term is damped by an exponential. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToEFraction (struct Farey *self, double t)
#else
double ContinuedFractionToEFraction (self, t)
struct Farey *self;
double t;
#endif
{
   int i, n;
   double tmp;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = exp (((double) (n-1)) * t);
   tmp *= ((double) TINUED_FRAC[n-1]);
   tmp = 1.0 / tmp;
   for (i=n-2; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i] * exp (((double) i) * t);
      tmp = 1.0 / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* the routine below computes the generalized farey number from the
 * continued fraction. To get the usual farey number, simply use t=ln2;
 * this gives same result as bit-shifting algorithm above.
 */

#ifdef ANSI_C
double ContinuedFractionToEFarey (struct Farey *self, double t)
#else
double ContinuedFractionToEFarey (self, t)
struct Farey *self;
double t;
#endif
{
   int i;
   double sgn, sum, tmp;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   tmp = 0.0;
   sum = -1.0;    /* initally, shift by one bit */
   sgn = 1.0;
   for (i=0; i<NTERMS; i++) {
      sum += (double) TINUED_FRAC[i];
      tmp += sgn * exp (-t*sum);
      sgn = - sgn;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* the routine below computes the generalized farey number from the
 * continued fraction. To get the usual farey number, simply use t=1;
 * this gives same result as bit-shifting algorithm above.
 *
 * The algorithm multiplies each term by g sub k (n) where
 * g sub k (n) = (1 + sin ((k+1)*t) / sin (k*t)) ** (-n)
 * If I computed this correctly, this function is continous at all rationals.
 * (Ooops. It not continuous at all rationals. It is continuous at
 * all rationals whose denominators are powers of two. Sigh. 
 */

#ifdef ANSI_C
double ContinuedFractionToSinFarey (struct Farey *self, double t)
#else
double ContinuedFractionToSinFarey (self, t)
struct Farey *self;
double t;
#endif
{
   int i;
   double sgn, sum, tmp;
   double retval;
   double broke;
   double ct, cti;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   tmp = 0.0;
   sum = -1.0;    /* initally, shift by one bit */
   sgn = 1.0;
   for (i=0; i<NTERMS; i++) {
      sum += (double) TINUED_FRAC[i];
      ct = 0.5 * (1.0 + cos (((double) i)*t));
      cti = 0.5 * (1.0 + cos (((double)(i+1))*t));
      broke = 1.0 + cti/ct;
      tmp += sgn * ct * pow (broke, -sum);
      sgn = - sgn;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* the routine below computes the generalized farey number from the
 * continued fraction. To get the usual farey number, simply use t=1;
 * this gives same result as bit-shifting algorithm above.
 *
 * The algorithm multiplies each term by g sub k (n) where
 * g sub k (n) = t**k / (1+t)**n
 * If I computed this correctly, this function is continous at all rationals.
 */

#ifdef ANSI_C
double ContinuedFractionToTFarey (struct Farey *self, double t)
#else
double ContinuedFractionToTFarey (self, t)
struct Farey *self;
double t;
#endif
{
   int i;
   double sgn, sum, tmp;
   double retval;
   double broke;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   tmp = 0.0;
   sum = -1.0;    /* initally, shift by one bit */
   sgn = 1.0;
   for (i=0; i<NTERMS; i++) {
      sum += (double) TINUED_FRAC[i];
      broke = sum * log (1.0 + t);          /* linux pow() is broken */
      if (600.0 < broke ) broke = 600.0;
      tmp += sgn * pow (t, ((double) i)) * exp (-broke);
      sgn = - sgn;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* the routine below returns the "gap" between generalized Farey Numbers
 */

#ifdef ANSI_C
double ContinuedFractionToEFareyGap (struct Farey *self, double t)
#else
double ContinuedFractionToEFareyGap (self, t)
struct Farey *self;
double t;
#endif
{
   int i;
   double tmp, sum;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   sum = -2.0;    /* initally, shift by one bit */
   for (i=0; i<NTERMS; i++) {
      sum += (double) TINUED_FRAC[i];
   }

   if (sum > 0.0) {
      tmp = exp (-t * sum);
   } else {
      tmp = 1.0;
   }
   tmp *= 1.0 - 2.0 * exp (-t);
   retval += tmp;
   return (retval);
}


/* ------------------------------------------------------------ */
/* I've defined an cos-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToCosReal (struct Farey *self, double omega)
#else
double ContinuedFractionToCosReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, n;
   double tmp;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
      tmp = cos (((double) i) * omega) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an bessel-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToSincReal (struct Farey *self, double omega)
#else
double ContinuedFractionToSincReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, n;
   double tmp;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
      tmp = j0 (((double) i) * omega) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an cos-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToCnReal (struct Farey *self, double omega)
#else
double ContinuedFractionToCnReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, n;
   double tmp;
   double fomega;
   double retval;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
/*
      fomega = ((double) i) * omega;
      fomega *= 2.0 * M_PI;
      fomega = 2.0 * M_PI * omega / tmp; 
*/
      fomega = 2.0 * M_PI * omega;
      tmp = (1.0 + cos (fomega)) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an cos-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToSnReal (struct Farey *self, double omega)
#else
double ContinuedFractionToSnReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, n;
   double tmp;
   double retval;
   double fomega;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {
      tmp += (double) TINUED_FRAC[i];
/*
      fomega = ((double) i) * omega;
      fomega *= 2.0 * M_PI;
      fomega = 2.0 * M_PI * omega / tmp; 
*/
      fomega = 2.0 * M_PI * omega;
      tmp = (1.0 + sin (fomega)) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an cos-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

static struct Farey *scratch = 0x0;

#ifdef ANSI_C
double ContinuedFractionToFCnReal (struct Farey *self, double omega)
#else
double ContinuedFractionToFCnReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, j, k, n;
   double tmp;
   double retval;
   double fomega;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   if (!scratch) scratch = CreateFarey ();

omega *= 2.0 * M_PI;
   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {

/*
      k = i;
*/
      k = 1;
      for (j=0; j<i; j++) k *= 3;

      SetReal (scratch, ((double) k) * omega);
      fomega = GetFarey (scratch);
/*
      fomega *= 2.0 * M_PI;
*/

      tmp += (double) TINUED_FRAC[i];
      tmp = (1.0 + cos (fomega)) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an cos-real to be the result of the continued fraction
 * where each term is damped by a cosine. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToFSnReal (struct Farey *self, double omega)
#else
double ContinuedFractionToFSnReal (self, omega)
struct Farey *self;
double omega;
#endif
{
   int i, j, k, n;
   double tmp;
   double retval;
   double fomega;

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   if (!scratch) scratch = CreateFarey ();

omega *= 2.0 * M_PI;
   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {

/*
      k = i;
*/
      k = 1;
      for (j=0; j<i; j++) k *= 3;

      SetReal (scratch, ((double) k) * omega);
      fomega = GetFarey (scratch);
/*
      fomega *= 2.0 * M_PI;
*/

      tmp += (double) TINUED_FRAC[i];
      tmp = (1.0 + sin (fomega)) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */

#ifdef ANSI_C
double ContinuedFractionToZCnReal (struct Farey *self, double omega, double zzz)
#else
double ContinuedFractionToZCnReal (self, omega, zzz)
struct Farey *self;
double omega, zzz;
#endif
{
   int i, j, k, n;
   double tmp;
   double retval;
   double fomega;
   double zees[101];

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   n = NTERMS;
   if (100 < n) n = 100;
   for (i=0; i<n; i++) {
      fomega = InvZReal (((double) i) * omega, zzz);
      fomega *= 2.0 * M_PI;
      zees[i] = fomega;
   }

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {

      tmp += (double) TINUED_FRAC[i];
      tmp = (1.0 + cos (zees[i])) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */

#ifdef ANSI_C
double ContinuedFractionToZSnReal (struct Farey *self, double omega, double zzz)
#else
double ContinuedFractionToZSnReal (self, omega, zzz)
struct Farey *self;
double omega, zzz;
#endif
{
   int i, j, k, n;
   double tmp;
   double retval;
   double fomega;
   double zees[101];

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   n = NTERMS;
   if (100 < n) n = 100;
   for (i=0; i<n; i++) {
      fomega = InvZReal (((double) i) * omega, zzz);
      fomega *= 2.0 * M_PI;
      zees[i] = fomega;
   }

   /* now, work backwards and reconstruct the fraction. */
   n = NTERMS;
   tmp = 0.0;
   for (i=n-1; i>=0; i--) {

      tmp += (double) TINUED_FRAC[i];
      tmp = (1.0 + sin (zees[i])) / tmp;
   }

   retval += tmp;
   return (retval);
}

/* ------------------------------------------------------------ */
/* I've defined an x-fraction to be the result of the continued fraction
 * where each term is whaever. Read the code. */

#ifdef ANSI_C
double ContinuedFractionToXFarey (struct Farey *self, double t)
#else
double ContinuedFractionToXFarey (self, t)
struct Farey *self;
double t;
#endif
{
   int i, n;
   double tmp;
   double retval;
   double logotoo;
   double cygnus;
   double sum;
/*
printf ("\n\n yooo \n");
*/

   retval = (double) INTPART;
   if (NTERMS == 0) return (retval);

   logotoo = - log (2.0);
   sum = -1.0;
   cygnus = 1.0;

   /* now, work forwards */
   n = NTERMS;
   for (i=0; i< n; i++) {
      sum += ((double) TINUED_FRAC[i]);
#ifdef BAD_STRAT
      if (i+1<n) {
         tmp = ((double) TINUED_FRAC[i+1]);
         if (tmp > 1.1) {
            tmp *= t * exp (tmp * logotoo);
            tmp = 1.0 - tmp;
         } else{
            tmp = 1.0;
         }
      } else { tmp = 1.0;}
#endif BAD_STRAT
      if (0 < i) {
         tmp = ((double) TINUED_FRAC[i-1]);
         if (tmp < 1.1) {
            tmp *= t * exp (((double) TINUED_FRAC[i]) * logotoo);
            tmp = 1.0 - tmp;
         } else{
            tmp = 1.0;
         }
      } else { tmp = 1.0;}
      retval += cygnus * tmp * exp (sum * logotoo);
      cygnus *= -1.0;
/*
printf (" its %i %i %f  %f %f \n", i, TINUED_FRAC[i], sum, tmp, retval);
*/
   }

   return (retval);
}

/* ------------------------------------------------------------ */
/* computes crazy sum of continued fractions
 */


double CFSum (struct Farey *self, struct Farey *other,
              double alpha, double beta, double gamma)
{
   int i;
   double eval;
   double znum;
   int minterms, maxterms;
   struct Farey *tmp;

   znum = alpha * (double) INTPART;
   znum += beta * ((double) (other->intpart));
   znum += gamma;

   minterms = MIN ((NTERMS), (other->nterms));
   maxterms = MAX ((NTERMS), (other->nterms));

   if (maxterms == 0) return (znum);

   /* let self point at the one with more terms */
   /* this is NOT an object-oriented move. HACK_ALERT */
   if (maxterms != NTERMS) {
      tmp = other;
      other = self;
      self = tmp;

      eval = alpha;
      alpha = beta;
      beta = eval;
   }

   /* now, work backwards and reconstruct the fraction. */
   eval = 0.0;
   for (i=NTERMS-1; i>=minterms; i--) {
      eval += alpha * ((double) TINUED_FRAC[i]) + gamma;
      /* not normally needed, will this help mystery crash? */
      if (eval == 0.0) { eval = 1.0e30; } else { eval = 1.0 / eval; }
   }

   for (i=minterms-1; i>=0; i--) {
      eval += alpha * ((double) TINUED_FRAC[i]) + beta * ((double) other->tinued_frac[i]) + gamma;
      /* not normally needed, will this help mystery crash? */
      if (eval == 0.0) { eval = 1.0e30; } else { eval = 1.0 / eval; }
   }

   znum += eval;
   return (znum);

}

/* ------------------------------------------------------------ */
/* computes crazy product of continued fractions
 */

double CFProd (struct Farey *self, struct Farey *other,
              double alpha, double beta)
{
   int i;
   double eval;
   double znum;
   int minterms, maxterms;
   struct Farey *tmp;

   znum = alpha * (double) INTPART;
   znum += beta * ((double) (other->intpart));

   minterms = MIN ((NTERMS), (other->nterms));

   if (minterms == 0) return (znum);

   /* now, work backwards and reconstruct the fraction. */
   eval = 0.0;
   for (i=minterms-1; i>=0; i--) {
      eval += alpha * ((double) TINUED_FRAC[i]) * ((double) other->tinued_frac[i]) + beta;
      /* not normally needed, will this help mystery crash? */
      if (eval == 0.0) { eval = 1.0e30; } else { eval = 1.0 / eval; }
   }

   znum += eval;
   return (znum);

}

/* ------------------------------------------------------------ */
/* computes inverses by binary subdivision.
 */

double Inverse (void *cxt, double (*func)(void *, double), double val)
{
   double mid, guess;
   int ires;
   double delta;

   delta = 0.5;
   guess = 0.5;

   mid = (*func)(cxt, guess);

   for (ires=0; ires <55; ires ++) {
      delta *= 0.5;

      if (val > mid) {
         guess += delta;
      } else {
         guess -= delta;
      }
      mid = (*func) (cxt, guess);
   }
   
   return guess;
}

/* ------------------------------------------------------------ */
/* computes inverse of Zreal mapping */

struct InvZReal_s {
   struct Farey *fp;
   double zee;
};

double InvZReal_f (void * stru, double val) 
{
   struct InvZReal_s *sp;
   double retval;

   sp = (struct InvZReal_s *) stru;
    
   SetReal (sp->fp, val);
   retval = ContinuedFractionToZReal (sp->fp, sp->zee);

   return retval;
}

double InvZReal (double val, double zzz) 
{
   struct InvZReal_s s;
   double retval;
   int intpart;

   s.fp = CreateFarey ();
   s.zee = zzz;

   intpart = (int) val;
   if (0.0 > val) intpart --;
   val -= (double) intpart;

   retval = Inverse (((void *) &s), InvZReal_f, val);

   retval += (double) intpart;

   free (s.fp);
   return retval;
}

/* ---------------------- END OF FILE ------------------------- */
