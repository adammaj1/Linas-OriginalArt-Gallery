
/* 
 * FUNCTION:
 * Test operation of farey number converter
 *
 * HISTORY:
 * Linas Vepstas Januery 16 1994
 */

#include "gl.h"
#include "Farey.h"
#include <stdio.h>
#include <math.h>

struct Farey *f;

/* ------------------------------------------------------------ */
double farey_logistic_map (lambda, x)
double lambda, x;
{
   double y;

   y = (lambda * x * (1.0-x));
   SetReal (f, y);
   y = GetFarey (f);

   return (y);
}
/* ------------------------------------------------------------ */
double farey_tent_map (lambda, x)
double lambda, x;
{
   double y;

   if (x<0.5) {
      y = (2.0*lambda*x);
   } else {
      y = (2.0*lambda*(1.0-x));
   }  
   SetReal (f, y);
   y = GetFarey (f);

   return (y);
}
/* ------------------------------------------------------------ */
double logistic_map (lambda, x)
double lambda, x;
{
   return (lambda * x * (1.0-x));
}
/* ------------------------------------------------------------ */
double tent_map (lambda, x)
double lambda, x;
{
   if (x<0.5) {
      return (2.0*lambda*x);
   } else {
      return (2.0*lambda*(1.0-x));
   }  
}
/* ------------------------------------------------------------ */


/*
#define LOW (0.495)
#define HIGH (0.55)
*/

#define LOW (0.0)
#define HIGH (10.0)

redraw ()
{
   int i, j;
   double lambda;
   double x;

/*
   clear ();

*/
   ortho2 (LOW, HIGH, 0.0, 1.0);

   for (lambda=LOW; lambda<HIGH; lambda += (HIGH-LOW)*0.002) {

      for (j=0; j<161; j++) {
         x = ((float) j) / 161.0;
         for (i=0; i<20; i++) {
            x = farey_logistic_map (lambda, x);        
         }
         v2d (lambda, x);
      }
      gsync ();
   }

   gsync ();
}

/* ------------------------------------------------------------ */

main (argc, argv)
int argc;
char *argv[];
{
   double r, x, y, z, t;
   int i, n;
   int nume, deno;
   int ix, iy;

/*
   if (argc <3) {
      printf ("Usage: %s <number of terms> <base> \n", argv[0]);
      exit (1);
   }
*/

   prefsize (500, 500);
   winopen ();

/*
   n = atoi (argv[1]);
   z = atof (argv[2]);
*/

   f = CreateFarey ();

   while (TRUE) {
      XEvent ev;
      XNextEvent (dpy, &ev);

      switch (ev.type) {
         case ConfigureNotify:
         case Expose:
            printf (" Expose \n");
            redraw ();
            break;
         default:
            break;
      }
   }

}

/* ---------------------- END OF FILE ------------------------- */
