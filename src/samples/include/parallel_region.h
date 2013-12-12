/* =============================================================================
 *
 * parallel_region.h
 *	All needed jobs.
 *  M2 ALMA 2013/2014
 * =============================================================================
 */

/*******************************************************************************
 ****                      PARALLEL REGION DECLARATION                      ****
 ******************************************************************************/ 

#define APRINTER_MT_TL2     1
#define ABCPRINTER_MT_TL2   2

/* Parallel_region is start to start the threads routine. the sample parameters 
 * indicate wich jobs file you want to use in the routine.
 */
void parallel_region();