#ifndef BST_H
#define BST_H

#include <sys/types.h>

// Forward declaration of opaque types
typedef struct bst bst_t;
typedef struct node node_t;

/**
 * Function pointer type for comparing two complete data elements.
 * Used for maintaining tree structure and insertion order.
 *
 * @param p_left  Pointer to first data element
 * @param p_right Pointer to second data element
 * @return < 0 if p_left < p_right, 0 if equal, > 0 if p_left > p_right
 */
typedef int (*cmp_f)(const void *p_left, const void *p_right);

/**
 * Function pointer type for printing data elements.
 *
 * @param p_data Pointer to data element to print
 */
typedef void (*print_f)(const void *p_data);

/**
 * Function pointer type for deallocating data elements.
 *
 * @param p_data Pointer to data element to deallocate
 */
typedef void (*del_f)(void *p_data);

/**
 * Function pointer type for visiting data elements during traversal.
 *
 * @param p_data Pointer to data element being visited
 */
typedef void (*visit_f)(const void *p_data);

/**
 * Creates a new binary search tree.
 * The primary comparison function determines tree structure and must be
 * consistent for all insertions to maintain BST property.
 *
 * @param primary_cmp Function to compare data elements for tree ordering
 * @return Pointer to new BST, or NULL if allocation fails
 */
bst_t *bst_create(cmp_f primary_cmp);

/**
 * Destroys a binary search tree and optionally deallocates all data elements.
 * Sets the tree pointer to NULL after destruction.
 *
 * @param pp_tree    Double pointer to BST to destroy
 * @param del_func Function to deallocate data, or NULL to skip deallocation
 */
void bst_destroy(bst_t **pp_tree, del_f del_func);

/**
 * Inserts a data element into the BST using the primary comparison function.
 * Duplicate elements (as determined by primary comparator) are not inserted.
 *
 * @param p_tree Pointer to BST
 * @param p_data Pointer to data element to insert
 * @return 1 on successful insertion, 0 on failure or duplicate
 */
int bst_insert(bst_t *p_tree, void *p_data);

/**
 * Removes a data element from the BST.
 * Uses search comparator if provided, otherwise uses primary comparator.
 *
 * @param p_tree      Pointer to BST
 * @param p_query     Pointer to key (may be partial data or different type)
 * @param search_cmp Optional search comparison function, or NULL to use primary
 * @return Pointer to removed data element, or NULL if not found
 */
void *bst_remove(bst_t *p_tree, const void *p_query, cmp_f search_cmp);

/**
 * Finds a data element in the BST.
 * Uses search comparator if provided, otherwise uses primary comparator.
 * Search comparator allows finding by partial data (e.g., search person by name
 * string).
 *
 * Example:
 *   // Tree stores person_t* using person_full_cmp
 *   // But search by name string using person_name_cmp
 *   person_t *found = bst_find(tree, "John Doe", person_name_cmp);
 *
 * @param p_tree      Pointer to BST
 * @param p_query     Pointer to key (may be partial data or different type)
 * @param search_cmp Optional search comparison function, or NULL to use primary
 * @return Pointer to found data element, or NULL if not found
 */
node_t *bst_find(const bst_t *p_tree, const void *p_query, cmp_f search_cmp);

/**
 * Returns the number of elements in the BST.
 *
 * @param p_tree Pointer to BST
 * @return Number of elements, or -1 if tree is NULL
 */
ssize_t bst_size(const bst_t *p_tree);

/**
 * Returns the height of the BST (longest path from root to leaf).
 *
 * @param p_tree Pointer to BST
 * @return Height of tree (0 for single node, -1 for empty tree)
 */
ssize_t bst_height(const bst_t *p_tree);

/**
 * Checks if the BST is empty.
 *
 * @param p_tree Pointer to BST
 * @return 1 if empty, 0 if contains elements, -1 if tree is NULL
 */
int bst_is_empty(const bst_t *p_tree);

/**
 * Performs inorder traversal of BST (left, root, right).
 * For BST, this visits elements in sorted order according to primary
 * comparator.
 *
 * @param p_tree       Pointer to BST
 * @param visit_func Function to call for each element
 */
void bst_traverse_inorder(const bst_t *p_tree, visit_f visit_func);

/**
 * Performs preorder traversal of BST (root, left, right).
 *
 * @param p_tree       Pointer to BST
 * @param visit_func Function to call for each element
 */
void bst_traverse_preorder(const bst_t *p_tree, visit_f visit_func);

/**
 * Performs postorder traversal of BST (left, right, root).
 *
 * @param p_tree       Pointer to BST
 * @param visit_func Function to call for each element
 */
void bst_traverse_postorder(const bst_t *p_tree, visit_f visit_func);

/**
 * Performs level-order (breadth-first) traversal of BST.
 *
 * @param p_tree       Pointer to BST
 * @param visit_func Function to call for each element
 */
void bst_traverse_levelorder(const bst_t *p_tree, visit_f visit_func);

/**
 * Prints the BST structure for debugging.
 *
 * @param p_tree       Pointer to BST
 * @param print_func Function to print individual data elements
 */
void bst_print(const bst_t *p_tree, print_f print_func);

/**
 * Finds the minimum element in the BST according to primary comparator.
 *
 * @param p_tree Pointer to BST
 * @return Pointer to minimum data element, or NULL if tree is empty
 */
void *bst_find_min(const bst_t *p_tree);

/**
 * Finds the maximum element in the BST according to primary comparator.
 *
 * @param p_tree Pointer to BST
 * @return Pointer to maximum data element, or NULL if tree is empty
 */
void *bst_find_max(const bst_t *p_tree);

/**
 * Validates that the BST maintains proper ordering according to primary
 * comparator. Useful for debugging and testing.
 *
 * @param p_tree Pointer to BST
 * @return 1 if valid BST structure, 0 if invalid, -1 if tree is NULL
 */
int bst_is_valid(const bst_t *p_tree);

/**
 * Creates a deep copy of the BST using the same primary comparator.
 * Data elements themselves are not copied - only tree structure is duplicated.
 *
 * @param p_tree Pointer to BST to copy
 * @return Pointer to new BST copy, or NULL if allocation fails
 */
bst_t *bst_copy(const bst_t *p_tree);

/**
 * Removes all elements from BST but keeps the tree structure intact.
 *
 * @param p_tree     Pointer to BST
 * @param del_func Function to deallocate data, or NULL to skip deallocation
 */
void bst_clear(bst_t *p_tree, del_f del_func);

/**
 * Counts the number of leaf nodes (nodes with no children) in the BST.
 *
 * @param p_tree Pointer to BST
 * @return Number of leaf nodes, or -1 if tree is NULL
 */
ssize_t bst_count_leaves(const bst_t *p_tree);

/**
 * Counts the number of internal nodes (nodes with at least one child) in the
 * BST.
 *
 * @param p_tree Pointer to BST
 * @return Number of internal nodes, or -1 if tree is NULL
 */
ssize_t bst_count_internal_nodes(const bst_t *p_tree);

/**
 * Finds the inorder successor of a given element in the BST.
 * The successor is the next element in sorted order according to primary
 * comparator. Uses search comparator if provided, otherwise uses primary
 * comparator.
 *
 * @param p_tree      Pointer to BST
 * @param p_query     Pointer to search key
 * @param search_cmp Optional search comparison function, or NULL to use primary
 * @return Pointer to successor data element, or NULL if no successor exists
 */
void *bst_successor(const bst_t *p_tree, const void *p_query, cmp_f search_cmp);

/**
 * Finds the inorder predecessor of a given element in the BST.
 * The predecessor is the previous element in sorted order according to primary
 * comparator. Uses search comparator if provided, otherwise uses primary
 * comparator.
 *
 * @param p_tree      Pointer to BST
 * @param p_query     Pointer to search key
 * @param search_cmp Optional search comparison function, or NULL to use primary
 * @return Pointer to predecessor data element, or NULL if no predecessor exists
 */
void *bst_predecessor(const bst_t *p_tree,
                      const void  *p_query,
                      cmp_f        search_cmp);

#endif