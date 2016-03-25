#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

/*
 * This set of exercises aims at showing some basic OpenMP directives and how
 * they affect the order (and correctness) of execution.
 *
 * This exercise tries to illustrate the usage of rudimentary OpenMP
 * synchronization constructs. Try to make all the threads end at the
 * same time.
 *
*/

int main(int argc, char **argv) {

    int i, j, tid, n, *buffer;
    time_t ts;
    FILE *fp;

    /*--------------------------------------------------------------------------*/
    printf("\n   ----- Exercise 3 ------\n\n");

    #pragma omp parallel private(ts, tid)
    {
	tid = omp_get_thread_num();
	time(&ts);

	printf("Thread %d spawned at:\t %s", tid, ctime(&ts));
	sleep(1);
	if(tid%2 == 0)
	sleep(5);

    #pragma omp barrier
	time(&ts);
	printf("Thread %d done at:\t %s", tid, ctime(&ts));
    }

    printf("\n   ----- End of exercise 3 ------\n\n");
    /*--------------------------------------------------------------------------*/

  return 0;
}
