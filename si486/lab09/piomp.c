#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

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

    #pragma omp parallel for shared(insiders)
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
    int myworld = omp_get_num_threads();
    long ncount = atoi(argv[1]);
    long sumIn;
    long ans;
    double mypi;
    int i;

    // do your work
    sumIn = ans = throwdarts(ncount);

    printf("%ld / %ld * 4 / % d ", sumIn, ncount, myworld);
    mypi = ((double)sumIn/(double)ncount) * 4.0 / (double) myworld;
    printf("my pi is: %20.18f\n", mypi);

} // main
