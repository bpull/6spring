#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * approximate pi by monte carlo methods
 */

long
throwdarts(long iters)
{
  long l;
  long insiders=0;
  double x, y;
  double h;		// hypotenuse

  for(l=0; l < iters; l++) {
    x = ((double) rand()) / RAND_MAX;
    y = ((double) rand()) / RAND_MAX;
    // pythagorean theorem for hypotenuse
    // c^2 = a^2 + b^2
    // c=sqrt(a^2+b^2)
    h = sqrt(x*x+y*y);
    if (h > 1.0) { continue; }
    insiders++;
  }

  return insiders;
} // throwdarts

main(int argc, char ** argv)
{
  int myworld;
  long ncount;
  long sumIn;
  long ans;
  double mypi;
    
  srand(time(NULL));	// seed the rnd#gen w/ unique rank

  ncount = atol(argv[1]);

  // do your work
  sumIn = ans = throwdarts(ncount);

  printf("%ld / %ld * 4 ", sumIn, ncount);
  mypi = ((double)sumIn/(double)ncount) * 4.0;
  printf("my pi is: %20.18f\n", mypi);
}
