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

/* Include the LITMUS^RT API.*/
#include "litmus.h"

/* Include the TL2 MACRO override */
#include "tm.h"

#define PERIOD1           100
#define PERIOD2           800
#define RELATIVE_DEADLINE 50
#define EXEC_COST         10

/* Let's create 10 threads in the example, 
 * for a total utilization of 1.
 */
#define NUM_THREADS      2 
#define MAX_VALUE		 10000

/* The information passed to each thread. Could be anything. */
struct thread_context {
	int id;
};

void parallel_region();

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
	pthread_t task[NUM_THREADS];
	
	/* The task is in background mode upon startup. */		


	/*****
	 * 1) Command line paramter parsing would be done here.
	 */


       
	/*****
	 * 2) Work environment (e.g., global data structures, file data, etc.) would
	 *    be setup here.
	 */
	
	//shared_a = 1;

	/*****
	 * 3) Initialize LITMUS^RT.
	 *    Task parameters will be specified per thread.
	 */
	init_litmus();

	/***** 
	 * 4) Launch threads.
	 */
	printf("[main] TM_STARTUP(%d)\n", NUM_THREADS);
	TM_STARTUP(NUM_THREADS);
	
	printf("[main] P_MEMORY_STARTUP(%d)\n", NUM_THREADS);
	P_MEMORY_STARTUP(NUM_THREADS);
	
	printf("[main] rt_thread_startup\n");
	thread_startup(NUM_THREADS);
	
	
	thread_start(parallel_region, NULL);	
	
	/*****
	 * 5) Wait for RT threads to terminate.
	 */

	printf("[main] TM_SHUTDOWN()\n");
	TM_SHUTDOWN();
	
	printf("[main] P_MEMORY_SHUTDOWN()\n");
	P_MEMORY_SHUTDOWN();
	
	printf("[main] Wait for RT threads to terminate\n");
	thread_shutdown();
	
	/***** 
	 * 6) Clean up, maybe print results and stats, and exit.
	 */
	return 0;
}



void parallel_region(){
  
  printf("[parallel_region] ...\n");
  
}


