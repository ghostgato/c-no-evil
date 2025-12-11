#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdbool.h>
#include <stddef.h>

// threadpool struct
typedef struct threadpool tpool_t;

/**
 * Create a new thread pool with specified number of worker threads
 * 
 * @param thread_num Number of worker threads (0 = use default [4])
 * @return Pointer to thread pool, or NULL on failure
 */
tpool_t *tpool_create(size_t thread_num);

/**
 * Destroy thread pool and free all resources
 * 
 * @param p_pool Pointer to thread pool
 * @return true on success, false on failure
 */
bool tpool_destroy(tpool_t *p_pool);

/**
 * Wait until all tasks in the pool are completed
 * 
 * @param p_pool Pointer to thread pool
 */
void tpool_wait(tpool_t *p_pool);

/**
 * Add a task to the thread pool queue
 * 
 * @param p_pool Pointer to thread pool
 * @param function Function pointer to execute
 * @param p_arg Argument to pass to function
 * @return true on success, false on failure
 */
bool tpool_add_task(tpool_t *p_pool, void (*function)(void *), void *p_arg);

#endif /* THREADPOOL_H */
