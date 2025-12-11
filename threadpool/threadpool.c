/**
 * @file threadpool.c
 * @author darren (cyber-gato)
 * @brief threadpool library in C
 * @version 0.1
 * @date 2025-12-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "threadpool.h"

#define DEFAULT_THREAD_CT 4

// threadpool tasks
typedef struct task
{
    void       (*function)(void *);
    void        *p_arg;
    struct task *p_next;
}task_t;

struct threadpool
{
    pthread_t *p_threads;
    size_t threadcount;
    int b_shutdown;

    task_t  *p_queue_head;
    task_t  *p_queue_tail;
    size_t   queue_size;

    pthread_mutex_t task_mutex;
    pthread_cond_t task_available;
    pthread_cond_t task_empty;
    size_t active_threads;
};

static void *taskloop_thread(void *p_arg);
static task_t *get_task(tpool_t *p_pool);


tpool_t *tpool_create(size_t thread_num)
{
    tpool_t *p_tpool = calloc(1, sizeof(tpool_t));
    if (!p_tpool)
    {
        fprintf(stderr, "[ERR] calloc failed in %s\n", __func__);
        return NULL;
    }

    if (0 == thread_num) // if no threadcount provided, default 4
    {
        thread_num = DEFAULT_THREAD_CT;
    }

    p_tpool->p_threads = calloc(thread_num, sizeof(pthread_t));
    if (!p_tpool->p_threads)
    {
        perror("[ERR] Calloc fail - tpool_create()\n");
        free(p_tpool);
        goto END_CREATEPOOL;
    }

    // initialize threadpool struct
    p_tpool->threadcount = thread_num;
    p_tpool->b_shutdown = 0;
    p_tpool->p_queue_head = NULL;
    p_tpool->p_queue_tail = NULL;
    p_tpool->queue_size = 0;
    p_tpool->active_threads = 0;

    pthread_mutex_init(&p_tpool->task_mutex, NULL);
    pthread_cond_init(&p_tpool->task_available, NULL);
    pthread_cond_init(&p_tpool->task_empty, NULL);

    // start up threads
    for (size_t idx = 0; idx < p_tpool->threadcount; idx++)
    {
        pthread_create(&p_tpool->p_threads[idx], NULL, taskloop_thread, p_tpool);
    }

END_CREATEPOOL:
    return p_tpool;
}

bool tpool_destroy(tpool_t *p_pool)
{
    if (!p_pool)
    {
        return false;
    }

    pthread_mutex_lock(&p_pool->task_mutex);
    p_pool->b_shutdown = 1;

    // wake all threads to prepare to join
    pthread_cond_broadcast(&p_pool->task_available);
    pthread_mutex_unlock(&p_pool->task_mutex);

    // join all threads back
    for (size_t idx = 0; idx < p_pool->threadcount; idx++)
    {
        pthread_join(p_pool->p_threads[idx], NULL);
    }

    // free remaining tasks in queue
    task_t *p_task = NULL;
    while (p_pool->p_queue_head)
    {
        p_task = p_pool->p_queue_head;
        p_pool->p_queue_head = p_task->p_next;
        free(p_task);
    }

    // cleanup
    pthread_mutex_destroy(&p_pool->task_mutex);
    pthread_cond_destroy(&p_pool->task_available);
    pthread_cond_destroy(&p_pool->task_empty);
    free(p_pool->p_threads);
    free(p_pool);

    return true;
}

void tpool_wait(tpool_t *p_pool)
{
    if (!p_pool)
    {
        return;
    }

    pthread_mutex_lock(&p_pool->task_mutex);

    // wait until no more tasks and no active threads
    while ((p_pool->queue_size > 0) || (p_pool->active_threads > 0))
    {
        pthread_cond_wait(&p_pool->task_empty, &p_pool->task_mutex);
    }

    pthread_mutex_unlock(&p_pool->task_mutex);
}

bool tpool_add_task(tpool_t *p_pool, void (*function)(void *), void *p_arg)
{
    if (!p_pool || !function)
    {
        return false;
    }

    bool status = true;

    // create new task struct and assign values
    task_t *p_newtask = calloc(1, sizeof(task_t));
    if (!p_newtask)
    {
        fprintf(stderr, "[ERR] calloc failed in %s\n", __func__);
        status = false;
        goto END_ADDTASK;
    }

    p_newtask->function = function;
    p_newtask->p_arg = p_arg;
    p_newtask->p_next = NULL;

    pthread_mutex_lock(&p_pool->task_mutex);

    if (p_pool->p_queue_tail)
    {
        p_pool->p_queue_tail->p_next = p_newtask;
    }
    else
    {
        p_pool->p_queue_head = p_newtask;
    }
    p_pool->p_queue_tail = p_newtask;
    p_pool->queue_size++;
    
    // signal new work available
    pthread_cond_signal(&p_pool->task_available);
    pthread_mutex_unlock(&p_pool->task_mutex);

END_ADDTASK:
    return status;
}

static void *taskloop_thread(void *p_arg)
{
    tpool_t *p_pool = (tpool_t *)p_arg;
    task_t       *p_task = NULL;

    while (1)
    {
        pthread_mutex_lock(&p_pool->task_mutex);

        // wait for task
        while (!p_pool->b_shutdown && (0 == p_pool->queue_size))
        {
            pthread_cond_wait(&p_pool->task_available, &p_pool->task_mutex);
        }

        // check for shut down 
        if ((p_pool->b_shutdown) && (0 == p_pool->queue_size))
        {
            pthread_mutex_unlock(&p_pool->task_mutex);
            break;
        }

        // get task from queue
        p_task = get_task(p_pool);
        
        pthread_mutex_unlock(&p_pool->task_mutex);

        // execute task
        if (p_task)
        {
            p_task->function(p_task->p_arg);
            free(p_task); // free after completing task

            pthread_mutex_lock(&p_pool->task_mutex);
            p_pool->active_threads--;

            // if no tasks and active workers, signal empty queue
            if ((0 == p_pool->queue_size) && (0 == p_pool->active_threads))
            {
                pthread_cond_signal(&p_pool->task_empty);
            }

            pthread_mutex_unlock(&p_pool->task_mutex);
        }
    }

    return NULL;
}

static task_t *get_task(tpool_t *p_pool)
{
    if (!p_pool)
    {
        return NULL;
    }

    task_t *p_task = NULL;

    // we are assuming that mutex is called from where this function is called

    if (p_pool->p_queue_head)
    {
        p_task = p_pool->p_queue_head;
        p_pool->p_queue_head = p_task->p_next;
        p_pool->queue_size--;

        if (0 == p_pool->queue_size)
        {
            p_pool->p_queue_tail = NULL;
        }

        p_pool->active_threads++;
    }

    return p_task;
}
