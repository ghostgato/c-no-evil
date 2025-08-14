// deque.c - Fast array-based implementation (drop-in replacement)
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "qarr.h"

#define GROWTH_FACTOR 2

// Array-based deque structure (replaces linked list)
struct qarr
{
    void  **pp_data;
    size_t  head;     // Index of pop point
    size_t  tail;     // Index of insertion point
    size_t  capacity; // Total space of queue
    ssize_t size;     // Current number of elements
};

// Initial capacity for new deques
#define INITIAL_CAPACITY 1024

qarr_t *
qarr_create (void)
{
    qarr_t *p_newq = malloc(sizeof(qarr_t));
    if (!p_newq)
    {
        perror("[ERR] deque memory allocation fail");
        return NULL;
    }

    p_newq->pp_data = malloc(INITIAL_CAPACITY * sizeof(void *));
    if (!p_newq->pp_data)
    {
        perror("[ERR] deque data array allocation fail");
        free(p_newq);
        return NULL;
    }

    p_newq->capacity = INITIAL_CAPACITY;
    p_newq->head     = 0;
    p_newq->tail     = 0;
    p_newq->size     = 0;

    return p_newq;
}

void
qarr_destroy (qarr_t **pp_q, dq_del_f del_func)
{
    if (!pp_q || !*pp_q)
    {
        return;
    }

    qarr_t *p_queue = *pp_q;

    // delete data if function was provided
    if (del_func && p_queue->size > 0)
    {
        for (ssize_t i = 0; i < p_queue->size; i++)
        {
            size_t idx = (p_queue->head + i) % p_queue->capacity;
            if (p_queue->pp_data[idx])
            {
                del_func(p_queue->pp_data[idx]);
            }
        }
    }

    free(p_queue->pp_data);
    free(p_queue);
    *pp_q = NULL;
}

void
qarr_purge (qarr_t *p_q)
{
    if (!p_q)
    {
        return;
    }

    // Just move ptrs back to begining to overwrite
    p_q->head = 0;
    p_q->tail = 0;
    p_q->size = 0;
}

void *
qarr_peek (qarr_t *p_q)
{
    if (!p_q || p_q->size <= 0)
    {
        return NULL;
    }

    return p_q->pp_data[p_q->head];
}

static int
qarr_resize (qarr_t *p_q)
{
    size_t new_cap = p_q->capacity * GROWTH_FACTOR;
    void **pp_new     = malloc(new_cap * sizeof(void *));
    if (!pp_new)
    {
        perror("[ERR] Failed to resize deque");
        return 0;
    }

    // Copy existing data in order
    for (ssize_t i = 0; i < p_q->size; i++)
    {
        size_t old_idx = (p_q->head + i) % p_q->capacity;
        pp_new[i]    = p_q->pp_data[old_idx];
    }

    free(p_q->pp_data);
    p_q->pp_data  = pp_new;
    p_q->capacity = new_cap;
    p_q->head     = 0;
    p_q->tail     = p_q->size;

    return 1;
}

int
qarr_push (qarr_t *p_q, void *p_data)
{
    if (!p_q || !p_data)
    {
        return 0;
    }

    // Resize if full
    if (p_q->size >= p_q->capacity)
    {
        if (!qarr_resize(p_q))
        {
            return 0;
        }
    }

    // Modulo to wrap around array if needed
    p_q->head               = (p_q->head + p_q->capacity - 1) % p_q->capacity;
    p_q->pp_data[p_q->head] = p_data;
    p_q->size++;

    return 1;
}

void *
qarr_pop (qarr_t *p_q)
{
    if (!p_q || p_q->size <= 0)
    {
        return NULL;
    }

    void *p_data = p_q->pp_data[p_q->head];
    p_q->head    = (p_q->head + 1) % p_q->capacity;
    p_q->size--;

    return p_data;
}

int
qarr_enque (qarr_t *p_q, void *p_data)
{
    if (!p_q)
    {
        fprintf(stderr, "[ERR] No queue present.\n");
        return 0;
    }

    if (!p_data)
    {
        return 0;
    }

    // Resize if full
    if (p_q->size >= p_q->capacity)
    {
        if (!qarr_resize(p_q))
        {
            fprintf(stderr, "[ERR] Failed to resize queue for enqueue\n");
            return 0;
        }
    }

    // Add to tail, modulo to wrap around array
    p_q->pp_data[p_q->tail] = p_data;
    p_q->tail               = (p_q->tail + 1) % p_q->capacity;
    p_q->size++;

    return 1;
}

void *
qarr_deque (qarr_t *p_q)
{
    if (!p_q || p_q->size <= 0)
    {
        return NULL;
    }

    void *p_data = p_q->pp_data[p_q->head];
    p_q->head    = (p_q->head + 1) % p_q->capacity;
    p_q->size--;

    return p_data;
}

ssize_t
qarr_size (qarr_t *p_q)
{
    if (!p_q)
    {
        return 0;
    }

    return p_q->size;
}

void
qarr_print (qarr_t *p_q, void (*print_func)(void *data))
{
    if (!p_q || !print_func)
    {
        return;
    }

    for (ssize_t i = 0; i < p_q->size; i++)
    {
        size_t idx = (p_q->head + i) % p_q->capacity;
        print_func(p_q->pp_data[idx]);
        printf(" ");
    }
    puts("");
}

void
print_int (void *data)
{
    printf("%d", *(int *)data);
}