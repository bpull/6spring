#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

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
    int myworld, myrank;
    MPI_Status status;
    int count;
    int src;
    int tag;
    int dest;
    long ncount;
    long sumIn;
    long ans;
    double mypi;
    int i;

    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &myworld);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (myrank != 0) {
	tag=1;
	src = 0;
	// printf("%2d: waiting for msg from %2d\n", myrank, src);
	MPI_Recv(&ncount, 1, MPI_LONG, src, tag, MPI_COMM_WORLD, &status);
	// printf("%2d: received count: %ld\n", myrank, ncount);
	srand(myrank);	// seed the rnd#gen w/ unique rank
	ans = throwdarts(ncount);
	dest=0;
	tag=2;
	// printf("%2d: sending ans to %2d\n", myrank, src);
	MPI_Send(&ans, 1, MPI_LONG, dest, tag, MPI_COMM_WORLD);
	// printf("%2d: sent.\n", myrank);
    } else {
	// tell everyone how much work to do
	tag=1;
	ncount = atol(argv[1]);
	for(i=1; i<myworld; i++) {
	    // printf("%2d: sending msg to %2d\n", myrank, src);
	    MPI_Send(&ncount, 1, MPI_LONG, i, tag, MPI_COMM_WORLD);
	    // printf("%2d: Sent.\n", myrank);
	}

	// do your work
	sumIn = ans = throwdarts(ncount);

	// get a msg from everyone else
	tag=2;
	for(i=1; i<myworld; i++) {
	    src=i;
	    // printf("%2d: waiting for msg from %2d\n", myrank, src);
	    MPI_Recv(&ans, 1, MPI_LONG, src, tag, MPI_COMM_WORLD, &status);
	    sumIn += ans;
	}
	printf("%ld / %ld * 4 / % d ", sumIn, ncount, myworld);
	mypi = ((double)sumIn/(double)ncount) * 4.0 / (double) myworld;
	printf("my pi is: %20.18f\n", mypi);
    }


   MPI_Finalize();

} // main
