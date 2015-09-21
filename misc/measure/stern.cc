
/*
 * Stern-Brocot tree, new algo
 * This is the same old question mark, just a different API.
 *
 * XXX This has been copied to stern-brocot.c  This version
 * is dead.
 *
 * Linas Vepstas September 2015
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * Computes the Stern-Brocot tree.
 * It takes as input a dyadic fraction pos / 2^level
 * assuming 0 <= pos <= 2^level.
 * It returns the corresponding p/q value in p and q.
 *
 * In practical terms, given an input x = pos / 2^level
 * this returns the inverse question-mark, given by
 * (as per usual) ?(p/q) = pos / 2^level.
 */
void stern_brocot_tree(unsigned long pos, int level,
                       unsigned long &p, unsigned long &q)
{
	if (0 == level)
	{
		if (0 == pos)
		{
			p = 0; q = 1; return;
		}
		if (1 == pos)
		{
			p = 1; q = 1; return;
		}
	}
	if (0 == pos%2)
		return stern_brocot_tree(pos>>1, level-1, p, q);

	unsigned long pl, ql, pr, qr;
	stern_brocot_tree(pos>>1, level-1, pl, ql);
	stern_brocot_tree((pos>>1) + 1, level-1, pr, qr);
	
	p = pl + pr;
	q = ql + qr;
}

int main(int argc, char *argv[])
{
	int level = atoi(argv[1]);

	unsigned long p, q, pm, qm, pmid, qmid;
	double sum = 0.0;

	double norm = 1.0 / (double)(1<<level);
	for (int i=0; i< (1<<level); i++)
	{
		stern_brocot_tree(i, level, p, q);
		stern_brocot_tree(i+1, level, pm, qm);
		double x = i * norm;
		double a = ((double) p) / (double) q;
		double b = ((double) pm) / (double) qm;
		stern_brocot_tree(2*i+1, level+1, pmid, qmid);
		double y = ((double) pmid) / (double) qmid;
		// unsigned long det = pm * q - p * qm;
		double delta = norm * q * qm;

		sum += (b-a)* delta;
		printf("%d	%g	%lu	%lu	%g	%g	%g\n", i, x, p, q, y, delta, sum);
	}
}
