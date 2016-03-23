#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

/*
 * This set of exercises aims at showing some basic OpenMP directives and how
 * they affect the order (and correctness) of execution.
 *
 * This exercise illustrate the different scheduling algorithms in
 * OpenMP. Run the program several times, with different values for
 * OMP_SCHEDULE. 
 *
 * After each run, execute gnuplot schedule.gp, and look at
 * schedule.png to see how the schedule assigned parts of the
 * iteration to different threads.
 */

int main(int argc, char **argv) {

    int i, j, tid, n, *buffer;
    time_t ts;
    FILE *fp;

    /*--------------------------------------------------------------------------*/
    printf("\n   ----- Exercise 4 ------\n\n");

    n = 1000;
    buffer = malloc(n * sizeof(int));

    printf("Computing...");
    fflush(stdout);

    #pragma omp parallel for schedule(runtime)
    for (i = 0; i < n; i++) {
	buffer[i] = omp_get_thread_num();    
	usleep(random()%2000); 
    }
    printf("Done\n");
    fp = fopen("schedule.dat","w");

    for (i = 0; i < n; i++) {
	fprintf(fp, "%d\t%d\n", i, buffer[i]);
    }

    fclose(fp);

    printf("Now, run 'gnuplot schedule.gp' to visualize the scheduling policy\n");
    printf("The output from gnuplot will be put in schedule.png\n");
    free(buffer);

    printf("\n   ----- End of exercise 4 ------\n\n");
    /*--------------------------------------------------------------------------*/

    return 0;
}
