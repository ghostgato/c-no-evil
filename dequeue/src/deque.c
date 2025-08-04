#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../include/deque.h" 

typedef struct node {
    struct node *p_next;
    void        *p_data;
} node_t;

struct deque {
    node_t  *p_head;
    node_t  *p_tail;
    ssize_t  size;
};

deque_t *deque_create(void)
{
    deque_t *p_newdeque = calloc(1, sizeof(deque_t));
    if (!p_newdeque)
    {
        perror("[ERR] deque memory allocation fail");
    }

    return p_newdeque;
}

void deque_destroy(deque_t **pp_q, del_f del_func)
{
    if ((NULL == pp_q) || (NULL == *pp_q))
    {
        return;
    }
    //deque_t *p_deqtemp = pp_q;
    // node_t *p_curr = p_deqtemp->p_head;
    node_t *p_curr = (*pp_q)->p_head;
    
    while(p_curr)
    {
        node_t *p_temp = p_curr;
        p_curr = p_curr->p_next;
        
        if (del_func)
        {
            del_func(p_temp->p_data);
        }
        free(p_temp);
    }
    free(*pp_q);
    *pp_q = NULL;  // Prevent use-after-free
}

void deque_purge(deque_t *pp_q)
{
    node_t *p_curr = pp_q->p_head;

    while (p_curr)
    {
        node_t *p_temp = p_curr;
        p_curr = p_curr->p_next;

        free(p_temp);
    }
}

void *deque_peek(deque_t *p_q)
{
    void *p_peekdata = NULL;
    if (p_q != NULL && p_q->p_head != NULL)
    {
        p_peekdata = p_q->p_head->p_data;
    }

    return p_peekdata;
}

int deque_push(deque_t *p_q, void *p_data)
{
    int status = 1; // start success

    // if stack is null or nod data is passed
    if ((NULL == p_q) || (NULL == p_data))
    {
        status = 0;
        goto END_PUSH;
    }

    node_t *p_newnode = calloc(1, sizeof(node_t));
    if (!p_newnode)
    {
        perror("[ERR] Memory allocation fail for new node");
        status = 0;
        goto END_PUSH;
    }

    //put parameter data into newly created node
    p_newnode->p_data = p_data;      // put data in new node
    p_newnode->p_next = p_q->p_head; // set new's next to head
    p_q->p_head = p_newnode;         // set new node as new head

    if (!p_q->p_tail)
    {
        p_q->p_tail = p_q->p_head;
    }

    p_q->size++;                     // update size

END_PUSH:

    return status;
}

void *deque_pop(deque_t *p_q)
{
    void *p_data = NULL;
    if (!p_q || 0 >= p_q->size)
    {
        return NULL;  
    }
    
    
    node_t *p_temp = p_q->p_head;
    p_data = p_temp->p_data; // get data to return

    p_q->p_head =p_temp->p_next; // set new head to next in line
    
    // If head is null, set tail to NULL as well
    if (!p_q->p_head) {
        p_q->p_tail = NULL;
    }
    
    free(p_temp);
    p_q->size--;
    
    return p_data;
}

int deque_enque(deque_t *p_q, void *p_data)
{
    int status = 1;

    if (!p_q)
    {
        fprintf(stderr, "[ERR] No dequeue present.\n");
        status = 0;
        goto END_ENQUE;
    }

    node_t *p_newnode = calloc(1, sizeof(node_t));
    if (!p_newnode)
    {
        perror("[ERR] Memory allocation failure\n");
        status = 0;
        goto END_ENQUE;
    }

    // put data into new node
    p_newnode->p_data = p_data;

    if (!p_q->p_tail) // if there is no tail present, set to head
    {
        p_q->p_head = p_newnode;
    }
    else
    {
        p_q->p_tail->p_next = p_newnode;
    }

    // set new node as tail
    p_q->p_tail = p_newnode;
    p_q->size++;

END_ENQUE:
    return status;
}


void *deque_deque(deque_t *p_q)
{
    void *p_data = NULL;
    if (!p_q || 0 >= p_q->size) // if no deque or empty
    {
        fprintf(stderr, "[Err] dequeue is empty\n");
        return p_data;
    }
        p_data = p_q->p_head->p_data;

        node_t *p_temp = p_q->p_head;      //copy current head to temp before we cut it from the chain
        p_q->p_head = p_q->p_head->p_next; // set new head as head's next
        free(p_temp); // free the previous head
        p_q->size--;

    printf("dqueueing this value: %d\n", *(int*)p_data);
    return p_data;
}

ssize_t deque_size(deque_t *p_q)
{
    ssize_t deqsize = 0;

    if (p_q)
    {
        deqsize = p_q->size;
    }

    return deqsize;
}

void deque_print(deque_t *p_q, void (*print_func)(void *data))
{
    if (!p_q || !print_func) 
    {
        return;
    }

    node_t *p_curr = p_q->p_head;
    while (p_curr)
    {
        print_func(p_curr->p_data);
        printf(" ");
        p_curr = p_curr->p_next;
    }
    puts("");
}

void print_int(void *data) 
{
    printf("%d", *(int*)data);
}