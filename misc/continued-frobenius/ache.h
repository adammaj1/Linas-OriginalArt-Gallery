/*
 * ache.h
 *
 * routines that compute various commonly required 
 * things, such as matrix elements for the continued fraction
 *
 * Linas  Vepstas
 * December 2003
 */

// return the matrix element for H_mp
long double ache_mp(int m, int p);

// return the nth term of the binomial expansion of the zeta function
long double a_sub_n (int n);
long double a_sub_n_poor_convergence (int n);

// return the nth term of the binomial expansion of the zeta function
long double t_sub_n (int n, long double a);

// return the p'th element of the zeroth eigenvector.
// this is the eigenvector with eigenvalue 1
// Note that zer_m = sum_p h_mp zer_p and that
// zer is normalized to unit length.  
// zer is just the taylor coeffs of 1/(2-y)
long double zer_p (int p);

// return the nth term of the binomial expansion of the hurwitz zeta function 
long double hz_a_sub_n (int n, double q);

// Hurwitz, for q=m/k
long double lfunc_a_sub_n (int n, int m, int k);
