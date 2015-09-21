
/*
 * Stern-Brocot tree, new algo
 * This is the same old question mark, just a different API.
 *
 * XXX This has been copied to stern-brocot.c  This version
 * is dead.
 *
 * Linas Vepstas September 2015
 */

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
