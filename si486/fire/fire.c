#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include "mpi.h"

int rank;
int np;
int process_grid_size;

void calculateGridSize(int totalSize){
        int math;
        math = sqrt(np);
        math = totalSize/math;
        process_grid_size = math;
}

int main(int argc, char* argv[]){
        //ERROR CHECKING**************************
        //error check for correct program usage
        if(argc < 2)
        {
                fprintf(stderr, "USAGE: ./fire <length> \n");
                exit(1);
        }
        //***************************************

        //MPI init things
        int ierr;
        int gen;

        MPI_Init(&argc, &argv);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &np);
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);


        calculateGridSize(atoi(argv[1]));
        printf("%d\n", process_grid_size);

        MPI_Finalize();
}
