#ifndef PQ_H
#define PQ_H

#include <stddef.h>
#include <stdbool.h>

#define MIN_SIZE 8

/**
 * @brief User provided function to delete void* without memory leaks
 * @param p_data Pointer to data to be freed
 */
typedef void (*del_f)(void *p_data);

/**
 * @brief Entry structure containing user data and its priority
 */
typedef struct
{
    void *data;
    int   priority;
} pq_entry_t;

/**
 * @brief Opaque priority queue structure
 */
typedef struct priority_queue pq_t;

typedef enum
{
    PQ_MIN_HEAP, /**< Minimum priority queue - lower values, higher priority */
    PQ_MAX_HEAP  /**< Maximum priority queue - higher values, higher priority */
} pq_type_t;

/**
 * @brief Create a new priority queue
 *
 * @param initial_capacity Initial capacity (MIN_SIZE minimum)
 * @param type Type of priority queue (min-heap or max-heap)
 * @return pq_t* Pointer to newly allocated priority queue or NULL on failure
 */
pq_t *pq_create(size_t initial_capacity, pq_type_t type);

/**
 * @brief Clear all entries from the priority queue
 *
 * @param p_pq Pointer to the priority queue
 * @param del Function to free user data, or NULL if no cleanup required
 */
void pq_clear(pq_t *p_pq, del_f del);

/**
 * @brief Destroy the priority queue and free all memory
 *
 * @param pp_pq Pointer to pointer of priority queue (set to NULL after
 * destruction)
 * @param del Function to free user data, or NULL if no cleanup required
 */
void pq_destroy(pq_t **pp_pq, del_f del);

/**
 * @brief Add an entry to the priority queue
 *
 * @param p_pq Pointer to the priority queue
 * @param p_data Pointer to user data to be enqueued
 * @param priority Priority value associated to user data
 * @return true on success, else false (invalid argument, memory allocation)
 */
bool pq_enqueue(pq_t *p_pq, void *p_data, int priority);

/**
 * @brief Remove highest priority entry from the queue
 *
 * @param p_pq Pointer to the priority queue
 * @param[out] p_entry Pointer to entry struct to store dequeued data/priority
 * @return true on success, else false (invalid argument, empty queue)
 *
 * @note Caller is responsible for managing memory of returned data
 */
bool pq_dequeue(pq_t *p_pq, pq_entry_t *p_entry);

/**
 * @brief Get number of entries in the priority queue
 *
 * @param p_pq Pointer to the priority queue
 * @return size_t Number of entries currently in the queue (0 on NULL)
 */
size_t pq_size(pq_t *p_pq);

/**
 * @brief returns if priority queue is empty
 * 
 * @param p_queue 
 * @return int 1 for empty; 0, if there are entries present
 */
int pq_isempty(pq_t *p_queue);

#endif
