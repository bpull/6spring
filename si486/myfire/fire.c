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
int n;
int** board[2];

//create a board of the size requested with an extra row (the halo) on all sides
void initBoard()
{
        int b, i, j;

        for(b=0; b<2; b++)
        {
                board[b] = calloc((size_t)process_grid_size, sizeof(int*));
                for(i=0; i<process_grid_size; i++)
                {
                        board[b][i] = calloc((size_t)process_grid_size, sizeof(int));
                        for(j=0; j<process_grid_size; j++)
                        {
                                board[b][i][j] = rank;
                        }
                }
        }
}

//populate the board for the first time
void populateBoard(int num)
{
        int i, j;
        for(i=0; i<process_grid_size; i++)
        {
                for(j=0; j<process_grid_size+2; j++)
                {
                        board[0][i][j] = rank;
                }
        }
}


// //the print board function will pretty print the playable board (minus the halo)
// void printBoard(int b)
// {
//
//         //first corner
//         printf("\n  +");
//
//         //top border
//         int length = 0;
//         while(length < x)
//         {
//                 printf("-");
//                 length++;
//         }
//
//         //second corner
//         int height = 0;
//         printf("+\n");
//
//         //loop to print out the insides of the board
//         while(height < y)
//         {
//                 printf("  |");
//                 for(length=0; length<x; length++)
//                 {
//                         printf("%c", board[b][length+1][height+1]);
//                 }
//                 printf("|\n");
//                 height++;
//         }
//
//         //bottom corner, row and last corner
//         printf("  +");
//         length = 0;
//         while(length < x)
//         {
//                 printf("-");
//                 length++;
//         }
//         printf("+\n\n");
// }

void calculateGridSize(int totalSize){
        int math;
        math = sqrt(np);
        math = totalSize/math;
        process_grid_size = math;
        n = sqrt(process_grid_size);
}

void left_ex(int b, int odd){
        int i, j;
        int rneighbor = rank+1;
        int lneighbor = rank-1;

        if(rank % n == 0) {
                return;
        }
        else{
                if(odd) {
                        //even rank
                        if (rank & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][process_grid_size-1][i], 1, MPI_INT, rneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }

                        //odd rank
                        if(rank & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][0][i], 1, MPI_INT, lneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                }

                else{
                        //even rank
                        if(rank & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][0][i], 1, MPI_INT, lneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                        //odd rank
                        if (rank & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][process_grid_size-1][i], 1, MPI_INT, rneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }
                }
        }
}

void right_ex(int b, int even){
        int i, j;
        int rneighbor = rank+1;
        int lneighbor = rank-1;

        if(rank % n == (n-1)) {
                return;
        }
        else{
                if(even) {
                        //even rank
                        if (rank & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][process_grid_size-1][i], 1, MPI_INT, rneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                        //odd rank
                        if(rank & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][0][i], 1, MPI_INT, lneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }

                }
                else{
                        //even rank
                        if(rank & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][0][i], 1, MPI_INT, lneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }
                        //odd rank
                        if (rank & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][process_grid_size-1][i], 1, MPI_INT, rneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                }
        }
}

void top_ex(int b, int odd){
        int i, j;
        int tneighbor = rank-n;
        int bneighbor = rank+n;

        if(rank - n < 0)
                return;
        else{
                if(odd) {
                        //even rank
                        if (rank/n & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][i][process_grid_size-1], 1, MPI_INT, bneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }
                        //odd rank
                        if(rank & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][i][0], 1, MPI_INT, tneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                }
                else{
                        //odd rank
                        if (rank/n & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][i][process_grid_size-1], 1, MPI_INT, bneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }

                        //even rank
                        if(rank & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][i][0], 1, MPI_INT, tneighbor, rank, MPI_COMM_WORLD);
                                }
                        }
                }
        }
}


void bottom_ex(int b, int even){
        int i, j;
        int tneighbor = rank-n;
        int bneighbor = rank+n;

        if(rank + n >= np)
                return;
        if(even) {
                //even row
                if (rank/n & 1 == 0) {
                        for(i = 0; i < process_grid_size; i++) {
                                MPI_Send(&board[b][i][process_grid_size-1], 1, MPI_INT, bneighbor, rank, MPI_COMM_WORLD);
                        }
                }

                //odd row
                if(rank/n & 1) {
                        for(i = 0; i < process_grid_size; i++) {
                                MPI_Recv(&board[b][i][0], 1, MPI_INT, tneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        }
                }
        }
        else{
                //dont do if bottom row
                if(rank + n >= np) {
                        //odd row
                        if (rank/n & 1) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Send(&board[b][i][process_grid_size-1], 1, MPI_INT, bneighbor, rank, MPI_COMM_WORLD);
                                }
                        }

                        //even rank
                        if(rank/n & 1 == 0) {
                                for(i = 0; i < process_grid_size; i++) {
                                        MPI_Recv(&board[b][i][0], 1, MPI_INT, tneighbor, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                }
                        }
                }
        }
}

int main(int argc, char* argv[]){
        //MPI init things
        int ierr;
        int gen;
        MPI_Init(&argc, &argv);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &np);
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        //ERROR CHECKING**************************
        //error check for correct program usage
        if(rank==0) {
                if(argc != 2) {
                        fprintf(stderr, "USAGE: ./fire <length> \n");
                        exit(1);
                }
        }
        //***************************************

        //create a 200x200 board for each process and fill each cell with its rank num
        calculateGridSize(atoi(argv[1]));
        initBoard();


        right_ex(0, 1);
        left_ex(0, 1);
        right_ex(0, 0);
        left_ex(0, 0);

        bottom_ex(0, 1);
        top_ex(0, 1);
        bottom_ex(0, 0);
        top_ex(0, 0);

        printf("rank %d: My board[0][199][199] is equal to: %d\n", rank, board[0][199][199]);


        MPI_Finalize();
}
