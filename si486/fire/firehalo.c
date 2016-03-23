#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "mpi.h"
/*
 *  Forest Fire Simulation
 *  Phase I. Halo Exchange
 */
extern void LeftRightExch();
extern void TopBotExch();

// globals
int myworld;
int myrank;
char *bufr;	// scratchpad the size of one side of the board

// reset each iteration and tracked to see if board changes
static int isChanging=1;

// (debug) print a board
void
prBoard(char **board, int size)
{
    int i, j;
    if (myrank == 2) {
	for(i=0;i<size;i++) {
	    for(j=0;j<size;j++) {
		printf("%d ", board[i][j]);
	    }
	    printf("\n");
	}
    }
} // prBoard

/*
 * sum up the moore's neighborhood
 */
int
mornaybrs(char **board, int z)
{
    int total = 0;
    total+=board[z-1][z-1];
    total+=board[z  ][z-1];
    total+=board[z+1][z-1];
    total+=board[z-1][z  ];
    total+=board[z  ][z  ];
    total+=board[z+1][z  ];
    total+=board[z-1][z+1];
    total+=board[z  ][z+1];
    total+=board[z+1][z+1];
    return total;
} // mornaybrs

haloExchange(char **board, int bdim, int gridn)
{
    LeftRightExch(board, bdim, gridn);
    TopBotExch(board, bdim, gridn);
} // haloExchange

main(int argc, char ** argv)
{
    int n;	// dim. of ranks
    int i,j,k,b;
    int sum;
    int totsum;
    int size;
    char **myboard[2];

    if (argc < 2) {
	fprintf(stderr, "usage: %s size\n", argv[0]);
	exit(1);
    }

    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &myworld);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // identtify yourself
    // printf("rank=%d\n", myrank);
    n = (int)sqrt(myworld);
    size = atoi(argv[1])+2;	// +2 for halo

    // the scratch buffer is one row or column + halo cells
    bufr = calloc(size, sizeof(char));

    // allocate the storage for both boards
    for(k=0; k<2; k++) {
	myboard[k] = calloc(size, sizeof(char *));
	for(i=0; i<size; i++) {
	    myboard[k][i] = calloc(size, sizeof(char));
	} //next i

	// test scenario - fill both boards
	for(i=1; i<(size-1); i++) {
	    for(j=1; j<(size-1); j++) {
		myboard[k][i][j] = myrank;
	    } // next j
	} //next i
    } // next k


    // for each iteration:
    for(b=0 ; isChanging ; b=1-b) {	// switch between boards
	printf("rank=%d ready to exchange.\n", myrank);
	// halo exchange
	haloExchange(myboard[b], size, n);
	printf("rank=%d exchanged.\n", myrank);
	isChanging = 0;	// may be updated in computing the next iteration
	// compute the next
	// TODO
	// periodically, show results?
    }

    // test scenario
    sum = mornaybrs(myboard[0], size-2);
    MPI_Reduce(&sum, &totsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (myrank == 0) { printf("total sum = %d\n", totsum); }

    MPI_Finalize();
} // main
