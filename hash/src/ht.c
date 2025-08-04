#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ht.h"

#define BALANCE_FACTOR 0.65f
#define HASH_SEED      5381

typedef struct ht_node
{
    char           *p_key;
    void           *p_value;
    struct ht_node *p_next; // using chaining for anti collision
} ht_node;

struct ht
{
    ht_node **pp_items;    // nodes in the table
    size_t    table_size; // number of buckets or indexes in table
    size_t    node_count; // number of nodes entered in total
    hash_f    hash_func;  // function to hash keys
};

static size_t
default_hash (const char *p_key)
{
    // using djb2 hash algorithm
    // https: // theartincode.stanis.me/008-djb2/
    size_t hash = HASH_SEED;
    int    idx  = 0;

    while ((idx = *p_key++))
    {
        hash = ((hash << 5) + hash) + idx; //  hash = hash * 33 + idx;
    }

    return hash;
}

/**
 * @brief Create a node object for hash table
 *
 * @param p_key key associated with value pair
 * @param p_value value assigned to key
 * @return ht_node*
 */
static ht_node *
create_node (const char *p_key, void *p_value)
{
    if (!p_key || !p_value)
    {
        fprintf(stderr, "[ERR] Invalid variables to create new node.\n");
        return NULL;
    }

    ht_node *p_new = calloc(1, sizeof(ht_node));
    if (!p_new)
    {
        perror("[ERR] Memory allocation fail - create_node()\n");
        goto END_CREATE;
    }

    p_new->p_key = calloc(1, (strlen(p_key) + 1));
    if (!p_new->p_key)
    {
        perror("[ERR] Memory Allocation Fail - create_node(): p_key\n");
        free(p_new);
    }
    else
    {
        strcpy(p_new->p_key, p_key);
        p_new->p_value = p_value;
        p_new->p_next  = NULL;
    }

END_CREATE:
    return p_new;
}

static void free_node(ht_node *p_node, del_f del)
{
    if (!p_node)
    {
        fprintf(stderr, "[ERR] No node provided to free - free_node()\n");
        return;
    }

    free(p_node->p_key);

    if (del)
    {
        del(p_node->p_value);
    }

    free(p_node);
}

// trying out inline for use in this simple function that returns a simple boolean value
static inline int needs_resize(ht_t *p_htable)
{
    return ((float)p_htable->node_count / p_htable->table_size) > BALANCE_FACTOR;
}

static int resize_table(ht_t *p_htable)
{
    if (!p_htable)
    {
        fprintf(stderr, "[ERR] No valid table to resize - resizetable()\n");
        return 0;
    }

    int status = 1; // start success
    size_t og_size = p_htable->table_size; // well need this to iterateog items
    size_t new_size = p_htable->table_size * 2; // double size

    ht_node **pp_og_items = p_htable->pp_items;
    ht_node **pp_new_items = calloc(new_size, sizeof(ht_node*));
    if (!pp_new_items)
    {
        perror("[ERR] Memory allocation failure - resize_table().\n");
        status = 0;
    }

    if (status)
    {
        // resize table struct then rahash every item and save back in 

        // RESIZE
        p_htable->pp_items = pp_new_items;
        p_htable->table_size = new_size;
        p_htable->node_count = 0; // we will update this as we rehash

        //REHASH
        for (size_t idx = 0; idx < og_size; idx++)
        {
            ht_node *p_curr = pp_og_items[idx];

            while (p_curr)
            {
                ht_node *p_next = p_curr->p_next; // set up for next in chain

                //get new index based on new size
                size_t new_idx = p_htable->hash_func(p_curr->p_key) % new_size;
                p_curr->p_next = pp_new_items[new_idx];
                pp_new_items[new_idx] = p_curr;
                p_htable->node_count++;

                p_curr = p_next;
            }
        }

        free(pp_og_items);
    }

    return status;
}

ht_t *
ht_create (size_t initial_size, hash_f hash)
{
    if (initial_size <= 0)
    {
        fprintf(stderr, "[ERR] Invalid size provided to create new table.\n");
        return NULL;
    }

    // initialize hash table struct
    ht_t *p_htable = calloc(1, sizeof(ht_t));
    if (!p_htable)
    {
        return NULL;
    }

    // initialize array of items
    p_htable->pp_items = calloc(initial_size, sizeof(ht_node*));
    if (!p_htable->pp_items)
    {
        perror("[ERR] Memory allocation fail - ht_create() pp_items");
    }
    

    if (hash) // if a hash func is provided
    {
        p_htable->hash_func = hash;
    }
    else
    {
        p_htable->hash_func = default_hash;
    }

    p_htable->table_size = initial_size;
    p_htable->node_count = 0;

    return p_htable;
}

void
ht_destroy (ht_t **pp_htable, del_f del)
{
    if (!pp_htable || !*pp_htable) // check at different levels
    {
        fprintf(stderr, "[ERR] Invalid hash table provided.\n");
        return;
    }

    ht_t *p_htable = *pp_htable; // dereference to get to the actual table

    // Go through each bucket in the hash table
    for (size_t idx = 0; idx < p_htable->table_size; idx++)
    {
        ht_node *p_current = p_htable->pp_items[idx];

        // will need to clear any nodes that are chained to the same hash
        while (p_current)
        {
            ht_node *p_next = p_current->p_next;
            free_node(p_current, del);
            p_current = p_next;
        }
    }

    free(p_htable->pp_items);
    free(p_htable);
    *pp_htable = NULL;
}

/**
 * Inserts a new key-value pair into the hashtable.
 * Creates an internal copy of the key string.
 * If the key already exists, the operation fails without modifying the table.
 *
 * Performance: Average O(1), worst case O(n) if many collisions occur
 *
 * @param p_htable Pointer to hashtable
 * @param p_key    Pointer to null-terminated string key
 * @param p_value  Pointer to value to associate with key
 *
 * @return true on success, false on failure or if key already exists
 */
bool
ht_put (ht_t *p_htable, const char *p_key, void *p_value)
{
    if (!p_htable || !p_key || !p_value)
    {
        fprintf(stderr, "[ERR] Invalid variables provided to insert\n");
        return false;
    }

    bool   status = true; // start success
    size_t index  = p_htable->hash_func(p_key) % p_htable->table_size;

    // Check if key already exists
    ht_node *p_current = p_htable->pp_items[index];
    while (p_current)
    {
        if (strcmp(p_current->p_key, p_key) == 0)
        {
            // if key already exists
            status = false;
            break;
        }
        p_current = p_current->p_next;
    }

    if (status)
    {
        // Create new node
        ht_node *p_new_node = create_node(p_key, p_value);
        if (!p_new_node)
        {
            fprintf(stderr, "[ERR] Memory allocation failure.\n");
            status = false;
        }
        else
        {
            // Insert at beginning of chain
            // set curr head to next
            p_new_node->p_next       = p_htable->pp_items[index];
            p_htable->pp_items[index] = p_new_node; // set new node to head
            p_htable->node_count++;

            // Check if resize is needed
            if (needs_resize(p_htable))
            {
                resize_table(p_htable);
            }
        }
    }

    return status;
}

void *
ht_get (ht_t *p_htable, const char *p_key)
{
    if (!p_htable || !p_key)
    {
        fprintf(stderr, "[ERR] Invalid variables provided - ht_get()\n");
        return NULL;
    }

    // get index using the provided hash key
    size_t   index     = p_htable->hash_func(p_key) % p_htable->table_size;
    ht_node *p_current = p_htable->pp_items[index];
    void    *p_data    = NULL;

    // in case there are other nodes chained, start at head
    while (p_current)
    {
        if (0 == (strcmp(p_current->p_key, p_key))) // find hashkey matchup
        {
            p_data = p_current->p_value;
            break;
        }
        p_current = p_current->p_next;
    }

    return p_data;
}

void *
ht_remove (ht_t *p_htable, const char *p_key)
{
    if (!p_htable || !p_key)
    {
        fprintf(stderr, "[ERR] Invalid variables provided - ht_remove().\n");
        return NULL;
    }

    size_t   index     = p_htable->hash_func(p_key) % p_htable->table_size;
    ht_node *p_current = p_htable->pp_items[index];
    ht_node *p_prev    = NULL;
    void    *p_data    = NULL;

    while (p_current) // iterate through chain (sharing same hash)
    {
        if (0 == (strcmp(p_current->p_key, p_key))) // key match found
        {
            p_data = p_current->p_value;

            // Remove from chain
            if (p_prev) // if not removing the HEAD of chain
            {
                p_prev->p_next = p_current->p_next;
            }
            else // removing head
            {
                p_htable->pp_items[index] = p_current->p_next;
            }

            free(p_current->p_key);
            free(p_current);
            p_htable->node_count--;

            break;
        }

        p_prev    = p_current;
        p_current = p_current->p_next;
    }

    return p_data;
}

void *
ht_update (ht_t *p_htable, const char *p_key, void *p_new_value)
{
    if (!p_htable || !p_key || !p_new_value)
    {
        fprintf(stderr, "[ERR] Invalid variables provided - ht_update()\n");
        return NULL;
    }

    // get index
    size_t   index  = (p_htable->hash_func(p_key)) % p_htable->table_size;
    ht_node *p_curr = p_htable->pp_items[index];
    void    *p_data = NULL; // will hold return value

    // go through potential chain
    while (p_curr)
    {
        if (0 == strcmp(p_curr->p_key, p_key)) // if match found
        {
            p_data          = p_curr->p_value; // get old value
            p_curr->p_value = p_new_value;     // set new value
            break;                             // exit loop return old data
        }
        p_curr = p_curr->p_next;
    }

    return p_data; // returning old data or null;
}

/**
 * Returns the number of key-value pairs in the hashtable.
 *
 * @param p_htable Pointer to hashtable
 *
 * @return Number of key-value pairs, 0 if table is empty/NULL
 */
// ? when checking for a vallid table should we provide size max value for
// invalid

size_t
ht_size (ht_t *p_htable)
{
    size_t size = 0;

    if (!p_htable)
    {
        fprintf(stderr, "[ERR] No valid hash table provided - ht_size()\n");
    }
    else
    {
        size = p_htable->table_size;
    }

    return size;
}

void
ht_iter_values (ht_t *p_htable, iter_f iter)
{
    if (!p_htable || !iter)
    {
        fprintf(stderr, "[ERR] Invalid variables provided- ht_iter_values()\n");
        return;
    }

    // go through each bucket
    for (size_t idx = 0; idx < p_htable->table_size; idx++)
    {
        // go through each chain
        ht_node *p_curr = p_htable->pp_items[idx];
        while (p_curr)
        {
            iter(p_curr->p_value);
            p_curr = p_curr->p_next;
        }
    }
}

/**
 * Prints all keys in the hashtable to standard output.
 * Useful for debugging and inspection.
 * Order of output is not guaranteed.
 *
 * @param p_htable Pointer to hashtable
 */
void
ht_print_keys (ht_t *p_htable)
{
    if (!p_htable)
    {
        fprintf(stderr, "[ERR] Invalid hashtable provided - print_keys()\n");
        return;
    }

    if (p_htable->table_size < 1)
    {
        fprintf(stderr, "Hash table empty, Nothing to print.\n");
        return;
    }

    printf("Keys in hash table:\n");
    for (size_t idx = 0; idx < p_htable->table_size; idx++)
    {
        ht_node *p_curr = p_htable->pp_items[idx];

        while (p_curr)
        {
            printf("[%zu] %s\n", idx, p_curr->p_key);
            p_curr = p_curr->p_next;
        }
    }
}
