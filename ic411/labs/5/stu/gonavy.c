#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NTHREADS 10
void *army_function(void *);
void *navy_function(void *);

int statics[NTHREADS];      // will be initialized to 1,2,3...etc.
int army_scores[NTHREADS];     // will be initialized to 0
int navy_scores[NTHREADS];     // will be initialized to 0

int main()
{
   pthread_t thread_id[NTHREADS];
   int i, j;
   
   // Initialize the scores and the static numeric array
   for(i=0; i < NTHREADS; i++)
   {
      statics[i] = i;
      army_scores[i] = 0;
      navy_scores[i] = 0;
   }   

    /* The statics[] array is useful because &i may increment
    before it is referenced, resulting in the wrong thread_num being assigned.
    We use &statics[i] instead of &i to initalize the (common) thread number. */
    
    // TODO: Create the Army threads. 
   for(i=0; i < NTHREADS; i++)
     pthread_create(&thread_id[i], NULL, army_function, &statics[i]);

    // TODO: Join all Army threads when complete
   for(i=0; i < NTHREADS; i++)
     pthread_join( thread_id[i], NULL);
    
    // TODO: Create the Navy threads. 
   for(j=0; j < NTHREADS; j++)
     pthread_create(&thread_id[j], NULL, navy_function, &statics[j]);

    // TODO: Join all Navy threads when complete
   for(j=0; j < NTHREADS; j++)
     pthread_join( thread_id[j], NULL);
  
    // Reveal final scores
    printf("Final scores:\n");
    for(i=0; i < NTHREADS; i++)
    {
        printf("Thread:%d Navy:%d, Army:%d\n", i, navy_scores[i], army_scores[i]);
    }    
}

void *army_function(void * arg)
{
    int thread_num = * (int *) arg;
    // thread_num is a "common" thread index between 0 and (NTHREADS-1)
    
    // TODO: Give Army a random integer score between 0 and 100
    srand(time(NULL)*(thread_num+1));


    int random_num;
    random_num = rand() % 100;
    army_scores[thread_num] = random_num;

    return 0;
}

void *navy_function(void * arg)
{
    int thread_num = * (int *) arg;
    // thread_num is a "common" thread index between 0 and (NTHREADS-1)
    
    // TODO: Give Navy double the score of Army for this thread
    navy_scores[thread_num] = army_scores[thread_num] * 2;
    return 0;
}
