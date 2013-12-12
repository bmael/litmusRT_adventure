/* =============================================================================
 *
 * uselessjobs.c
 *	All needed jobs.
 *  M2 ALMA 2013/2014
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uselessjobs.h"

/* Include the TL2 MACRO override */
#include "tm.h"



/*******************************************************************************
 ****                         SHARED VARIABLES                              ****
 ******************************************************************************/
int shared_a = 1;
int shared_b = 1;
int shared_c = 2;

/*******************************************************************************
 ****                          IMPLEMENTATION                               ****
 ******************************************************************************/

/* Declare the periodically invoked job: increment shared_a by step of 2. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobPlus2(int max_value) 
{ 	
  int stop;

  TM_THREAD_ENTER();
  stop = 0;

  /* Do real-time calculation. */
  TM_BEGIN();
  TM_SHARED_WRITE(shared_a, shared_a + 2);

  if(shared_a >= max_value || shared_a < 0){
	stop = 1;
  }

  TM_END();
  
  printf("[jobPlus2] value updated: shared_a = %d\n", shared_a);
  
  TM_THREAD_EXIT();
  return stop;

}

/* Declare the periodically invoked job: multiply shared_a by 2. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobMultiplyBy2(int max_value) 
{  
  int stop;
  
  TM_THREAD_ENTER();
  stop = 0;

  /* Do real-time calculation. */
  TM_BEGIN();
  TM_SHARED_WRITE(shared_a, shared_a * 2);

  if(shared_a >= max_value || shared_a < 0){
	stop = 1;
  } 

  TM_END();
  
  printf("[jobMultiplyBy2] value updated: shared_a = %d\n", shared_a);
  
  TM_THREAD_EXIT();
  
  return stop;
}

/* Declare the periodically invoked job: multiply shared_a by shared_c 
 * and store the result in shared_b. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAPlusB(int max_value) 
{ 	
  int stop;

  TM_THREAD_ENTER();
  stop = 0;

  /* Do real-time calculation. */
  TM_BEGIN();
  TM_SHARED_WRITE(shared_a, ((int)TM_SHARED_READ(shared_a) + (int)TM_SHARED_READ(shared_b)));
  printf("[jobPlus2] value updated: shared_a = %d\n", (int)TM_SHARED_READ(shared_a));
  TM_END();
  
  TM_BEGIN();
  
  if((int)TM_SHARED_READ(shared_a) >= max_value || (int)TM_SHARED_READ(shared_a) < 0){
	stop = 1;
  }
  
  TM_END();

  TM_THREAD_EXIT();
  return stop;

}

/* Declare the periodically invoked job: multiply shared_a by 2. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAMultiplyByC(int max_value) 
{  
  int stop;
  
  TM_THREAD_ENTER();
  stop = 0;

  /* Do real-time calculation. */
  TM_BEGIN();
  TM_SHARED_WRITE(shared_b, ((int)TM_SHARED_READ(shared_a) * shared_c));
  printf("[jobMultiplyBy2] value updated: shared_b = %d\n", (int)TM_SHARED_READ(shared_b));
  TM_END();

  TM_BEGIN();
  if((int)TM_SHARED_READ(shared_b) >= max_value || (int)TM_SHARED_READ(shared_b) < 0){
	stop = 1;
  } 
  TM_END();
 
  TM_THREAD_EXIT();
  
  return stop;
}
