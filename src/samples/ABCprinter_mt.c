/* based_mt_task.c -- A basic multi-threaded real-time task skeleton. 
 *
 * This (by itself useless) task demos how to setup a multi-threaded LITMUS^RT
 * real-time task. Familiarity with the single threaded example (base_task.c)
 * is assumed.
 *
 * Currently, liblitmus still lacks automated support for real-time
 * tasks, but internaly it is thread-safe, and thus can be used together
 * with pthreads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include gettid() */
#include <sys/types.h>

/* Include threading support. */
#include <pthread.h>

/* Include semaphores support */
#include <semaphore.h>

/* Include the LITMUS^RT API.*/
#include "litmus.h"

#define PERIOD1           100
#define PERIOD2           800
#define RELATIVE_DEADLINE 50
#define EXEC_COST         10

/* Let's create 10 threads in the example, 
 * for a total utilization of 1.
 */
#define NUM_THREADS      2 
#define MAX_VALUE		 3000000

/* The information passed to each thread. Could be anything. */
struct thread_context {
	int id;
};

/* The real-time thread program. Doesn't have to be the same for
 * all threads. Here, we only have one that will invoke job().
 */
void* rt_thread(void *tcontext);

/* Declare the periodically invoked job: increment shared_a by step of shared_b 
 * value and store the result in shared a. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAPlusBBasic(void);

/* Declare the periodically invoked job: multiply shared_a by shared_c 
 * and store the result in shared_b. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAMultiplyByCBasic(void);

int shared_a; // the shared variable to read/write.
int shared_b; // the shared variable to read/write.
int shared_c; // the shared variable to read.
sem_t mutex_a;  // mutex to securize shared_a access.
sem_t mutex_b;  // mutex to securize shared_b access.

/* Catch errors.
 */
#define CALL( exp ) do { \
		int ret; \
		ret = exp; \
		if (ret != 0) \
			fprintf(stderr, "%s failed: %m\n", #exp);\
		else \
			fprintf(stderr, "%s ok.\n", #exp); \
	} while (0)


/* Basic setup is the same as in the single-threaded example. However, 
 * we do some thread initiliazation first before invoking the job.
 */
int main(int argc, char** argv)
{
	int i;
	struct thread_context ctx[NUM_THREADS];
	pthread_t             task[NUM_THREADS];
	/* The task is in background mode upon startup. */		


	/*****
	 * 1) Command line paramter parsing would be done here.
	 */


       
	/*****
	 * 2) Work environment (e.g., global data structures, file data, etc.) would
	 *    be setup here.
	 */
	
	shared_a = 1;
	shared_b = 1;
	shared_c = 2;
	sem_init(&mutex_a, 0,1);
	sem_init(&mutex_b, 0,1);


	/*****
	 * 3) Initialize LITMUS^RT.
	 *    Task parameters will be specified per thread.
	 */
	init_litmus();


	/***** 
	 * 4) Launch threads.
	 */
	for (i = 0; i < NUM_THREADS; i++) {
		ctx[i].id = i;
		pthread_create(task + i, NULL, rt_thread, (void *) (ctx + i));
	}

	
	/*****
	 * 5) Wait for RT threads to terminate.
	 */
	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(task[i], NULL);
	

	/***** 
	 * 6) Clean up, maybe print results and stats, and exit.
	 */
	return 0;
}



/* A real-time thread is very similar to the main function of a single-threaded
 * real-time app. Notice, that init_rt_thread() is called to initialized per-thread
 * data structures of the LITMUS^RT user space libary.
 */
void* rt_thread(void *tcontext)
{
	int do_exit;
	struct thread_context *ctx = (struct thread_context *) tcontext;
	struct rt_task param;

	/* Set up task parameters */
	init_rt_task_param(&param);
	param.exec_cost = ms2ns(EXEC_COST);
	
	/* Invoke the right job. */
	if(ctx->id == 0){
	  param.period = ms2ns(PERIOD1); 
	}
	else{
	  param.period = ms2ns(PERIOD2);
	}
	
	param.relative_deadline = ms2ns(RELATIVE_DEADLINE);

	/* What to do in the case of budget overruns? */
	param.budget_policy = NO_ENFORCEMENT;

	/* The task class parameter is ignored by most plugins. */
	param.cls = RT_CLASS_SOFT;

	/* The priority parameter is only used by fixed-priority plugins. */
	param.priority = LITMUS_LOWEST_PRIORITY;

	/* Make presence visible. */
	printf("RT Thread %d active.\n", ctx->id);

	/*****
	 * 1) Initialize real-time settings.
	 */
	CALL( init_rt_thread() );

	/* To specify a partition, do
	 *
	 * param.cpu = CPU;
	 * be_migrate_to(CPU);
	 *
	 * where CPU ranges from 0 to "Number of CPUs" - 1 before calling
	 * set_rt_task_param().
	 */
	CALL( set_rt_task_param(gettid(), &param) );

	/*****
	 * 2) Transition to real-time mode.
	 */
	CALL( task_mode(LITMUS_RT_TASK) );

	/* The task is now executing as a real-time task if the call didn't fail. 
	 */



	/*****
	 * 3) Invoke real-time jobs.
	 */
	do {
		/* Wait until the next job is released. */
		sleep_next_period();
		/* Invoke the right job. */
		if(ctx->id == 0){
		 do_exit = jobAPlusBBasic(); 
		}
		else{
		  do_exit = jobAMultiplyByCBasic();
		}		
	} while (!do_exit);


	
	/*****
	 * 4) Transition to background mode.
	 */
	CALL( task_mode(BACKGROUND_TASK) );


	return NULL;
}



int jobAPlusBBasic(void) 
{
	int stop = 0;
	
	/* Do real-time calculation. */
	sem_wait(&mutex_a);
	
	  sem_wait(&mutex_b);
		shared_a = shared_a + shared_b;
	  sem_post(&mutex_b);
	  
	  printf("FROM Thread 1: shared_a value updated: %d\n", shared_a);
	  
	  if(shared_a >= MAX_VALUE){
		stop = 1;
	  }
	
	sem_post(&mutex_a);
	
	/* Don't exit. */
	return stop;
}

int jobAMultiplyByCBasic(void) 
{
	int stop = 0;
  
	/* Do real-time calculation. */
	sem_wait(&mutex_b);
	
	  sem_wait(&mutex_a);
		shared_b = shared_a * shared_c;
	  sem_post(&mutex_a);
	  
	  printf("FROM Thread 2: shared_b value updated: %d\n", shared_b);
	  
	  if(shared_b >= MAX_VALUE){
		stop = 1;
	  }
	
	sem_post(&mutex_b);
	
	/* Don't exit. */
	return stop;
}
 
