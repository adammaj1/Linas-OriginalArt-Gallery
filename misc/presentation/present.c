/*
 * present.c
 *
 * hunt out group presentations
 *
 * Linas February 2007
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _matrix {
	int dim;
	int *elts;
} Matrix;

Matrix * matrix_new (int dim)
{
	Matrix *mat = (Matrix *)malloc (sizeof (Matrix));
	mat->dim = dim;
	mat->elts = (int *)malloc (dim*dim*sizeof(int));
	return mat;
}

/* element acccessor */
#define MELT(mat,m,n) ((mat)->elts[(m)*(mat)->dim + (n)])

void matrix_set (Matrix *mat, int m, int n, int val)
{
	MELT(mat,m,n) = val;
}

void matrix_copy (Matrix *to, Matrix *from)
{
	int m,n;
	for (m=0; m<from->dim; m++)
	{
		for (n=0; n<from->dim; n++)
		{
			MELT(to,m,n) = MELT(from, m,n);
		}
	}
}

Matrix * matrix_dup (Matrix *from)
{
	Matrix *mat = matrix_new (from->dim);
	matrix_copy (mat, from);
	return mat;
}

void matrix_unit (Matrix *mat)
{
	int m,n;
	for (m=0; m<mat->dim; m++)
	{
		for (n=0; n<mat->dim; n++)
		{
			MELT(mat,m,n) = 0;
		}
		MELT(mat,m,m) = 1;
	}
}

void matrix_mult (Matrix *prod, Matrix *a, Matrix *b)
{
	int m,n,p;
	for (m=0; m<a->dim; m++)
	{
		for (n=0; n<a->dim; n++)
		{
			int acc = 0;
			for (p=0; p<a->dim; p++)
			{
				acc += MELT(a,m,p) * MELT(b, p,n);
			}
			MELT(prod,m,n) = acc;
		}
	}
}

int matrix_equal (Matrix *a, Matrix *b)
{
	int m,n;
	for (m=0; m<a->dim; m++)
	{
		for (n=0; n<a->dim; n++)
		{
			if (MELT(a,m,n) != MELT(b, m,n)) return 0;
		}
	}
	return 1;
}

/* ---------------------------------------------------- */
/* linked list of matrixes */
typedef struct _mlist MatList;

struct _mlist {
	MatList *next;  //next in list
	Matrix *mat;    // this group element
	char *str;      // word describing this group element
	char *last;     // last letter of the word
	MatList *match; // equivalent word
};

MatList *matlist_prepend (MatList *ml, Matrix *mat, char *str, char letter)
{
	MatList *node = (MatList *)malloc(sizeof (MatList));
	node->mat = mat;
	node->next = ml;

	/* append letter to string, store string */
	int len = strlen(str);
	node->str = malloc (len+2);
	strcpy(node->str, str);
	node->str[len] = letter;
	node->str[len+1] = 0x0;
	node->last = &node->str[len];

	node->match = NULL;
	return node;
}

/* Find a matching matrix in the list */
MatList *matlist_find (MatList *ptr, Matrix *mat)
{
	while (ptr)
	{
		if (matrix_equal (ptr->mat, mat)) return ptr;
		ptr = ptr->next;
	}
	return NULL;
}

/* ---------------------------------------------------- */
/* list of strings */

typedef struct _slist StrList;

struct _slist {
	StrList *next;
	char * str;
	size_t len;
};

StrList * slist_prepend (StrList *sl, char * str)
{
	StrList *head = (StrList *)malloc (sizeof(StrList));
	head->str = str;
	head->len = strlen (str);
	head->next = sl;
	return head;
}

/* Turn a matching pair into a single word string equal to the identity */
char * make_str (MatList *word)
{
	int la = strlen (word->str);
	int lb = strlen (word->match->str);
	lb --; // strip off leading E
	la --;

	char *e = malloc (la+lb+1);
	strcpy (e, &word->str[1]);   // be sure to srip off leading E
	while (lb)
	{
		e[la] = 0x20 ^ word->match->str[lb];
		la++;
		lb--;
	}
	e[la] = 0x0;
	return e;
}

/* check to see if its a cyclic reordering */
static int is_cyclic_off (char *a, char * b, size_t offset)
{
	char * p = b+offset;
	while (*a)
	{
		if (*a != *p) return 0;
		a++;
		p++;
		if (0x0 == *p) p = b;
	}
	return 1;
}

/* check to see if its a cyclic reversed reordering */
static int is_cyclic_rev (char *a, char * b, size_t offset)
{
	char * p = b+offset;
	while (*a)
	{
		if (*a != (0x20^(*p))) return 0;
		a++;
		if (b == p) p = b+strlen(b);
		p--;
	}
	return 1;
}

int is_cyclic (char *a, char * b, size_t len)
{
	size_t i;
	for (i=0; i<len; i++)
	{
		if (1 == is_cyclic_off (a,b,i)) return 1;
		if (1 == is_cyclic_rev (a,b,i)) return 1;
	}
	return 0;
}

/* If the word appears in the list of known words, then return true, else false */
int slist_in_list (StrList *sl, char * word)
{
	int len = strlen (word);
	while (sl)
	{
		if (len == sl->len)
		{
			if (is_cyclic (sl->str, word, len)) return 1;
		}
		sl= sl->next;
	}
	return 0;
}

/* ---------------------------------------------------- */
/* master struct */

typedef struct _present {
	MatList *generators;    // list of group generators (and thier inverses)
	MatList *words;         // list of words tested so far 
	MatList *presentation;  // presentation of group so far.
	int found;
	int cnt;
	StrList *unique;
} Present;

Present * present_new (int dim)
{
	Present *pr = (Present *) malloc (sizeof (Present));
	pr->generators = NULL;
	pr->words = NULL;
	pr->presentation = NULL;
	pr->cnt =0;
	pr->found = 0;
	pr->unique = NULL;

	/* identity matrix */
	Matrix *e = matrix_new (dim);
	matrix_unit (e);
	pr->words = matlist_prepend (NULL, e, "", 'E');

	return pr;
}
	
static inline isinv (char a, char b)
{
	if ((a^0x20) == b) return 1;
	return 0;
}

void present_walk_tree (Present *pr, MatList *node, int depth)
{
	if (0 == depth) return;
	depth --;
	pr->cnt ++;

	MatList *gen = pr->generators;
	while (gen)
	{
		/* Do not concatent matrix to its inverse -- skip these words */
		if (isinv (node->last[0], gen->str[0]))
		{
			gen = gen->next;
			continue;
		}

		/* concatenate matrixes, make a new word */
		Matrix *mat = matrix_new (node->mat->dim);
		matrix_mult (mat, node->mat, gen->mat);

		/* Have we seen this word before ? */
		MatList *match = matlist_find (pr->words, mat);
		if (match)
		{
			pr->presentation = matlist_prepend (pr->presentation, mat, node->str, gen->str[0]);	

			/* add forward and backward links */
			match->match = pr->presentation;
			pr->presentation->match = match;
			pr->found ++;

			printf ("got one! %d %s == (%s %c)\n", pr->found, match->str, node->str, gen->str[0]);
		}
		else
		{
			/* unknown. keep going */
			pr->words = matlist_prepend (pr->words, mat, node->str, gen->str[0]);	
			present_walk_tree (pr, pr->words, depth);
		}

		gen = gen->next;
	}
}

void present_cleanup (Present *pr)
{
	MatList *word = pr->presentation;
	while (word)
	{
		char * sword = make_str (word);
		if (0 == slist_in_list(pr->unique, sword))
		{
			pr->unique = slist_prepend (pr->unique, sword);
printf ("found unique %s\n", sword);
		}
		else
		{
			free(sword);
		}
		word = word->next;
	}
}

/* ---------------------------------------------------- */
/* set up braid group 3 */

Present * setup_braid3 (void)
{
	Present *pr = present_new(2);
	
	MatList *ml = NULL;

	/* Set up sigma 1 and sigma 2 of braid group B_3 */
	Matrix *s1 = matrix_new (2);
	MELT(s1, 0,0) = 1;
	MELT(s1, 0,1) = 1;
	MELT(s1, 1,0) = 0;
	MELT(s1, 1,1) = 1;
	ml = matlist_prepend (ml, s1, "", 'A');
	
	Matrix *s2 = matrix_new (2);
	MELT(s2, 0,0) = 1;
	MELT(s2, 0,1) = 0;
	MELT(s2, 1,0) = -1;
	MELT(s2, 1,1) = 1;
	ml = matlist_prepend (ml, s2, "", 'B');
	
	/* and now thier inverses (by hand) */
	s1 = matrix_new (2);
	MELT(s1, 0,0) = 1;
	MELT(s1, 0,1) = -1;
	MELT(s1, 1,0) = 0;
	MELT(s1, 1,1) = 1;
	ml = matlist_prepend (ml, s1, "", 'a');
	
	s2 = matrix_new (2);
	MELT(s2, 0,0) = 1;
	MELT(s2, 0,1) = 0;
	MELT(s2, 1,0) = 1;
	MELT(s2, 1,1) = 1;
	ml = matlist_prepend (ml, s2, "", 'b');

	pr->generators = ml;
	return pr;
}

/* ---------------------------------------------------- */
/* set up 3D Heisenberg group */

Present * setup_heisenberg (void)
{
	Present *pr = present_new(3);
	
	MatList *ml = NULL;

	/* Set up x and y of Heisenberg group */
	Matrix *x = matrix_new (3);
	MELT(x, 0,0) = 1;
	MELT(x, 0,1) = 1;
	MELT(x, 0,2) = 0;
	MELT(x, 1,0) = 0;
	MELT(x, 1,1) = 1;
	MELT(x, 1,2) = 0;
	MELT(x, 2,0) = 0;
	MELT(x, 2,1) = 0;
	MELT(x, 2,2) = 1;
	ml = matlist_prepend (ml, x, "", 'X');

	Matrix *y = matrix_new (3);
	MELT(y, 0,0) = 1;
	MELT(y, 0,1) = 0;
	MELT(y, 0,2) = 0;
	MELT(y, 1,0) = 0;
	MELT(y, 1,1) = 1;
	MELT(y, 1,2) = 1;
	MELT(y, 2,0) = 0;
	MELT(y, 2,1) = 0;
	MELT(y, 2,2) = 1;
	ml = matlist_prepend (ml, y, "", 'Y');

	x = matrix_new (3);
	MELT(x, 0,0) = 1;
	MELT(x, 0,1) = -1;
	MELT(x, 0,2) = 0;
	MELT(x, 1,0) = 0;
	MELT(x, 1,1) = 1;
	MELT(x, 1,2) = 0;
	MELT(x, 2,0) = 0;
	MELT(x, 2,1) = 0;
	MELT(x, 2,2) = 1;
	ml = matlist_prepend (ml, x, "", 'x');

	y = matrix_new (3);
	MELT(y, 0,0) = 1;
	MELT(y, 0,1) = 0;
	MELT(y, 0,2) = 0;
	MELT(y, 1,0) = 0;
	MELT(y, 1,1) = 1;
	MELT(y, 1,2) = -1;
	MELT(y, 2,0) = 0;
	MELT(y, 2,1) = 0;
	MELT(y, 2,2) = 1;
	ml = matlist_prepend (ml, y, "", 'y');

	pr->generators = ml;
	return pr;
}
	
/* ---------------------------------------------------- */
/* set up trilog (polylog n=3) */

Present * setup_trilog (void)
{
	Present *pr = present_new(4);
	
	MatList *ml = NULL;
	Matrix *g0 = matrix_new (4);
	matrix_unit (g0);
	MELT(g0,0,1) = 1;
	MELT(g0,0,2) = 1;
	MELT(g0,1,2) = 2;
	ml = matlist_prepend (ml, g0, "", 'A');
	
	Matrix *g1 = matrix_new (4);
	matrix_unit (g1);
	MELT(g1,2,3) = 1;
	ml = matlist_prepend (ml, g1, "", 'B');
	
	g0 = matrix_new (4);
	matrix_unit (g0);
	MELT(g0,0,1) = -1;
	MELT(g0,0,2) = 1;
	MELT(g0,1,2) = -2;
	ml = matlist_prepend (ml, g0, "", 'a');
	
	g1 = matrix_new (4);
	matrix_unit (g1);
	MELT(g1,2,3) = -1;
	ml = matlist_prepend (ml, g1, "", 'b');
	
	pr->generators = ml;
	return pr;
}

/* ---------------------------------------------------- */
/* set up polylog (polylog n=4) */

Present * setup_quadlog (void)
{
	Present *pr = present_new(5);
	
	MatList *ml = NULL;
	Matrix *g0 = matrix_new (5);
	matrix_unit (g0);
	MELT(g0,0,1) = 1;
	MELT(g0,0,2) = 1;
	MELT(g0,1,2) = 2;
	MELT(g0,0,3) = 1;
	MELT(g0,1,3) = 3;
	MELT(g0,2,3) = 3;
	ml = matlist_prepend (ml, g0, "", 'A');
	
	Matrix *g1 = matrix_new (5);
	matrix_unit (g1);
	MELT(g1,3,4) = 1;
	ml = matlist_prepend (ml, g1, "", 'B');
	
	g0 = matrix_new (5);
	matrix_unit (g0);
	MELT(g0,0,1) = -1;
	MELT(g0,0,2) = 1;
	MELT(g0,1,2) = -2;

	MELT(g0,0,3) = -1;
	MELT(g0,1,3) = 3;
	MELT(g0,2,3) = -3;
	ml = matlist_prepend (ml, g0, "", 'a');
	
	g1 = matrix_new (5);
	matrix_unit (g1);
	MELT(g1,3,4) = -1;
	ml = matlist_prepend (ml, g1, "", 'b');
	
	pr->generators = ml;
	return pr;
}

/* ---------------------------------------------------- */
/* set up polylog (polylog n=5) */

Present * setup_pentalog (void)
{
	Present *pr = (Present *) malloc (sizeof (Present));
	pr->generators = NULL;
	pr->words = NULL;
	pr->presentation = NULL;
	pr->cnt =0;
	pr->found = 0;


	/* identity matrix */
	Matrix *e = matrix_new (6);
	matrix_unit (e);
	pr->words = matlist_prepend (NULL, e, "", 'E');
	
	MatList *ml = NULL;
	Matrix *g0 = matrix_new (6);
	matrix_unit (g0);
	MELT(g0,0,1) = 1;
	MELT(g0,0,2) = 1;
	MELT(g0,0,3) = 1;
	MELT(g0,0,4) = 1;
	MELT(g0,1,2) = 2;
	MELT(g0,1,3) = 3;
	MELT(g0,2,3) = 3;
	MELT(g0,1,4) = 4;
	MELT(g0,2,4) = 6;
	MELT(g0,3,4) = 4;
	ml = matlist_prepend (ml, g0, "", 'A');
	
	Matrix *g1 = matrix_new (6);
	matrix_unit (g1);
	MELT(g1,4,5) = 1;
	ml = matlist_prepend (ml, g1, "", 'B');
	
	g0 = matrix_new (6);
	matrix_unit (g0);
	MELT(g0,0,1) = -1;

	MELT(g0,0,2) = 1;
	MELT(g0,1,2) = -2;

	MELT(g0,0,3) = -1;
	MELT(g0,1,3) = 3;
	MELT(g0,2,3) = -3;

	MELT(g0,0,4) = 1;
	MELT(g0,1,4) = -4;
	MELT(g0,2,4) = 6;
	MELT(g0,3,4) = -4;
	ml = matlist_prepend (ml, g0, "", 'a');
	
	g1 = matrix_new (6);
	matrix_unit (g1);
	MELT(g1,4,5) = -1;
	ml = matlist_prepend (ml, g1, "", 'b');
	
	pr->generators = ml;
	return pr;
}

/* ---------------------------------------------------- */

main ()
{
	int depth;
	Present *pr;

	for (depth=4; ; depth++)
	{
		printf ("start depth=%d\n", depth);
		// pr = setup_braid3();
		// pr = setup_heisenberg();
		pr = setup_trilog();
		// pr = setup_quadlog();
		// pr = setup_pentalog();
		present_walk_tree (pr, pr->words, depth);
		present_cleanup (pr);
		if (pr->found) break;
	}

	printf ("tested %d words\n", pr->cnt);
}
