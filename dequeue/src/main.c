#include <stdio.h>

#include "../include/deque.h"

int main (void)
{
    int nums[] = {5, 7, 3, 8, 9, 3, 2, 6};
    deque_t *mydeq = deque_create();
    
    if (!mydeq)
    {
        fprintf(stderr, "Failed to create deque\n");
        return 1;
    }

    // Push nums to deque as stack (using push)
    for (size_t idx = 0; idx < sizeof(nums) / sizeof(*nums); idx++)
    {
        if (!deque_push(mydeq, &nums[idx]))
        {
            fprintf(stderr, "Failed to push element\n");
            break;
        }
    }

    printf("=== Initial Deque List  ===\n");
    deque_print(mydeq, print_int);
    printf("Size: %zd\n\n", deque_size(mydeq));

    // Test stack operations (LIFO)
    printf("=== Stack Operations (LIFO) ===\n");
    void *popped = deque_pop(mydeq);
    if (popped)
    {
        printf("Popped: %d\n", *(int*)popped);
    }
    
    void *peeked = deque_peek(mydeq);
    if (peeked)
    {
        printf("Peeked: %d\n", *(int*)peeked);
    }
    
    popped = deque_pop(mydeq);
    if (popped)
    {
        printf("Popped: %d\n", *(int*)popped);
    }

    printf("After popping twice: ");
    deque_print(mydeq, print_int);

    // Test queue operations (FIFO)
    printf("\n=== Queue Operations (FIFO) ===\n");
    int new_val = 69;
    if (deque_enque(mydeq, &new_val))
    {
        printf("Enqueued: %d\n", new_val);
    }

    printf("After enqueue: ");
    deque_print(mydeq, print_int);

    void *dequeued = deque_deque(mydeq);
    if (dequeued)
    {
        printf("Dequeued: %d\n", *(int*)dequeued);
    }

    printf("After dequeue: ");
    deque_print(mydeq, print_int);
    printf("Final size: %zd\n", deque_size(mydeq));

    // Test error conditions
    printf("\n=== Error  Testing ===\n");
    printf("Passing null deque: %s\n", 
    deque_enque(NULL, &new_val) ? "Success" : "Failed (expected)");
    printf("Pass null to pop: %s\n", 
    deque_pop(NULL) ? "Success" : "Failed (expected)");

    // Clean up
    deque_destroy(&mydeq, NULL);
    printf("Deque destroyed successfully\n");
    
    return 0;
}