
/* 
 * FUNCTION:
 * Convert real numbers to Farey numbers, and back
 * Coded in pseudo-OO style for easy conversion to C++
 *
 * HISTORY:
 * created Linas Vepstas January 16 1994
 * Added Farey July 1995 -- linas
 * Added xplus/minus Oct 2004 -- linas
 */

// #include <gmp.h>

#include <complex.h>
// #define Complex double __complex__
// template<typename _Tp> class complex;
// template<> class complex<double>;
#define Complex complex<double>

#define CONTINUED_FRAC_MAX_TERMS 32

class ContinuedFraction
{
   public:
      ContinuedFraction (void);
      void SetReal (double);
      void SetRatio (int num, int deno);
		void SetEvenize (void);

      void Print (void);
      int GetNumTerms (void);
		void SwapTerms (int p, int q);

		int GetConvNum (int n);
		int GetConvDenom (int n);
		
      double ToReal (void);
      double ToXPlus (double w);
      double ToXMinus (double w);
      double ToXEven (double w);
      double ToXOdd (double w);
      double ToGapPlus (void);
      double ToGapMinus (void);
		double ToGapEven (void);
		double ToGapOdd (void);
		double ToLastPair (void);

      double ToZReal (double z);
      double ToInvZReal (double z);
      double ToEReal (double t);
      double ToEFraction (double t);

      Complex cToZReal (Complex z);

      double ToCosReal (double omega);
      double ToSincReal (double omega);
      double ToCnReal (double omega);
      double ToSnReal (double omega);

      double ToFCnReal (double omega);
      double ToFSnReal (double omega);

      double ToZCnReal (double omega, double z);
      double ToZSnReal (double omega, double z);

      double ToZRealGap (void);

      double ToFarey (void);
      double ToPAdicFarey (int prime);
		
      double ToEFarey (double t);
      double ToEFareyGap (double t);
      double ToSinFarey (double t);
      double ToTFarey (double t);
      double ToXFarey (double t);

      double CFSum (ContinuedFraction *other,
                     double alpha, double beta, double gamma);
      double CFProd (ContinuedFraction *other,
                     double alpha, double beta);

   protected:
      void RealToContinuedFraction (double);
      void RatioToContinuedFraction (int num, int deno);
		double GapSum (int sn, int tn, int un, int vn);

   private:
      double real;          /* the number, in float pt rep. */ 

      int intpart;          /* the integer part of the real number */
      double fracpart;      /* the fractional part of the real number  */
      unsigned int num;     /* the numerator of the fractional part */
      unsigned int denom;   /* the denominator of the fractional part */
      unsigned int rdenom;  /* the denominator, reduced so its prime to numerator */

      int tinued_frac[CONTINUED_FRAC_MAX_TERMS];  /* values of continued fraction */
      int nterms;           /* number of terms in the continued fraction expansion */

      int cutoff;           /* used to avoid instability when converting
                             * floating point numbers */

		int evenize;          /* If set, work with continued fractions 
                             * that have even number of terms */

		/* Numerator and denominator of partial convergents */
		// mpz_t p_convergent[CONTINUED_FRAC_MAX_TEMRS];
		// mpz_t q_convergent[CONTINUED_FRAC_MAX_TEMRS];
      double partial[32];   /* scratch area of partial values */
      ContinuedFraction *scratch;
};


extern double Inverse (void *, double (*)(void *, double), double);

extern double InvFarey (double);
extern double InvZReal (double, double);


/* ---------------------- END OF FILE ------------------------- */
