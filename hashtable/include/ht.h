#ifndef HT_H
#define HT_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ht ht_t;

/**
 * Function pointer type for hashing string keys.
 * Should produce consistent hash values for identical strings.
 *
 * @param p_key Pointer to null-terminated string key to hash
 *
 * @return Hash value for the key
 */
typedef size_t (*hash_f)(const char *p_key);

/**
 * Function pointer type for deallocating value elements.
 *
 * @param p_data Pointer to value element to deallocate
 */
typedef void (*del_f)(void *p_data);

/**
 * Function pointer type for visiting values during iteration.
 *
 * @param p_value Pointer to value being visited
 */
typedef void (*iter_f)(void *p_value);

/**
 * Creates a new hashtable with specified initial capacity.
 * Uses a default hash function if none is provided.
 *
 * @param initial_size Initial number of buckets
 * @param hash         Optional hash function, or NULL to use default
 *
 * @return Pointer to new hashtable, or NULL if allocation fails
 */
ht_t *ht_create(size_t initial_size, hash_f hash);

/**
 * Destroys a hashtable and optionally deallocates all stored values.
 * Keys are automatically freed as they are internally managed strings.
 * Sets the hashtable pointer to NULL after destruction.
 *
 * @param pp_htable Double pointer to hashtable to destroy
 * @param del       Function to deallocate values, or NULL to skip deallocation
 */
void ht_destroy(ht_t **pp_htable, del_f del);

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
bool ht_put(ht_t *p_htable, const char *p_key, void *p_value);

/**
 * Retrieves the value associated with a key.
 *
 * Performance: Average O(1), worst case O(n) if many collisions occur
 *
 * @param p_htable Pointer to hashtable
 * @param p_key    Pointer to null-terminated string key to look up
 *
 * @return Pointer to associated value, or NULL if key not found
 */
void *ht_get(ht_t *p_htable, const char *p_key);

/**
 * Removes a key-value pair from the hashtable.
 * Returns the value that was associated with the key.
 * The key string is automatically deallocated.
 *
 * Performance: Average O(1), worst case O(n) if many collisions occur
 *
 * Note: The caller is responsible for deallocating the returned value
 * if necessary.
 *
 * @param p_htable Pointer to hashtable
 * @param p_key    Pointer to null-terminated string key to remove
 *
 * @return Pointer to value that was associated with key, or NULL if not found
 */
void *ht_remove(ht_t *p_htable, const char *p_key);

/**
 * Updates the value associated with an existing key.
 * Returns the previous value that was associated with the key.
 * If the key does not exist, the operation fails.
 *
 * Performance: Average O(1), worst case O(n) if many collisions occur
 *
 * @param p_htable    Pointer to hashtable
 * @param p_key       Pointer to null-terminated string key
 * @param p_new_value Pointer to new value to associate with key
 *
 * @return Pointer to previous value, or NULL if key not found
 */
void *ht_update(ht_t *p_htable, const char *p_key, void *p_new_value);

/**
 * Checks if a key exists in the hashtable. Does not return data
 *
 *
 *
 * @param p_htable Pointer to hashtable
 * @param p_key    Pointer to null-terminated string key to check
 *
 * @return true if key exists, false if key not found or invalid parameters
 */
bool ht_contains(ht_t *p_htable, const char *p_key);

/**
 * Returns the number of key-value pairs in the hashtable.
 *
 * @param p_htable Pointer to hashtable
 *
 * @return Number of key-value pairs, 0 if table is NULL
 */
size_t ht_size(ht_t *p_htable);

/**
 * Iterates over all values in the hashtable, applying the given function.
 * Order of iteration is not guaranteed and may change between calls.
 *
 * @param p_htable Pointer to hashtable
 * @param iter     Function to call for each value
 */
void ht_iter_values(ht_t *p_htable, iter_f iter);

/**
 * Prints all keys in the hashtable to standard output.
 * Useful for debugging and inspection.
 * Order of output is not guaranteed.
 *
 * @param p_htable Pointer to hashtable
 */
void ht_print_keys(ht_t *p_htable);

#endif