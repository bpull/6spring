#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define NUM_MEDITATORS		24
#define NUM_KEYS			3
#define NUM_MATS			3
#define NUM_BOOKS			3
#define SHORT_WAIT			1
#define MEDITATE			5

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

// TODO: declare semaphore(s) globally
semaphore key_semaphore;
semaphore mat_semaphore;
semaphore book_semaphore;

/*==========================================================================*/

int get_key(int n)
{
	int i = n % NUM_KEYS;
	while (keys[i] != -1) sleep(SHORT_WAIT);
	has_key[n] = i;
	keys[i] = n;	// Thread n finds key i available
	printf("meditator %i got key %i\n",n, i); fflush(stdout);
	return i;
}

int get_mat(int n)
{
	int i = n % NUM_MATS;
	while (mats[i] != -1) sleep(SHORT_WAIT);
	has_mat[n] = i;
	mats[i] = n;	// Thread n finds mat i available
	printf("meditator %i got mat %i\n",n, i); fflush(stdout);
	return i;
}

int get_book(int n)
{
	int i = n % NUM_BOOKS;
	while (books[i] != -1) sleep(SHORT_WAIT);
	has_book[n] = i;
	books[i] = n;	// Thread n finds book i available
	printf("meditator %i got book %i\n",n, i); fflush(stdout);
	return i;
}

int drop_key(int n)
{
	int key_num = has_key[n];
	keys[key_num] = -1;
	has_key[n] = -1;
	return key_num;
}

int drop_mat(int n)
{
	int mat_num = has_mat[n];
	mats[mat_num] = -1;
	has_mat[n] = -1;
	return mat_num;
}

int drop_book(int n)
{
	int book_num = has_book[n];
	books[book_num] = -1;
	has_book[n] = -1;
	return book_num;
}

/*==========================================================================*/

void * meditator(void * t_num)
/* Simulate a meditator. Each meditator must obtain the correct room key, meditation mat, and spritual book before meditating. The correct number for each is the meditator number modulo the number of that resource (e.g., n % num_keys). This function is called via pthread_create(). The argument is a pointer to an array of meditator numbers. Meditator threads randomly choose which resource to seek first. */
{
    int n = * (int *) t_num;

	semWait(key_semaphore);
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

	//sleep(MEDITATE);

	/* Release Supplies  and finish */
	printf("meditator %i released book %i\n", n, drop_book(n)); fflush(stdout);

	printf("meditator %i released mat %i\n", n, drop_mat(n)); fflush(stdout);

	printf("meditator %i released key %i\n", n, drop_key(n)); fflush(stdout);

	printf("meditator %i ........... done\n", n); fflush(stdout);

	semSignal(key_semaphore);
}

/*==========================================================================*/

int main( int argc, char *argv[] )
{

    int i;
    pthread_t med[NUM_MEDITATORS];
    int statics[NUM_MEDITATORS];

	srand(time(NULL));

    for ( i = 0; i < NUM_KEYS; i++ ) keys[i] = -1;
    for ( i = 0; i < NUM_MATS; i++ ) mats[i] = -1;
    for ( i = 0; i < NUM_BOOKS; i++ ) books[i] = -1;

    /* TODO: Initialize needed semaphore(s). */
    semInit(key_semaphore, 1);

    /* Initialize data. Create a thread for each meditator. */
    for ( i = 0; i < NUM_MEDITATORS; i++ )
    {
		statics[i] = i;
		has_key[i] = -1;
		has_mat[i] = -1;
		has_book[i] = -1;
		pthread_create( &med[i], NULL, meditator, &statics[i] );
    }

    /* Wait for all meditators to finish. */
    for ( i = 0; i < NUM_MEDITATORS; i++ ) pthread_join( med[i], NULL );

    /* TODO: Release semaphores. */
	semRelease(key_semaphore);


    /* Produce the final report. */
    printf( "All meditators have achieved nirvana\n" );

    return 0;
}
