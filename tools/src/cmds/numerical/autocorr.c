
#include "fileio.h"
#include "var.h"
#include <math.h>

#define XGRAPH
#define NSPEC 300

/* ========================================================== */
/* 
 * This program reads the indicated file, computes the auto-correlation
 * of the data, and dumps the results to stdout.
 *
 * Linas Vepstas  December 1993
 */

#ifdef ANSI_C
int main (int argc, char *argv[])
#else
int main (argc, argv)
int argc;
char *argv[];
#endif
{
   char *file_name, *xlabel, *ylabel;
   int i, j;
   double *xdata, *ydata;
   int npoints;
   double timeDelta, S;
   double timeDeltaZero, timeDeltaMax;
   double logtimeDelta;
   double scale;
   double center, width;
   double step, diff;
   int delta;

   /* Usage */
   if (argc < 4) {
      fprintf (stderr, 
          "Usage: %s <filename> <x-axis label> <y-axis label> [<y axis> ...] \n", argv[0]);
      exit (EXIT_FAILURE);
   }

   /* get the filename */
   file_name = argv[1];
   xlabel = argv[2];

   /* loop over all files */
   for (i=3; i<argc; i++) {
      ylabel = argv[i];

#ifdef XGRAPH
      /* print a data set label for xgraph */
      printf ("\"%s\n", ylabel);
#endif /* XGRAPH */

      npoints = read_data (file_name, xlabel, ylabel, &xdata, &ydata);

#ifdef LOGLOG
      /* The smallest interesting timeDelta is one unit (the correlation
       * at zero units should be unity).  The largest interesting time 
       * delta is, say, arbitrarily, half the length of the total sample.
       */
      timeDeltaZero = 1.0;
      timeDeltaMax = 0.5 * ((double) npoints);

      /* to get evenly spaced points on a log graph, thake the ratio of
       * the endpoints of the graph, and take nth root, where n is the 
       * number of graph points. */
      scale = pow ((timeDeltaMax / timeDeltaZero), (1.0/((double) NSPEC)));

      timeDelta = timeDeltaZero;

      /* compute derivative varience  */
      for (j=0; j<NSPEC; j++) {
         timeDelta *= scale;

         delta = timeDelta;
         S = AutoCorrelation (ydata, npoints, delta); 
#ifdef SMOOTH
         center = npoints / 2;
         width = npoints / 8;
         S = GAutoCorrelation (ydata, npoints, delta, center, width);
#endif /* SMOOTH */

         /* autocovariance could easily be negative */
         if (S > 0.0) { S = log10 (S); } else { S=0.0; }
         logtimeDelta = log10 ((double) delta);

         printf ("%f %f \n", logtimeDelta, S);
         fflush (stdout);
      }
#endif /* LOGLOG */

      /* The least we should overlap is about half the data sample */
      step = (double) npoints / ((double) (2*NSPEC));

      /* compute autocorrelation */
      diff = 0.0;
      for (j=0; j<NSPEC; j++) {

         delta = (int) diff;
         S = AutoCorrelation (ydata, npoints, delta); 

         printf ("%f %f \n", (double) delta, S);
         fflush (stdout);
         diff += step;
      }

#ifdef XGRAPH
      /* print a blank line for xgraph */
      printf ("\n");
#endif /* XGRAPH */
      fflush (stdout);

   }

   exit (0);
}
/* ====================== END OF FILE ===================== */
