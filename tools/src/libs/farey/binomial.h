/* 
 * binomial.h
 *
 * binomial coefficient related function prototypes 
 */
#ifndef BINOMIAL_H__
#define BINOMIAL_H__

#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

//  C and C++ is fugnuts insane in complex support.
#define complex _Complex

// brute-force factorial function
long double factorial (int n);

// brute-force binomial coefficent 
// return n! / m! (n-m)!
// must have m<=n, m>=0
long double binomial (int n, int m);

// real-valued binomial coefficent
// must have m>=0
// returns z*(z-1)*(z-2)...*(z-m+1) / m!
//
long double fbinomial (long double z, int m);

// complex-valued binomial coefficent
// must have m>=0
// returns z*(z-1)*(z-2)...*(z-m+1) / m!
//
long double complex cbinomial (long double complex z, int m);

// brute-force ratio of factorials
// return n! / m!  where m<=n
long double frat (int n, int m);

#ifdef __cplusplus
};
#endif

#endif /* BINOMIAL_H__ */
