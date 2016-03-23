#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

/*
 * This set of exercises aims at showing some basic OpenMP directives and how
 * they affect the order (and correctness) of execution.
 *
 * This exercise tries to illustrate a simple parallel OpenMP
 * program. Observe that the output is "wrong" (thread IDs do not match).
 * Why is that? Correct the program so that it is "correct".
 *
 */

int main(int argc, char **argv) {

    int i, j, tid, n, *buffer;
    time_t ts;
    FILE *fp;

    /*--------------------------------------------------------------------------*/
    printf("\n   ----- Exercise 1 ------\n\n");

    #pragma omp parallel private(i, j, tid)
    {
      for (i = 0; i < 1000; i++)
	     for (j = 0; j < 1000; j++)
	        tid = omp_get_thread_num();

    #pragma omp barrier

    printf("Thread %d : My value of tid (thread id) is %d\n",
				    omp_get_thread_num(), tid);
    }


    printf("\n   ----- End of exercise 1 ------\n\n");
    /*--------------------------------------------------------------------------*/
    return 0;

} // main
