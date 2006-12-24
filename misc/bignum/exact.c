/**
 * exact.c
 *
 * Look at hurtwitz zeta at the location of the Riemann zeta zeros
 *
 * December 2006
 */

#include <stdio.h>
#include <stdlib.h>

#include "mp-misc.h"
#include "mp-polylog.h"

int
main ()
{
	int prec = 40;
	double q;

	/* Set the precision (number of binary bits) */
	int nbits = 3.3*prec+100;
	mpf_set_default_prec (nbits);

	cpx_t ess, zeta;
	cpx_init (ess);
	cpx_init (zeta);

	mpf_t que;
	mpf_init (que);
			  
	cpx_set_d (ess, 0.5, 14.1);
	char * zero;
	zero = "14.134725141734693790457251983562470270784257115699243175685567460149 \
	        9634298092567649490103931715610127792029715487974367661426914698822545 \
	        8250536323944713778041338123720597054962195586586020055556672583601077";

	zero = "21.022039638771554992628479593896902777334340524902781754629520403587 \
	        5985860688907997136585141801514195337254736424758913838650686037313212 \
	        6211882162437574166925654471184407119403130672564622779261488733743555";
					
	mpf_set_str (ess[0].im, zero, 10);

	// printf ("#\n# graph of Hurwitz zeta as function of q, at the Riemann zero\n#\n");
	printf ("#\n# graph of periodic zeta as function of q, at the Riemann zero\n#\n");
	fp_prt ("# at s=0.5+i ", ess[0].im);
	printf ("\n#\n# prec=%d nbits=%d\n#\n", prec, nbits);
	fflush (stdout);
	for (q=0.01; q<1.0; q+=0.01)
	{
		mpf_set_d (que, q);
		// cpx_hurwitz_zeta (zeta, ess, que, prec);
		cpx_periodic_zeta (zeta, ess, que, prec);

		printf ("%g",q);
		fp_prt ("\t", zeta[0].re);
		fp_prt ("\t", zeta[0].im);
		printf ("\n");
		fflush (stdout);
	}

	return 0;
}
