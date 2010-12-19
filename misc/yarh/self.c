/*
 * self.c
 *
 * Self-similarity explorer
 * Linas Vepstas Dec 2010
 */

void get_mob(int a1, int a2, int *a, int *b, int *c, int *d)
{

	*a = 1-a1*(a1-a2);
	*b = a1-a2;
	*c = -(a1-a2)*(1+a1*a2);
	*d = 1+a2*(a1-a2);
}

int det(int M[4][4])
{
	int d = 0;
	d += M[0][0] * M[1][1] * M[2][2] * M[3][3];
	d -= M[0][0] * M[1][1] * M[2][3] * M[3][2];
	d -= M[0][0] * M[1][2] * M[2][1] * M[3][3];
	d += M[0][0] * M[1][2] * M[2][3] * M[3][1];
	d += M[0][0] * M[1][3] * M[2][1] * M[3][2];
	d -= M[0][0] * M[1][3] * M[2][2] * M[3][1];

	d -= M[0][1] * M[1][0] * M[2][2] * M[3][3];
	d += M[0][1] * M[1][0] * M[2][3] * M[3][2];
	d += M[0][1] * M[1][2] * M[2][0] * M[3][3];
	d -= M[0][1] * M[1][2] * M[2][3] * M[3][0];
	d -= M[0][1] * M[1][3] * M[2][0] * M[3][2];
	d += M[0][1] * M[1][3] * M[2][2] * M[3][0];

	d += M[0][2] * M[1][1] * M[2][0] * M[3][3];
	d -= M[0][2] * M[1][1] * M[2][3] * M[3][0];
	d -= M[0][2] * M[1][0] * M[2][1] * M[3][3];
	d += M[0][2] * M[1][0] * M[2][3] * M[3][1];
	d += M[0][2] * M[1][3] * M[2][1] * M[3][0];
	d -= M[0][2] * M[1][3] * M[2][0] * M[3][1];

	d -= M[0][3] * M[1][0] * M[2][2] * M[3][1];
	d += M[0][3] * M[1][0] * M[2][1] * M[3][2];
	d += M[0][3] * M[1][2] * M[2][0] * M[3][1];
	d -= M[0][3] * M[1][2] * M[2][1] * M[3][0];
	d -= M[0][3] * M[1][1] * M[2][0] * M[3][2];
	d += M[0][3] * M[1][1] * M[2][2] * M[3][0];

	return d;
}

main ()
{
}
