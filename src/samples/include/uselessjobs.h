/* =============================================================================
 *
 * uselessjobs.h
 *	All needed jobs.
 *  M2 ALMA 2013/2014
 * =============================================================================
 */

#define MAX_VALUE     1000000

/*******************************************************************************
 ****                        JOBS DECLARATIONS                              ****
 ******************************************************************************/

/* Declare the periodically invoked job: increment shared_a by step of 2. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobPlus2();

/* Declare the periodically invoked job: multiply shared_a by 2. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobMultiplyBy2();

/* Declare the periodically invoked job: increment shared_a by step of shared_b 
 * value and store the result in shared a. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAPlusB();

/* Declare the periodically invoked job: multiply shared_a by shared_c 
 * and store the result in shared_b. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */
int jobAMultiplyByC();

/*******************************************************************************
 ****                         SHARED VARIABLES                              ****
 ******************************************************************************/
int shared_a;
int shared_b;
int shared_c;

 
