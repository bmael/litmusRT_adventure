/* =============================================================================
 *
 * jobs.h
 *	All needed jobs.
 *  M2 ALMA 2013/2014
 * =============================================================================
 */

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

/*******************************************************************************
 ****                         SHARED VARIABLES                              ****
 ******************************************************************************/
int shared_a;

 
