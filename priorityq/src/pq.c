#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pq.h"


typedef int (*cmp_f)(int priority_a, int priority_b);

static bool resize_heap(pq_t *p_pq, size_t new_capacity);
static size_t getleft_child(size_t idx);
static size_t getright_child(size_t idx);
static void bubble_up(pq_t *p_pq, size_t idx);
static void bubble_down(pq_t *p_pq, size_t idx);
static void
swap(pq_entry_t *node1, pq_entry_t *node2);

static int
min_cmp (int a, int b)
{
    return (a < b) - (b < a); // avoid integer overflow (-1, 0, 1)
}

static int
max_cmp (int a, int b)
{
    return (b < a) - (a < b);
}

typedef struct priority_queue
{
    pq_entry_t *p_heap;
    cmp_f       cmp;
    size_t      count;
    size_t      capacity;
} pq_t;

pq_t *
pq_create (size_t initial_capacity, pq_type_t type)
{
    if (initial_capacity < MIN_SIZE) // if lower than min size, set to min
    {
        initial_capacity = MIN_SIZE;
    }

    pq_t *p_pq = calloc(1, sizeof(*p_pq));
    if (NULL != p_pq)
    {
        // set if the pq will be a min or max heap
        p_pq->cmp      = (PQ_MIN_HEAP == type) ? min_cmp : max_cmp;
        p_pq->capacity = initial_capacity;

        p_pq->p_heap = calloc(initial_capacity, sizeof(pq_entry_t));
        if (NULL == p_pq->p_heap)
        {
            free(p_pq);
            p_pq = NULL;
        }
    }

    return p_pq;
}

void
pq_clear (pq_t *p_pq, del_f del)
{
    if (NULL == p_pq)
    {
        return;
    }

    if (NULL != del)
    {
        for (size_t idx = 0; idx < p_pq->count; ++idx)
        {
            del(p_pq->p_heap[idx].data);
        }
    }

    memset(p_pq->p_heap, 0, p_pq->capacity * sizeof(pq_entry_t));
    p_pq->count = 0;
}

void
pq_destroy (pq_t **pp_pq, del_f del)
{
    if ((NULL == pp_pq) || (NULL == *pp_pq))
    {
        return;
    }

    if (NULL != del)
    {
        for (size_t idx = 0; idx < (*pp_pq)->count; ++idx)
        {
            del((*pp_pq)->p_heap[idx].data);
        }
    }

    free((*pp_pq)->p_heap);
    free(*pp_pq);
    *pp_pq = NULL;
}

static bool
resize_heap(pq_t *p_pq, size_t new_capacity)
{
    if (!p_pq || !new_capacity)
    {
        fprintf(stderr, "[ERR] Invalid argument(s) provided - resize_heap()\n");
        return false;
    }

    bool status = false;

    pq_entry_t *p_newheap = realloc(p_pq->p_heap, new_capacity * sizeof(pq_entry_t));
    if (p_newheap)
    {
        p_pq->p_heap = p_newheap;
        p_pq->capacity = new_capacity;
        status = true;
    }

    return status;
}

static void
swap(pq_entry_t *node1, pq_entry_t *node2)
{
    pq_entry_t temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}

static void
bubble_up(pq_t *p_pq, size_t idx)
{
    if (0 == idx) // root node detected, no bubble needed
    {
        return;
    }

    // get parrent index to get to node above
    size_t parentidx = (idx - 1) / 2;

    // compare node weights, to see if bubble up is neccessary
    int priority = 0; 
    priority = p_pq->cmp(p_pq->p_heap[idx].priority, p_pq->p_heap[parentidx].priority);

    if (priority < 0)
    {
        swap(&p_pq->p_heap[idx], &p_pq->p_heap[parentidx]);
        bubble_up(p_pq, parentidx);
    }
}

static void bubble_down(pq_t *p_pq, size_t idx)
{
    if (idx >= p_pq->count) // if index is out of bounds, return
    {
        return;
    }

    size_t left = getleft_child(idx);
    size_t right = getright_child(idx);
    size_t priority = idx;
    
    // Find which child to bring up to root
    // check first if child is within bounds of the array

    if (left < p_pq->count && 
        p_pq->cmp(p_pq->p_heap[left].priority, p_pq->p_heap[priority].priority) < 0)
    {
        priority = left;
    }
    
    // check right child and overwrite priority or left if higher priority
    if (right < p_pq->count && 
        p_pq->cmp(p_pq->p_heap[right].priority, p_pq->p_heap[priority].priority) < 0)
    {
        priority = right;
    }
    
    // If a child has higher priority, swap and continue
    if (priority != idx)
    {
        swap(&p_pq->p_heap[idx], &p_pq->p_heap[priority]);
        bubble_down(p_pq, priority);
    }
}

bool
pq_enqueue (pq_t *p_pq, void *p_data, int priority)
{
    if (!p_pq || !p_data)
    {
        fprintf(stderr, "[ERR] Invalid argument(s) provided - enqueue()\n");
        return false;
    }

    bool status = true;

    // check if resize is needed before adding
    if (p_pq->count >= p_pq->capacity)
    {
        size_t new_cap = p_pq->capacity * 2;
        if (!resize_heap(p_pq, new_cap))
        {
            fprintf(stderr, "[ERR] Heap resize failure - enqueue()\n");
            status = false;
        }
    }

    if (status) // check incase resize fails
    {
        // to add to queue: add to the end and bubble up 

        // add new entry to the end by using the count (last index)
        p_pq->p_heap[p_pq->count].data = p_data;
        p_pq->p_heap[p_pq->count].priority = priority;

        bubble_up(p_pq, p_pq->count);
        p_pq->count++; // inc size for sucessful add
    }

    return status;
}

/**
 * @brief Remove highest priority entry from the queue
 *
 * @param p_pq Pointer to the priority queue
 * @param[out] p_entry Pointer to entry struct to store dequeued data/priority
 * @return true on success, else false (invalid argument, empty queue)
 *
 * @note Caller is responsible for managing memory of returned data
 */
bool
pq_dequeue (pq_t *p_pq, pq_entry_t *p_entry)
{
    if (!p_pq)
    {
        fprintf(stderr, "[ERR] Invalid parameters provided - dequeue()\n");
        return false;
    }

    bool status = true;

    if (0 == p_pq->count)
    {
        fprintf(stderr, "Nothing to dequeue, queue is empty.\n");
        status = false;
        goto END_DEQ;
    }

    // pull root data and put into provided pointer
    *p_entry = p_pq->p_heap[0];
    p_pq->count--;

    if (p_pq->count > 0)
    {
        // take last element in index and make it root
        p_pq->p_heap[0] = p_pq->p_heap[p_pq->count];

        // clear last element
        memset(&p_pq->p_heap[p_pq->count], 0, sizeof(pq_entry_t));

        // bubble down the element we just brought up so it can be sorted
        bubble_down(p_pq, 0);
    }
    else // queue is empty
    {
        // clear what we just passed over
        memset(&p_pq->p_heap[0], 0, sizeof(pq_entry_t));
    }

END_DEQ:
    return status;
}

size_t
pq_size (pq_t *p_pq)
{
    return (NULL != p_pq) ? p_pq->count : 0;
}

static size_t
getleft_child(size_t idx)
{
    return 2 * idx + 1;
}

static size_t
getright_child(size_t idx)
{
    return 2 * idx + 2;
}

int pq_isempty(pq_t *p_queue)
{
    int status = 0;

    if (p_queue->count <= 0)
    {
        status = 1;
    }
    return status;
}
