
/* tensor.c 
 *
 * bogus tensor library because I'm too lazy to learn other systems
 *
 */

#include <glib.h>

/* partial eff /partial x or y */
struct eff_s
{
	int dx;
	int dy;
};

typedef struct eff_s Part;

/* one term */
struct term_s
{
	int coeff;
	GList * parts;   /* list of effs */
};

typedef struct term_s Term;

/* list of terms */
struct expr_s
{
	GList * terms;
};

typedef struct expr_s Expr;

void SimplifyExpr (Expr *e);

/* =============================================== */

void 
PrintPart (Part *p)
{
	int i;
	if (p->dx != 0 || p->dy != 0) 
	{
		printf ("d");
	}
	for (i=0; i<p->dx; i++)
	{
		printf ("x");
	}
	for (i=0; i<p->dy; i++)
	{
		printf ("y");
	}
	printf ("f ");
}

void
PrintTerm (Term *t)
{
	// if (0 == t->coeff) return;

	if (1 == t->coeff && NULL != t->parts)
	{
		printf ("+ ");
	}
	else if (-1 == t->coeff && NULL != t->parts)
	{
		printf ("- ");
	}
	else if (0 > t->coeff)
	{
		printf ("- %d ", - t->coeff);
	}
	else 
	{
		printf ("+ %d ", t->coeff);
	}
	
	GList *pn;
	for (pn = t->parts; pn; pn=pn->next)
	{
		Part *p = pn->data;
		PrintPart(p);
	}
	printf ("\n");
}

void 
PrintExpr (Expr *e)
{
	if (NULL == e) 
	{
		printf ("NULL expresion!\n");
		return;
	}
	SimplifyExpr (e);

	GList *tn;
	for (tn = e->terms; tn; tn=tn->next)
	{
		Term *t = tn->data;
		PrintTerm (t);
	}

	printf ("The End\n\n");
}

/* ======================================================== */

Part * 
Copy_Part (Part *p)
{
	Part *p3 = g_new0(Part,1);
	p3->dx = p->dx;
	p3->dy = p->dy;
	return p3;
}

Term * Copy_Term (Term *t)
{
	Term *t3 = g_new0(Term, 1);
	GList *pn;
	for (pn = t->parts; pn; pn=pn->next)
	{
		Part *p = pn->data;
		t3->parts = g_list_append (t3->parts, Copy_Part(p));
	}

	t3->coeff = t->coeff;
	return t3;
}

Expr *Copy_Expr (Expr *e)
{
	Expr *e3 = g_new0(Expr, 1);

	GList *en;
	for (en = e->terms; en; en=en->next)
	{
		Term *tr = en->data;
		e3->terms = g_list_append (e3->terms, Copy_Term(tr));
	}
	return e3;
}

/* ======================================================== */
/* multipliy terms */

Term *Mult_T_T (Term *t1, Term *t2)
{
	Term * t3 = g_new0(Term,1);
	
	t3->coeff = t1->coeff * t2->coeff;
	t3->parts = NULL;

	GList *pn;
	for (pn = t1->parts; pn; pn=pn->next)
	{
		Part *p = pn->data;
		t3->parts = g_list_append (t3->parts, Copy_Part(p));
	}
	for (pn = t2->parts; pn; pn=pn->next)
	{
		Part *p = pn->data;
		t3->parts = g_list_append (t3->parts, Copy_Part(p));
	}

	return t3;
}

void Mult_T_I (Term *t, int c)
{
	t->coeff *= c;
}

Expr *Mult_E_T (Expr *e, Term *t)
{
	Expr *e3 = g_new0 (Expr,1);

	GList *en;
	for (en = e->terms; en; en=en->next)
	{
		Term *tr = en->data;
		e3->terms = g_list_append (e3->terms, Mult_T_T(tr, t));
	}
	return e3;
}

Expr *Mult_E_E (Expr *e1, Expr *e2)
{
	Expr *e3 = g_new0 (Expr,1);

	GList *en;
	for (en = e1->terms; en; en=en->next)
	{
		Term *tr = en->data;
		Expr *ep = Mult_E_T(e2, tr);
		e3->terms = g_list_concat (e3->terms, ep->terms);
		g_free (ep);
	}
	return e3;
}


/* ======================================================== */

Expr *Add_E_T (Expr *e, Term *t)
{
	Expr *e3 = Copy_Expr(e);

	e3->terms = g_list_append (e3->terms, Copy_Term(t));
	return e3;
}

Expr *Add_E_E (Expr *e1, Expr *e2)
{
	if (!e1) return Copy_Expr(e2);

	Expr *e3 = Copy_Expr(e1);
	Expr *e4 = Copy_Expr(e2);

	e3->terms = g_list_concat (e3->terms, e4->terms);
	SimplifyExpr (e3);

	g_free (e4); // xxx mem leak
	return e3;
}

Expr *Sub_E_E (Expr *e1, Expr *e2)
{
	Expr *e3 = Copy_Expr(e1);
	Expr *e4 = Copy_Expr(e2);

	GList *tn;
	for (tn=e4->terms; tn; tn=tn->next)
	{
		Term *t = tn->data;
		t->coeff *= -1;
	}

	e3->terms = g_list_concat (e3->terms, e4->terms);
	SimplifyExpr (e3);

	g_free (e4); // xxx mem leak
	return e3;
}

/* ======================================================== */
/* Derivatives */

Expr *DD_Term (Term *t, int which)
{
	if (0 == t->coeff) return NULL;
	if (NULL == t->parts) return NULL;

	Expr *e3 = g_new0(Expr, 1);

	GList *pn;
	for (pn = t->parts; pn; pn=pn->next)
	{
		Term *t3 = g_new0 (Term,1);
		t3->coeff = t->coeff;

		GList *pm;
		for (pm = t->parts; pm; pm=pm->next)
		{
			Part *p = pm->data;
			Part *p3 = Copy_Part (p);
			
			if (pm == pn)
			{
				if (0 == which) p3->dx ++;
				if (1 == which) p3->dy ++;
			}
			t3->parts = g_list_append (t3->parts, p3);
		}
		e3 = Add_E_T (e3, t3);		
	}

	return e3;
}

Expr *DD (Expr *e, int which)
{
	Expr *e3 = g_new0(Expr, 1);
	
	GList *en;
	for (en = e->terms; en; en=en->next)
	{
		Term *tr = en->data;
		Expr *em = DD_Term (tr, which);
		if (!em) continue;
		e3->terms = g_list_concat (e3->terms, em->terms);
		g_free (em);
	}
	return e3;
}

Expr *DDX (Expr *e)
{
	return DD (e,0);
}

Expr *DDY (Expr *e)
{
	return DD (e,1);
}

/* ======================================================== */
/* sort things out */

static gint part_compare (gconstpointer a, gconstpointer b)
{
	const Part *pa = a;
	const Part *pb = b;

	if (pa->dy < pb->dy) return -1;
	if (pa->dy > pb->dy) return 1;

	if (pa->dx < pb->dx) return -1;
	if (pa->dx > pb->dx) return 1;

	return 0;
}

void OrderTerm (Term *t)
{
	t->parts = g_list_sort (t->parts, part_compare);
}

void OrderTerm_E (Expr *e)
{
	GList *en;
	for (en = e->terms; en; en=en->next)
	{
		Term *tr = en->data;
		OrderTerm (tr);
	}
}

/* ======================================================== */
/* predicates */

static gint IsSame_Part (const Part *pa, const Part *pb)
{
	if (pa->dx != pb->dx) return 0;
	if (pa->dy != pb->dy) return 0;
	return 1;
}

static gint IsSame_Term (const Term *ta, const Term *tb)
{
	// Assume we are already ordered .... 
	// OrderTerm (ta);
	// OrderTerm (tb);
	GList *pna, *pnb;

	for (pna = ta->parts, pnb=tb->parts; pna && pnb; 
	                        pna=pna->next, pnb=pnb->next)
	{
		Part *pa = pna->data;
		Part *pb = pnb->data;

		if (0 == IsSame_Part (pa,pb)) return 0;
	}
	if (pna) return 0;
	if (pnb) return 0;
	return 1;
}

/* ======================================================== */

static void MergeTerms (Expr *e)
{
	OrderTerm_E (e);
	GList *en;

restart:
	for (en = e->terms; en; en=en->next)
	{
		Term *ta = en->data;
		GList *em;
		for (em = en->next; em; em=em->next)
		{
			Term *tb = em->data;
			if (IsSame_Term (ta, tb))
			{
				ta->coeff += tb->coeff;
				g_free (tb); // xx memleak,delete parts too
				g_list_remove_link (e->terms, em);
				goto restart;
			}
		}
	}
}

static void KillZeroTerms (Expr *e)
{
	GList *en;

restart:
	for (en = e->terms; en; en=en->next)
	{
		Term *ta = en->data;
		if (0 == ta->coeff)
		{
			e->terms = g_list_remove_link (e->terms, en);
			g_free (ta); // xx memleak,delete parts too
			goto restart;
		}
	}
}

void SimplifyExpr (Expr *e)
{
	MergeTerms(e);
	KillZeroTerms (e);
}

/* ======================================================== */
/* userland */

Term *tger (int x)
{
	Term *t = g_new0 (Term,1);
	t->coeff = x;
	return t;
}

Term *dxf (void)
{
	Term *t = g_new0 (Term,1);

	Part *p = g_new0 (Part,1);
	p->dx = 1;
	p->dy = 0;

	t->parts = g_list_append (t->parts, p);
	t->coeff = 1;

	return t;
}

Term *dyf (void)
{
	Term *t = g_new0 (Term,1);

	Part *p = g_new0 (Part,1);
	p->dx = 0;
	p->dy = 1;

	t->parts = g_list_append (t->parts, p);
	t->coeff = 1;

	return t;
}

/* ======================================================== */
/* return metric xx */
Expr * gxx (void)
{
	Expr *e = g_new0 (Expr,1);
	e = Add_E_T (e, tger(1));
	e = Add_E_T (e, Mult_T_T (dxf(), dxf()));
	return e;
}

/* return metric yy */
Expr * gyy (void)
{
	Expr *e = g_new0 (Expr,1);
	e = Add_E_T (e, tger(1));
	e = Add_E_T (e, Mult_T_T (dyf(), dyf()));
	return e;
}

/* return metric xy */
Expr * gxy (void)
{
	Expr *e = g_new0 (Expr,1);
	e = Add_E_T (e, Mult_T_T (dxf(), dyf()));
	return e;
}

Expr * gmn (int m, int n)
{
	if (0==m && 0==n) return gxx();
	if (0==m && 1==n) return gxy();
	if (1==m && 0==n) return gxy();
	if (1==m && 1==n) return gyy();
}

/* ----------------- */

/* return inverse metric xx */
Expr * ginv_xx (void)
{
	Expr *e = g_new0 (Expr,1);
	e = Add_E_T (e, tger(1));
	e = Add_E_T (e, Mult_T_T (dyf(), dyf()));
	return e;
}

/* return inverse metric yy */
Expr * ginv_yy (void)
{
	Expr *e = g_new0 (Expr,1);
	e = Add_E_T (e, tger(1));
	e = Add_E_T (e, Mult_T_T (dxf(), dxf()));
	return e;
}

/* return inverse metric xy */
Expr * ginv_xy (void)
{
	Expr *e = g_new0 (Expr,1);
	Term *t = Mult_T_T (dxf(), dyf());
	t->coeff *= -1;
	e = Add_E_T (e, t);
	return e;
}

Expr * ginv_mn (int m, int n)
{
	if (0==m && 0==n) return ginv_xx();
	if (0==m && 1==n) return ginv_xy();
	if (1==m && 0==n) return ginv_xy();
	if (1==m && 1==n) return ginv_yy();
}

/* ======================================================== */

Expr * Christoffel (int i, int k, int l)
{
	int m;

	Expr *cr = NULL;
	for (m=0; m<2; m++)
	{
		Expr *sum = NULL;
		Expr *e1 = DD (gmn (m,k) , l);
		Expr *e2 = DD (gmn (m,l) , k);
		Expr *e3 = DD (gmn (k,l) , m);

		sum = Add_E_E (sum,e1);
		sum = Add_E_E (sum,e2);
		sum = Sub_E_E (sum,e3);

		Expr *gi = ginv_mn (i, m);
		Expr *mt = Mult_E_E (gi, sum);
		cr = Add_E_E (cr, mt);
	}

	SimplifyExpr (cr);
	/* hack divide by 2 */
	Term *t = cr->terms->data;
	t->coeff /= 2;
	return cr;
}

/* ======================================================== */

Expr * Ricci (int i, int k)
{
	int l,m;

	Expr *ri = NULL;
	for (l=0; l<2; l++)
	{
		Expr *c1 = Christoffel (l,i,k);
	}
	return ri;
}

/* ======================================================== */

void setup (void)
{
#if 0
	printf ("gxx is:\n");
	PrintExpr (gxx());

	printf ("gxy is:\n");
	PrintExpr (gxy());

	printf ("gyy is:\n");
	PrintExpr (gyy());

	printf ("ginv_yy is:\n");
	PrintExpr (ginv_yy());

	printf ("dx gxx is:\n");
	PrintExpr (DDX(gxx()));

	printf ("dx gyy is:\n");
	PrintExpr (DDX(gyy()));

	printf ("dy gxx is:\n");
	PrintExpr (DDY(gxx()));
#endif

	printf ("gamma xxx is:\n");
	PrintExpr (Christoffel(0,0,0));
	
	printf ("gamma yxx is:\n");
	PrintExpr (Christoffel(1,0,0));
	
	printf ("gamma xxy is:\n");
	PrintExpr (Christoffel(0,0,1));
	
	printf ("gamma yxy is:\n");
	PrintExpr (Christoffel(1,0,1));
	
	printf ("gamma xyy is:\n");
	PrintExpr (Christoffel(0,1,1));
	
	printf ("gamma yyy is:\n");
	PrintExpr (Christoffel(1,1,1));
	
}

main ()
{

	setup();
}
