/*
 * recv buffer from myrank-1		// recv LHShalo from myrank-1
 * copy buffer into RHS halo		// recv RHShalo from myrank+1
 * recv buffer from myrank+1		// recv RHShalo from myrank+1
 * copy buffer into LHS halo column	// recv LHShalo from myrank-1
 * send buffer to rank myrank-1		// send LHSdata to myrank-1
 * copy RHS data into buffer		// send RHSdata to myrank+1
 * send buffer to rank myrank+1		// send RHSdata to myrank+1
 * copy LHS column data into buffer	// send LHSdata to myrank-1
 */
#include "mpi.h"

#define LFT 8
#define RIT 2
#define TOP 4
#define BOT 16

extern int myrank;
extern char *bufr;

void
recvCol(char **full, int bdim, int LoR, int gridn)
{
    int i;
    int col;
    int src;
    MPI_Status mpistatus;

    // on the edge; don't expect to recv anything
    if ((LoR == LFT) && ((myrank % gridn) == 0)) { return; }
    if ((LoR == RIT) && ((myrank % gridn) == (gridn - 1))) { return; }

    src = (LoR == LFT)? myrank-1 : myrank+1;
    MPI_Recv(bufr, bdim, MPI_CHAR, src, 1, MPI_COMM_WORLD, &mpistatus);
    
    // copy bufr into halo
    col = (LoR == LFT) ? 0 : bdim-1;
    for(i=0; i<bdim; i++) {
	full[col][i] = bufr[i];
    }

} // recvCol

void
sendCol(char **full, int bdim, int LoR, int gridn)
{
    int i;
    int col;
    int dest;

    // on the edge; don't send to non-existant neighbor
    if ((LoR == LFT) && ((myrank % gridn) == 0)) { return; }
    if ((LoR == RIT) && ((myrank % gridn) == (gridn - 1))) { return; }

    // copy data into bufr to send
    col = ((LoR == LFT) ? 1 : bdim-2);
    for(i=0; i<bdim; i++) {
	bufr[i] = full[col][i];
    }
    dest = ((LoR == LFT)? myrank-1 : myrank+1);
    MPI_Send(bufr, bdim, MPI_CHAR, dest, 1, MPI_COMM_WORLD );

} // sendCol

void
sendRow(char **full, int bdim, int ToB, int gridn)
{
    int i;
    int row;
    int dest;

    dest = ((ToB == TOP)? (myrank-gridn) : (myrank+gridn));
    // don't send to non-existant neighbor
    if ((ToB == TOP) && (dest < 0)) { return; }
    if ((ToB == BOT) && (dest >= (gridn*gridn))) { return; }

    // copy data into bufr to send
    row = ((ToB == TOP) ? 1 : (bdim - 2));
    for(i=0; i<bdim; i++) {
	bufr[i] = full[i][row];
    }
    MPI_Send(bufr, bdim, MPI_CHAR, dest, 1, MPI_COMM_WORLD );

} // sendRow

recvRow(char **full, int bdim, int ToB, int gridn)
{
    int i;
    int row;
    int src;
    MPI_Status mpistatus;

    src = ((ToB == TOP) ? (myrank-gridn) : (myrank+gridn));
    // on the edge; don't expect to recv anything
    if ((ToB == TOP) && (src < 0)) { return; }
    if ((ToB == BOT) && (src >= (gridn*gridn))) { return; }

    MPI_Recv(bufr, bdim, MPI_CHAR, src, 1, MPI_COMM_WORLD, &mpistatus);
    
    // copy bufr into halo
    row = (ToB == TOP) ? 0 : (bdim - 1);
    for(i=0; i<bdim; i++) {
	full[i][row] = bufr[i];
    }

} // recvRow

LeftRightExch(char **board, int bdim, int gridn)
{
    if ((myrank & 1)  == 0) {		/* even rank */
	// send LHSdata to myrank-1
	sendCol(board, bdim, LFT, gridn);
	// recv LHShalo from myrank-1
	recvCol(board, bdim, LFT, gridn);

	// send RHSdata to myrank+1
	sendCol(board, bdim, RIT, gridn);
	// recv RHShalo from myrank+1
	recvCol(board, bdim, RIT, gridn);

    } else {				/* odd rank */
	// recv RHShalo from myrank+1
	recvCol(board, bdim, RIT, gridn);
	// send RHSdata to myrank+1
	sendCol(board, bdim, RIT, gridn);

	// recv LHShalo from myrank-1
	recvCol(board, bdim, LFT, gridn);
	// send LHSdata to myrank-1
	sendCol(board, bdim, LFT, gridn);
    }

} // LeftRightExch

TopBotExch(char **board, int bdim, int gridn)
{
    if (((myrank/gridn) & 1) == 0) {	/* even row */
	// send buffer to rank-n
	sendRow(board, bdim, TOP, gridn);
	// recv buffer from rank-n
	recvRow(board, bdim, TOP, gridn);

	// send buffer to rank+n
	sendRow(board, bdim, BOT, gridn);
	// recv buffer from rank+n
	recvRow(board, bdim, BOT, gridn);

    } else {				/* odd row */
	// recv buffer from rank+n
	recvRow(board, bdim, BOT, gridn);
	// send to rank+n
	sendRow(board, bdim, BOT, gridn);

	// recv buffer from rank-n
	recvRow(board, bdim, TOP, gridn);
	// send to rank-n
	sendRow(board, bdim, TOP, gridn);
    }

} // TopBotExch
