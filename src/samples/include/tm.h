/* =============================================================================
 *
 * tm.h
 *
 * Utility defines for transactional memory
 *
 * Developed for litmus_adventure project
 *
 * M2_ALMA November 2013
 *
 * =============================================================================
 */

#ifndef TM_H
#define TM_H 1

#include <string.h>
#include <stm.h>
#include "memory.h" 
 
 
#    define TM_ARG                        STM_SELF,
#    define TM_ARG_ALONE                  STM_SELF
#    define TM_ARGDECL                    STM_THREAD_T* TM_ARG
#    define TM_ARGDECL_ALONE              STM_THREAD_T* TM_ARG_ALONE
#    define TM_CALLABLE                   /* nothing */

#    define TM_STARTUP(numThread)     STM_STARTUP()
#    define TM_SHUTDOWN()             STM_SHUTDOWN()

#    define TM_THREAD_ENTER()         TM_ARGDECL_ALONE = STM_NEW_THREAD(); \
                                      STM_INIT_THREAD(TM_ARG_ALONE, thread_getId())
#    define TM_THREAD_EXIT()          STM_FREE_THREAD(TM_ARG_ALONE)

#    define P_MALLOC(size)            malloc(size)
#    define P_FREE(ptr)               free(ptr)
#    define TM_MALLOC(size)           STM_MALLOC(size)
#    define TM_FREE(ptr)              STM_FREE(ptr)

#    define TM_BEGIN()                  STM_BEGIN_WR()
#    define TM_BEGIN_RO()               STM_BEGIN_RD()
#    define TM_END()                    STM_END()
#    define TM_RESTART()                STM_RESTART()

#    define TM_EARLY_RELEASE(var)       /* nothing */

/* =============================================================================
 * Transactional Memory System interface for shared memory accesses
 *
 * There are 3 flavors of each function:
 *
 * 1) no suffix: for accessing variables of size "long"
 * 2) _P suffix: for accessing variables of type "pointer"
 * 3) _F suffix: for accessing variables of type "float"
 * =============================================================================
 */

#    define TM_SHARED_READ(var)           STM_READ(var)
#    define TM_SHARED_READ_P(var)         STM_READ_P(var)
#    define TM_SHARED_READ_F(var)         STM_READ_F(var)

#    define TM_SHARED_WRITE(var, val)     STM_WRITE((var), val)
#    define TM_SHARED_WRITE_P(var, val)   STM_WRITE_P((var), val)
#    define TM_SHARED_WRITE_F(var, val)   STM_WRITE_F((var), val)

#    define TM_LOCAL_WRITE(var, val)      STM_LOCAL_WRITE(var, val)
#    define TM_LOCAL_WRITE_P(var, val)    STM_LOCAL_WRITE_P(var, val)
#    define TM_LOCAL_WRITE_F(var, val)    STM_LOCAL_WRITE_F(var, val)
 

#    define P_MEMORY_STARTUP(numThread)   do { \
                                            bool_t status; \
                                            status = memory_init((numThread), \
                                                                 ((1<<28) / numThread), \
                                                                 2); \
                                            assert(status); \
                                        } while (0) /* enforce comma */
#  define P_MEMORY_SHUTDOWN()           memory_destroy()


#endif