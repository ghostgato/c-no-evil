#ifndef DEQUE
#define DEQUE

#include <stdlib.h>
#include <sys/types.h>

/**
 * @brief Opaque structure representing a double-ended queue (deque)
 *
 * This structure provides constant-time access to both head and tail,
 * enabling efficient stack and queue operations on the same data structure.
 * The implementation details are hidden from the user.
 */
typedef struct deque deque_t;

/**
 * @brief Function pointer type for deallocating data
 *
 * Called on each element's data during deque destruction or clearing.
 * Should free any memory allocated for the data.
 */
typedef void (*del_f)(void *);

/**
 * @brief Creates a new empty double-ended queue
 *
 * Allocates and initializes a new deque with no elements.
 * The deque supports both stack (LIFO) and queue (FIFO) operations
 * with constant-time complexity at both ends.
 *
 * @return Pointer to the newly created deque, or NULL if allocation fails
 */
deque_t *deque_create(void);

/**
 * @brief Destroys the deque and frees all associated memory
 *
 * Removes all elements from the deque, optionally calls a destructor function
 * on each element's data, then frees the deque structure itself.
 * Sets the deque pointer to NULL to prevent use-after-free errors.
 *
 * @param pp_q Pointer to the deque pointer (will be set to NULL)
 * @param del_func Called on each element's data before freeing (can be NULL)
 */
void deque_destroy(deque_t **pp_q, del_f del_func);

/**
 * @brief Purges all nodes from the deque.
 * 
 * @param pp_q pointer to deque that wwill be purged
 */
void deque_purge(deque_t *pp_q);

/**
 * @brief Peeks at the element at the head of the deque without removing it
 *
 * Returns a pointer to the data stored at the front of the deque.
 * The element remains in the deque. This is useful for inspecting
 * the next element to be popped or dequeued.
 *
 * @param p_q Pointer to the deque
 * @return Pointer to the data at the head, or NULL if deque is empty or NULL
 */
void *deque_peek(deque_t *p_q);

/**
 * @brief Pushes an element onto the head of the deque (stack push operation)
 *
 * Adds a new element to the front of the deque in constant time.
 * This element will be the next one returned by deque_pop() or deque_deque().
 * Supports standard stack (LIFO) behavior when used with deque_pop().
 *
 * @param p_q Pointer to the deque
 * @param p_data Pointer to the data to store (cannot be NULL)
 * @return Non-zero on success, 0 on failure (e.g., memory allocation failure)
 */
int deque_push(deque_t *p_q, void *p_data);

/**
 * @brief Pops an element from the head of the deque (stack pop operation)
 *
 * Removes and returns the element at the front of the deque in constant time.
 * The caller is responsible for managing the returned data's memory.
 * When used with deque_push(), provides standard stack (LIFO) behavior.
 *
 * @param p_q Pointer to the deque
 * @return Pointer to the data from the removed element, or NULL if empty
 */
void *deque_pop(deque_t *p_q);

/**
 * @brief Enqueues an element at the tail of the deque (queue enqueue operation)
 *
 * Adds a new element to the back of the deque in constant time.
 * When used with deque_deque(), provides standard queue (FIFO) behavior.
 * The element will be the last one returned by deque_deque().
 *
 * @param p_q Pointer to the deque
 * @param p_data Pointer to the data to store (cannot be NULL)
 * @return Non-zero on success, 0 on failure (e.g., memory allocation failure)
 */
int deque_enque(deque_t *p_q, void *p_data);

/**
 * @brief Dequeues an element from the head of the deque (queue dequeue
 * operation)
 *
 * Removes and returns the element at the front of the deque in constant time.
 * The caller is responsible for managing the returned data's memory.
 * When used with deque_enque(), provides standard queue (FIFO) behavior.
 *
 * @param p_q Pointer to the deque
 * @return Pointer to the data from the removed element, or NULL if empty
 */
void *deque_deque(deque_t *p_q);

/**
 * @brief Returns the number of elements in the deque
 *
 * Provides the current size of the deque in constant time.
 * The implementation maintains an internal count to avoid traversal.
 *
 * @param p_q Pointer to the deque
 * @return Number of elements in the deque, or -1 if deque is NULL
 */
ssize_t deque_size(deque_t *p_q);

/**
 * @brief Function pointer type for printing data elements
 *
 * Called for each element when printing the deque.
 * Should print the data in the desired format.
 */
typedef void (*print_f)(void *);

/**
 * @brief Prints all elements in the deque from head to tail
 *
 * Traverses the deque and calls the print function for each element.
 * This is a utility function for debugging and visualization.
 *
 * @param p_q Pointer to the deque
 * @param print_func Function to print each element's data
 */
void deque_print(deque_t *p_q, print_f print_func);

void print_int(void *data) ;

#endif