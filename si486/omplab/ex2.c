#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

/*
 * This set of exercises aims at showing some basic OpenMP directives and how
 * they affect the order (and correctness) of execution.
 *
 * This exercise illustrates some of the data-sharing clauses in
 * OpenMP. Run the program, is the output correct? Try to make the
 * program print the correct value for n for both  parallel sections.
 *
 */

int main(int argc, char **argv) {

    int i, j, tid, n, *buffer;
    time_t ts;
    FILE *fp;

    /*--------------------------------------------------------------------------*/
    printf("\n   ----- Exercise 2 ------\n\n");

    n = 10;
    #pragma omp parallel firstprivate(n)
    {
	n += omp_get_thread_num();
	printf("Thread %d : My value of n is %d \n", omp_get_thread_num(), n);
    }

    j = 100000;
    #pragma omp parallel for lastprivate(n)
    for (i = 1; i <= j; i++)
	n = i;

    printf("\nAfter %d iterations the value of n is %d \n\n", j, n);

    printf("\n   ----- End of exercise 2 ------\n\n");
    /*--------------------------------------------------------------------------*/

    return 0;

} // main
