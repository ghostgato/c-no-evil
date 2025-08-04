#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pq.h"

int
main (void)
{
    srand(time(NULL));

    pq_t *p_pq = pq_create(128, PQ_MIN_HEAP);

    char *my_string = strdup("Hello, World!");

    for (int i = 0; i < 16; i++)
    {
        printf("%d ", i);
        pq_enqueue(p_pq, my_string, rand() % 100);
    }
    printf("\n");

    my_string = NULL;

    pq_entry_t entry = { 0 };
    for (int i = 0; i < 16; i++)
    {
        pq_dequeue(p_pq, &entry);

        printf("Lowest priority %d\n", entry.priority);
        puts(entry.data);
    }
    free(entry.data);

    pq_destroy(&p_pq, NULL);
}

/*

pq_t *pq = pq_create(16, PQ_MIN_HEAP);

// Add some data
pq_enqueue(pq, "Low priority", 10);
pq_enqueue(pq, "High priority", 1);
pq_enqueue(pq, "Medium priority", 5);

// Remove highest priority item
pq_entry_t entry;
if (pq_dequeue(pq, &entry)) {
    printf("Dequeued: %s (priority %d)\n", (char*)entry.data, entry.priority);
    // This would print "High priority (priority 1)"
}

// Clean up
pq_destroy(&pq, NULL);

*/