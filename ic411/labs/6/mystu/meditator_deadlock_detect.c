#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define NUM_MEDITATORS		24
#define NUM_KEYS			3
#define NUM_MATS			3
#define NUM_BOOKS			3
#define RESOURCE_COUNT		3	// keys, mats, books
#define SHORT_WAIT			1
#define MEDITATE			2
#define KEY					0
#define MAT					1
#define BOOK				2

/*==========================================================================*/

/* Macros to encapsulate POSIX semaphore functions so they look like Stallings text code. Examples:
 * semaphore my_sem;
 * semInit(my_sem,1);
 * semWait(my_sem);
 * semSignal(my_sem);
 * semRelease(my_sem);
 * */
#define semInit(s,v)	sem_init( &s, 0, v )
#define semWait(s)		sem_wait( &s )
#define semSignal(s)	sem_post( &s )
#define semRelease(s)	sem_destroy( &s )
typedef sem_t semaphore;


/* Data structures to track shared resources. The number in each array represents the number of the meditator thread holding the resource. When not in use, set to -1. */
int keys[NUM_KEYS];
int mats[NUM_MATS];
int books[NUM_BOOKS];
/* Data structures to track shared resources. The number in each array represents the number of the key, mat, or book held by that meditator thread. When not in use, set to -1. */
int has_key[NUM_MEDITATORS];
int has_mat[NUM_MEDITATORS];
int has_book[NUM_MEDITATORS];

/* Deadlock detection data structures */
int Q[NUM_MEDITATORS][RESOURCE_COUNT] = {{0}};
int A[NUM_MEDITATORS][RESOURCE_COUNT] = {{0}};
int R[RESOURCE_COUNT] = {NUM_KEYS, NUM_MATS, NUM_BOOKS};
int V[RESOURCE_COUNT] = {0};
int W[RESOURCE_COUNT] = {0};


/*==========================================================================*/

int get_key(int n)
{
	int i = n % NUM_KEYS;
	while (keys[i] != -1) sleep(SHORT_WAIT);
	has_key[n] = i;
	keys[i] = n;	// Thread n finds key i available
	printf("meditator %i got key %i\n",n, i); fflush(stdout);
	Q[n][KEY] = 0;
	A[n][KEY]= 1;
	return i;
}

int get_mat(int n)
{
	int i = n % NUM_MATS;
	while (mats[i] != -1) sleep(SHORT_WAIT);
	has_mat[n] = i;
	mats[i] = n;	// Thread n finds mat i available
	printf("meditator %i got mat %i\n",n, i); fflush(stdout);
	Q[n][MAT] = 0;
	A[n][MAT]= 1;
	return i;
}

int get_book(int n)
{
	int i = n % NUM_BOOKS;
	while (books[i] != -1) sleep(SHORT_WAIT);
	has_book[n] = i;
	books[i] = n;	// Thread n finds book i available
	printf("meditator %i got book %i\n",n, i); fflush(stdout);
	Q[n][BOOK] = 0;
	A[n][BOOK]= 1;
	return i;
}

int drop_key(int n)
{
	int key_num = has_key[n];
	keys[key_num] = -1;
	has_key[n] = -1;
	A[n][KEY]= 0;
	return key_num;
}

int drop_mat(int n)
{
	int mat_num = has_mat[n];
	mats[mat_num] = -1;
	has_mat[n] = -1;
	A[n][MAT]= 0;
	return mat_num;
}

int drop_book(int n)
{
	int book_num = has_book[n];
	books[book_num] = -1;
	has_book[n] = -1;
	A[n][BOOK]= 0;
	return book_num;
}
/*==========================================================================*/

void * deadlock_detector(void * t_num)
{
	bool deadlock = false;	// True if two or more threads are deadlocked
	bool all_zero;  	// True if a thread has no allocations in A
	bool can_run;		// True if a thread's Q values are all less than W
	bool some_can_run;	// True if at least one thread was able to run this loop
	bool all_analyzed;	// True if every thread has been processed by the algorithm
	bool deadlocked[NUM_MEDITATORS] = {false};	// True if a thread is deadlocked
	bool analyzed[NUM_MEDITATORS] = {false};	// True if a thread has been processed
	int Q_snapshot[NUM_MEDITATORS][RESOURCE_COUNT] = {{0}};	// A snapshot of Q
	int A_snapshot[NUM_MEDITATORS][RESOURCE_COUNT] = {{0}}; // A snapshot of A
	int i,j,sum;

	while (!deadlock) {

		// Snapshot the requests Q and allocations A (since they change)
		memcpy(Q_snapshot, Q, sizeof (int) * NUM_MEDITATORS * RESOURCE_COUNT);
		memcpy(A_snapshot, A, sizeof (int) * NUM_MEDITATORS * RESOURCE_COUNT);

		// Populate V
		/* TODO done: Populate the available vector V, using data from the
		snapshot of A, and from the Resouce vector, R */
		for(i=0; i<RESOURCE_COUNT; i++){
			int added = 0;

			for(j=0; j<NUM_MEDITATORS; j++){
				added += A_snapshot[j][i];
			}

		 	V[i] = R[i] - added;
		}
		// W <- V (copy V into the Temporary vector, W)
		memcpy(W, V, sizeof (int) * RESOURCE_COUNT);

		// Look for a process that can run, and run it. If none exists, deadlock.
		some_can_run = true;
		all_analyzed = false;
		while (some_can_run || !all_analyzed) {

			some_can_run = false;
			for (i=0; i<NUM_MEDITATORS; i++) { // Loop over threads using i


				// See if a thread has nothing allocated in A.
				// If so, set all_zero to true.
				all_zero = true;
				for(j=0; j<RESOURCE_COUNT; j++)
				{
					if(A[i][j] != 0)
						all_zero = false;
				}

				if (all_zero) {
				//TODO done: Mark this thread i as analyzed but not deadlocked.
					analyzed[i] = true;
					deadlocked[i] = false;
				}
				else {
				// TODO: Check if a thread's requests Q are all < W.
				// If so, 'run it':
				//	- Mark as analyzed
				//  - Add its allocations in A_snapshot to W
				//  - Zero out its A and Q rows
				//  - Note that it's not deadlocked
				//  - Note that 'some' thread can run at the moment
				// Otherwise, mark it as analyzed and deadlocked
					can_run = true;
					for(j=0; j<RESOURCE_COUNT; j++){
							can_run = can_run && (Q[i][j] < W[j]);
					}
					if(can_run)
					{
						analyzed[i] = true;
						for(j=0; j<RESOURCE_COUNT; j++){
							W[j] += A_snapshot[i][j];
							A[i][j] = 0;
							Q[i][j] = 0;
						}
						deadlocked[i] = false;
						some_can_run = true;
					}
					else{
						analyzed[i] = true;
						deadlocked[i] = true;
					}
				}
			}

			// Check if all analyzed
			// TODO done: if all threads have been analyzed, set all_analyzed = true
			all_analyzed = true;
			for(i=0; i<NUM_MEDITATORS; i++)
			{
				if(analyzed[i] == false)
					all_analyzed = false;
			}

		} // End 'while (some_can_run || !all_analyzed)'

		// Do some threads remain deadlocked?
		deadlock = false;
		for (i=0; i<NUM_MEDITATORS; i++) deadlock = deadlock || deadlocked[i];

		// Print a report
		printf(deadlock ? "  **Deadlock detected!**\n" : "  **No deadlock**\n");
		if (deadlock) {

			/* print Q and A for deadlocked processes */
			printf("             Q        A\n");
			for (i=0; i<NUM_MEDITATORS; i++) {
				if (deadlocked[i]) {
					printf("thread %2i: ",i);
					for (j=0; j<RESOURCE_COUNT; j++) printf("%1i ",Q_snapshot[i][j]);
					printf("   ");
					for (j=0; j<RESOURCE_COUNT; j++) printf("%1i ",A_snapshot[i][j]);
					printf("\n");
				}
			}

			/* Print R, V, W */
			printf("\nR: ");
			for (i=0; i<RESOURCE_COUNT; i++) printf("%1i ",R[i]);
			printf("   V: ");
			for (i=0; i<RESOURCE_COUNT; i++) printf("%1i ",V[i]);
			printf("   W: ");
			for (i=0; i<RESOURCE_COUNT; i++) printf("%1i ",W[i]);
			printf("\n");
			return 0;
		}

		sleep(MEDITATE);

	} // End 'while !deadlock'

}
/*==========================================================================*/

void * meditator(void * t_num)
/* Simulate a meditator. Each meditator must obtain the correct room key, meditation mat, and spritual book before meditating. The correct number for each is the meditator number modulo the number of that resource (e.g., n % num_keys). This function is called via pthread_create(). The argument is a pointer to an array of meditator numbers. Meditator threads randomly choose which resource to seek first. */
{
    int n = * (int *) t_num;

    Q[n][KEY] = 1; Q[n][MAT] = 1; Q[n][BOOK] = 1; // Update request matrix Q;

    /* Get Supplies */
    int r = rand() % 3;
    if (r == 0) {
		printf("meditator %i getting key %i\n", n, n % NUM_KEYS); fflush(stdout);
		get_key(n);

		printf("meditator %i getting mat %i\n", n, n % NUM_MATS); fflush(stdout);
		get_mat(n);

		printf("meditator %i getting book %i\n", n, n % NUM_BOOKS); fflush(stdout);
		get_book(n);
	}
	else if (r == 1) {
		printf("meditator %i getting mat %i\n", n, n % NUM_MATS); fflush(stdout);
		get_mat(n);

		printf("meditator %i getting book %i\n", n, n % NUM_BOOKS); fflush(stdout);
		get_book(n);

		printf("meditator %i getting key %i\n", n, n % NUM_KEYS); fflush(stdout);
		get_key(n);
	}
	else {
		printf("meditator %i getting book %i\n", n, n % NUM_BOOKS); fflush(stdout);
		get_book(n);

		printf("meditator %i getting key %i\n", n, n % NUM_KEYS); fflush(stdout);
		get_key(n);

		printf("meditator %i getting mat %i\n", n, n % NUM_MATS); fflush(stdout);
		get_mat(n);
	}

	sleep(MEDITATE);

	/* Release Supplies  and finish */
	printf("meditator %i released book %i\n", n, drop_book(n)); fflush(stdout);

	printf("meditator %i released mat %i\n", n, drop_mat(n)); fflush(stdout);

	printf("meditator %i released key %i\n", n, drop_key(n)); fflush(stdout);

	printf("meditator %i ........... done\n", n); fflush(stdout);

}

/*==========================================================================*/

int main( int argc, char *argv[] )
{

    int i;
    pthread_t med[NUM_MEDITATORS];
    pthread_t deadlock_detect_thread;
    int statics[NUM_MEDITATORS];

	srand(time(NULL));

    for ( i = 0; i < NUM_KEYS; i++ ) keys[i] = -1;
    for ( i = 0; i < NUM_MATS; i++ ) mats[i] = -1;
    for ( i = 0; i < NUM_BOOKS; i++ ) books[i] = -1;

    /* Initialize data. Create a thread for each meditator. */
    for ( i = 0; i < NUM_MEDITATORS; i++ )
    {
		statics[i] = i;
		has_key[i] = -1;
		has_mat[i] = -1;
		has_book[i] = -1;
		pthread_create( &med[i], NULL, meditator, &statics[i] );
    }

    /* Create deadlock detection thread */
    pthread_create( &deadlock_detect_thread, NULL, deadlock_detector, NULL);

    /* Wait for all meditators to finish. */
    for ( i = 0; i < NUM_MEDITATORS; i++ ) pthread_join( med[i], NULL );

    /* Wait for detector to finish. */
    pthread_join(deadlock_detect_thread, NULL);

    /* Produce the final report. */
    printf( "All meditators have achieved nirvana\n" );

    return 0;
}
