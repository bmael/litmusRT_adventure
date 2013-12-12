/* =============================================================================
 *
 * thread.c
 *
 * =============================================================================
 *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * For the license of bayes/sort.h and bayes/sort.c, please see the header
 * of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of kmeans, please see kmeans/LICENSE.kmeans
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of ssca2, please see ssca2/COPYRIGHT
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/mt19937ar.c and lib/mt19937ar.h, please see the
 * header of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/rbtree.h and lib/rbtree.c, please see
 * lib/LEGALNOTICE.rbtree and lib/LICENSE.rbtree
 * 
 * ------------------------------------------------------------------------
 * 
 * Unless otherwise noted, the following license applies to STAMP files:
 * 
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 * 
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 */


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/* Include gettid() */
#include <sys/types.h>

#include "thread.h"
#include "types.h"
#include "litmus.h"
#include "uselessjobs.h"
#include "parallel_region.h"

#define RELATIVE_DEADLINE 50
#define EXEC_COST         10

static THREAD_LOCAL_T    global_threadId;
static long              global_numThread       = 1;
static THREAD_BARRIER_T* global_barrierPtr      = NULL;
static long*             global_threadIds       = NULL;
static THREAD_ATTR_T     global_threadAttr;
static THREAD_T*         global_threads         = NULL;
static void            (*global_funcPtr)(void*) = NULL;
static void*             global_argPtr          = NULL;
static volatile bool_t   global_doShutdown      = FALSE;
static int               global_sample          = 0;

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

/* =============================================================================
 * threadWait
 * -- Synchronizes all threads to start/stop parallel section
 * =============================================================================
 */
static void
threadWait (void* argPtr)
{
    long threadId = *(long*)argPtr;

    THREAD_LOCAL_SET(global_threadId, (long)threadId);
	
	struct rt_task param;

	/* Set up task parameters */
	init_rt_task_param(&param);
	param.exec_cost = ms2ns(EXEC_COST);
	
	/* Periods setup */
	if(threadId == 0)
	{
	  param.period = ms2ns(100); 
	}
	
	if(threadId == 1)
	{
	  param.period = ms2ns(800);
	}
	
	param.relative_deadline = ms2ns(RELATIVE_DEADLINE);

	/* What to do in the case of budget overruns? */
	param.budget_policy = NO_ENFORCEMENT;

	/* The task class parameter is ignored by most plugins. */
	param.cls = RT_CLASS_SOFT;

	/* The priority parameter is only used by fixed-priority plugins. */
	param.priority = LITMUS_LOWEST_PRIORITY;

	/* Make presence visible. */
	printf("RT Thread %li active.\n", threadId);

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

	int do_exit;
	printf("[thread.c] Sample value is: %d\n", global_sample);
    do {
        THREAD_BARRIER(global_barrierPtr, threadId); /* wait for start parallel */
        if (global_doShutdown) {
            break;
        }
        
        global_funcPtr(global_argPtr);
        
        //////////////// Custom JOBS defined in uselessjobs.h //////////////////
		THREAD_BARRIER(global_barrierPtr, threadId); /* wait for end parallel */
        sleep_next_period();
		// First sample for Aprinter_mt_tl2
        if(threadId == 0 && global_sample == 1){
			do_exit = jobPlus2(MAX_VALUE);
		}
		
		if(threadId == 1 && global_sample == 1){
			do_exit = jobMultiplyBy2(MAX_VALUE);
		}
		
		// Second sample for ABCprinter_mt_tl2
		if(threadId == 0 && global_sample == 2){
			do_exit = jobAPlusB(MAX_VALUE);
		}
		
		if(threadId == 1 && global_sample == 2){
			do_exit = jobAMultiplyByC(MAX_VALUE);
		}
		
        /////////////// END Custom JOBS ////////////////////////////////////////
        
    } while(!do_exit);
	
    CALL( task_mode(BACKGROUND_TASK) );
}

/* =============================================================================
 * thread_startup
 * -- Create pool of secondary threads
 * -- numThread is total number of threads (primary + secondaries)
 * =============================================================================
 */
void
thread_startup (long numThread, int sample)
{
    long i;

    global_numThread = numThread;
    global_doShutdown = FALSE;
	global_sample = sample;

    /* Set up barrier */
    assert(global_barrierPtr == NULL);
    global_barrierPtr = THREAD_BARRIER_ALLOC(numThread);
    assert(global_barrierPtr);
    THREAD_BARRIER_INIT(global_barrierPtr, numThread);

    /* Set up ids */
    THREAD_LOCAL_INIT(global_threadId);
    assert(global_threadIds == NULL);
    global_threadIds = (long*)malloc(numThread * sizeof(long));
    assert(global_threadIds);
    for (i = 0; i < numThread; i++) {
        global_threadIds[i] = i;
    }

    /* Set up thread list */
    assert(global_threads == NULL);
    global_threads = (THREAD_T*)malloc(numThread * sizeof(THREAD_T));
    assert(global_threads);

    /* Set up pool */
    THREAD_ATTR_INIT(global_threadAttr);
    for (i = 1; i < numThread; i++) {
        THREAD_CREATE(global_threads[i],
                      global_threadAttr,
                      &threadWait,
                      &global_threadIds[i]);
    }

    /*
     * Wait for primary thread to call thread_start
     */
}


/* =============================================================================
 * thread_start
 * -- Make primary and secondary threads execute work
 * -- Should only be called by primary thread
 * -- funcPtr takes one arguments: argPtr
 * =============================================================================
 */
void
thread_start (void (*funcPtr)(void*), void* argPtr)
{
	printf("[thread.c] starting thread with id: %ld\n", thread_getId());
    global_funcPtr = funcPtr;
    global_argPtr = argPtr;

    long threadId = 0; /* primary */
    threadWait((void*)&threadId);
}

/* =============================================================================
 * thread_shutdown
 * -- Primary thread kills pool of secondary threads
 * =============================================================================
 */
void
thread_shutdown ()
{
	printf("[thread.c] thread_shutdown\n");
    /* Make secondary threads exit wait() */
    global_doShutdown = TRUE;
    THREAD_BARRIER(global_barrierPtr, 0);

    long numThread = global_numThread;

    long i;
    for (i = 1; i < numThread; i++) {
		printf("[thread.c] join thread with id: %ld\n", i);
        THREAD_JOIN(global_threads[i]);
    }

    THREAD_BARRIER_FREE(global_barrierPtr);
    global_barrierPtr = NULL;

    free(global_threadIds);
    global_threadIds = NULL;

    free(global_threads);
    global_threads = NULL;

    global_numThread = 1;
}


/* =============================================================================
 * thread_barrier_alloc
 * =============================================================================
 */
thread_barrier_t*
thread_barrier_alloc (long numThread)
{
    thread_barrier_t* barrierPtr;

    assert(numThread > 0);
    assert((numThread & (numThread - 1)) == 0); /* must be power of 2 */
    barrierPtr = (thread_barrier_t*)malloc(numThread * sizeof(thread_barrier_t));
    if (barrierPtr != NULL) {
        barrierPtr->numThread = numThread;
    }

    return barrierPtr;
}


/* =============================================================================
 * thread_barrier_free
 * =============================================================================
 */
void
thread_barrier_free (thread_barrier_t* barrierPtr)
{
    free(barrierPtr);
}


/* =============================================================================
 * thread_barrier_init
 * =============================================================================
 */
void
thread_barrier_init (thread_barrier_t* barrierPtr)
{
    long i;
    long numThread = barrierPtr->numThread;

    for (i = 0; i < numThread; i++) {
        barrierPtr[i].count = 0;
        THREAD_MUTEX_INIT(barrierPtr[i].countLock);
        THREAD_COND_INIT(barrierPtr[i].proceedCond);
        THREAD_COND_INIT(barrierPtr[i].proceedAllCond);
    }
}


/* =============================================================================
 * thread_barrier
 * -- Simple logarithmic barrier
 * =============================================================================
 */
void
thread_barrier (thread_barrier_t* barrierPtr, long threadId)
{
    long i = 2;
    long base = 0;
    long index;
    long numThread = barrierPtr->numThread;

    if (numThread < 2) {
        return;
    }

    do {
        index = base + threadId / i;
        if ((threadId % i) == 0) {
            THREAD_MUTEX_LOCK(barrierPtr[index].countLock);
            barrierPtr[index].count++;
            while (barrierPtr[index].count < 2) {
                THREAD_COND_WAIT(barrierPtr[index].proceedCond,
                                 barrierPtr[index].countLock);
            }
            THREAD_MUTEX_UNLOCK(barrierPtr[index].countLock);
        } else {
            THREAD_MUTEX_LOCK(barrierPtr[index].countLock);
            barrierPtr[index].count++;
            if (barrierPtr[index].count == 2) {
                THREAD_COND_SIGNAL(barrierPtr[index].proceedCond);
            }
            while (THREAD_COND_WAIT(barrierPtr[index].proceedAllCond,
                                    barrierPtr[index].countLock) != 0)
            {
                /* wait */
            }
            THREAD_MUTEX_UNLOCK(barrierPtr[index].countLock);
            break;
        }
        base = base + numThread / i;
        i *= 2;
    } while (i <= numThread);

    for (i /= 2; i > 1; i /= 2) {
        base = base - numThread / i;
        index = base + threadId / i;
        THREAD_MUTEX_LOCK(barrierPtr[index].countLock);
        barrierPtr[index].count = 0;
        THREAD_COND_SIGNAL(barrierPtr[index].proceedAllCond);
        THREAD_MUTEX_UNLOCK(barrierPtr[index].countLock);
    }
}


/* =============================================================================
 * thread_getId
 * -- Call after thread_start() to get thread ID inside parallel region
 * =============================================================================
 */
long
thread_getId()
{
    return (long)THREAD_LOCAL_GET(global_threadId);
}


/* =============================================================================
 * thread_getNumThread
 * -- Call after thread_start() to get number of threads inside parallel region
 * =============================================================================
 */
long
thread_getNumThread()
{
    return global_numThread;
}


/* =============================================================================
 * thread_barrier_wait
 * -- Call after thread_start() to synchronize threads inside parallel region
 * =============================================================================
 */
void
thread_barrier_wait()
{
#ifndef SIMULATOR
    long threadId = thread_getId();
#endif /* !SIMULATOR */
    THREAD_BARRIER(global_barrierPtr, threadId);
}


/* =============================================================================
 * TEST_THREAD
 * =============================================================================
 */
#ifdef TEST_THREAD


#include <stdio.h>
#include <unistd.h>


#define NUM_THREADS    (4)
#define NUM_ITERATIONS (3)

void
printId (void* argPtr)
{
    long threadId = thread_getId();
    long numThread = thread_getNumThread();
    long i;

    for ( i = 0; i < NUM_ITERATIONS; i++ ) {
        thread_barrier_wait();
        if (threadId == 0) {
            sleep(1);
        } else if (threadId == numThread-1) {
            usleep(100);
        }
        printf("i = %li, tid = %li\n", i, threadId);
        if (threadId == 0) {
            puts("");
        }
        fflush(stdout);
    }
}

int
main ()
{
    puts("Starting...");

    /* Run in parallel */
    thread_startup(NUM_THREADS);
    /* Start timing here */
    thread_start(printId, NULL);
    thread_start(printId, NULL);
    thread_start(printId, NULL);
    /* Stop timing here */
    thread_shutdown();

    puts("Done.");

    return 0;
}


#endif /* TEST_THREAD */


/* =============================================================================
 *
 * End of thread.c
 *
 * =============================================================================
 */
