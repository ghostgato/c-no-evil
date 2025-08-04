#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"

struct node
{
    struct node *p_left;
    struct node *p_right;
    void        *p_data;
};

struct bst
{
    node_t *p_root;
    cmp_f   cmp;
    ssize_t size;
};

// Define's instead of enum because of ssize_t width for some functions
//
#define ERROR   (-1)
#define FAILURE 0
#define SUCCESS 1

#define IS_RIGHT 0
#define IS_LEFT  1

#define EQUAL 0

static int insert_rec(node_t **pp_root, void *p_data, cmp_f cmp_func);
static int insert_iter(node_t **pp_root, void *p_data, cmp_f cmp_func);

static node_t *find_rec(node_t *p_root, const void *p_data, cmp_f cmp_func);
static void *find_iter(node_t *p_root, const void *p_data, cmp_f cmp_func);

static void print_rec(node_t *p_root, print_f print_func);

static void *find_min_rec(node_t *p_root);
static void *find_min_iter(node_t *p_root);

static void *find_max_rec(node_t *p_root);
static void *find_max_iter(node_t *p_root);

static inline node_t **get_child_ptr(node_t *p_node, int direction);

// Aux functions for printing
//
typedef struct trunk
{
    struct trunk *p_prev;
    char         *p_str;
} trunk_t;

static void print_trunks(trunk_t *p_trunk);
static void print_tree_rec(node_t  *p_root,
                           print_f  print_func,
                           trunk_t *p_prev,
                           int      b_is_left);

bst_t *
bst_create (cmp_f primary_cmp)
{
    bst_t *p_tree = NULL;

    if (NULL != primary_cmp)
    {
        p_tree = calloc(1, sizeof(*p_tree));
        if (NULL != p_tree)
        {
            p_tree->cmp = primary_cmp;
        }
    }

    return p_tree;
}

static void destroy_node(node_t *p_node, del_f del_func)
{
    if (!p_node)
    {
        return;
    }

    destroy_node(p_node->p_left, del_func);
    destroy_node(p_node->p_right, del_func);

    // destroy data if function i s provided
    if (del_func && p_node->p_data)
    {
        del_func(p_node->p_data);
    }
    free(p_node);
}

void bst_destroy(bst_t **pp_tree, del_f del_func)
{
    //bst_traverse_postorder(*pp_tree, free);
    if (!pp_tree || !*pp_tree)
    {
        return;
    }

    destroy_node((*pp_tree)->p_root, del_func);
    free(*pp_tree);
    *pp_tree = NULL;
}

int
bst_insert (bst_t *p_tree, void *p_data)
{
    int retval = FAILURE;

    if ((NULL != p_tree) && (NULL != p_data))
    {
        retval = insert_rec(&(p_tree->p_root), p_data, p_tree->cmp);
        // retval = insert_iter(&(p_tree->p_root), p_data, p_tree->cmp);
    }

    if (SUCCESS == retval)
    {
        p_tree->size++;
    }

    return retval;
}

static int
insert_rec (node_t **pp_root, void *p_data, cmp_f cmp_func)
{
    int retval = FAILURE;

    if (NULL == *pp_root)
    {
        node_t *p_new = calloc(1, sizeof(*p_new));
        if (NULL != p_new)
        {
            p_new->p_data = p_data;
            *pp_root      = p_new;
            retval        = SUCCESS;
        }
    }
    else
    {
        int cmp_result = cmp_func(p_data, (*pp_root)->p_data);

        if (EQUAL != cmp_result)
        {
            int direction = (cmp_result < EQUAL) ? IS_LEFT : IS_RIGHT;

            node_t **pp_next = get_child_ptr(*pp_root, direction);
            retval           = insert_rec(pp_next, p_data, cmp_func);
        }
    }

    return retval;
}



static inline node_t **
get_child_ptr (node_t *p_node, int direction)
{
    return (IS_LEFT == direction) ? &(p_node->p_left) : &(p_node->p_right);
}

static int
insert_iter (node_t **pp_root, void *p_data, cmp_f cmp_func)
{
    int      retval     = FAILURE;
    node_t **pp_current = pp_root;

    while (NULL != *pp_current)
    {
        int cmp_result = cmp_func(p_data, (*pp_current)->p_data);

        if (EQUAL == cmp_result)
        {
            goto EXIT_INSERT_ITER;
        }

        int direction = (cmp_result < EQUAL) ? IS_LEFT : IS_RIGHT;
        pp_current    = get_child_ptr(*pp_current, direction);
    }

    node_t *p_new = calloc(1, sizeof(*p_new));
    if (NULL != p_new)
    {
        p_new->p_data = p_data;
        *pp_current   = p_new;
        retval        = SUCCESS;
    }

EXIT_INSERT_ITER:
    return retval;
}

static node_t *remove_rec(node_t **pp_root, const void *p_query, cmp_f cmp_func)
{
    if (*pp_root == NULL)
    {
        return NULL;
    }

    int cmp_result = cmp_func(p_query, (*pp_root)->p_data);
    node_t *p_target = NULL;

    if (cmp_result < 0) // search left tree
    {
        (*pp_root)->p_left = remove_rec(&(*pp_root)->p_left, p_query, cmp_func);
    }
    else if (cmp_result > 0) // search right tree
    {
        (*pp_root)->p_right = remove_rec(&(*pp_root)->p_right, p_query, cmp_func);
    }
    else // cmp result equal = match
    {
        // Node to delete found
        node_t *p_target = *pp_root;

        // Case 1: No children/ leaf nodes
        if (!p_target->p_left && !p_target->p_right)
        {
            //free(p_target); will be freed later on in bst_remove()
            *pp_root = NULL;
        }

        // separate r/l child to prevent accidentally catching case 3 in a
        // case 2 by using !left || !right

        // Case 2: set child to root
        // Case 2: One child (right)
        else if (!p_target->p_left)
        {
            *pp_root = p_target->p_right;
            //free(p_target);
        }
        // Case 2: One child (left)
        else if (!p_target->p_right)
        {
            *pp_root = p_target->p_left;
            //free(p_target);
        }

        // Case 3: Two children
        else
        {
            // Find min node in right subtree
            node_t **pp_min = &p_target->p_right;
            while ((*pp_min)->p_left)
            {
                // get the address of the minimum value in the right tree
                pp_min = &(*pp_min)->p_left;
            }

            node_t *min_node = *pp_min;

            // replace target node with the value of the lowest number in the
            // right sub tree
            p_target->p_data = min_node->p_data;

            // Remove the min_node from right subtree recursively
            (*pp_root)->p_right = remove_rec(&p_target->p_right, min_node->p_data, cmp_func);
        }
    }

    return p_target;
}


void *bst_remove(bst_t *p_tree, const void *p_query, cmp_f search_cmp)
{
    if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == p_query))
    {
        return NULL;
    }

    // if no search func provided, use default cmp func
    cmp_f cmp_func = (search_cmp) ? search_cmp : p_tree->cmp;
    //node_t *p_target = bst_find(p_tree, p_query, search_cmp);
    node_t *p_target = remove_rec(&(p_tree->p_root), p_query, cmp_func);
    void *p_data = NULL; // will hold the data that we will return

    if (NULL != p_data)
    {
        p_data = p_target->p_data;
        p_tree->size--;
        free(p_target);
    }

    return p_data;
}


node_t *
bst_find (const bst_t *p_tree, const void *p_query, cmp_f search_cmp)
{
    if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == p_query))
    {
        return NULL;
    }

    cmp_f comp = p_tree->cmp;
    if (NULL != search_cmp)
    {
        comp = search_cmp;
    }

    return find_rec(p_tree->p_root, p_query, comp);
    // return find_iter(p_tree->p_root, p_query, comp);
}

node_t *
find_rec (node_t *p_root, const void *p_query, cmp_f search_cmp)
{
    node_t *p_target = NULL;

    if (NULL != p_root)
    {
        int cmp_result = search_cmp(p_query, p_root->p_data);

        if (EQUAL == cmp_result)
        {
            p_target = p_root;
        }
        else
        {
            int      direction = (cmp_result < EQUAL) ? IS_LEFT : IS_RIGHT;
            node_t **pp_next   = get_child_ptr(p_root, direction);
            p_target           = find_rec(*pp_next, p_query, search_cmp);
        }
    }

    return p_target;
}

static void *
find_iter (node_t *p_root, const void *p_query, cmp_f search_cmp)
{
    void   *p_retval  = NULL;
    node_t *p_current = p_root;

    while (NULL != p_current)
    {
        int cmp_result = search_cmp(p_query, p_current->p_data);

        if (EQUAL == cmp_result)
        {
            p_retval = p_current->p_data;
            break;
        }

        int      direction = (cmp_result < EQUAL) ? IS_LEFT : IS_RIGHT;
        node_t **pp_next   = get_child_ptr(p_current, direction);
        p_current          = *pp_next;
    }

    return p_retval;
}

ssize_t bst_size(const bst_t *p_tree)
{
    if (!p_tree)
    {
        return 0;
    }

    return p_tree->size;
}

static ssize_t height_rec(node_t *p_root)
{
    if (NULL == p_root)
    {
        return ERROR;
    }
    
    ssize_t left_height = height_rec(p_root->p_left);
    ssize_t right_height = height_rec(p_root->p_right);
    
    return 1 + ((left_height > right_height) ? left_height : right_height);
}

ssize_t bst_height(const bst_t *p_tree)
{
    if (NULL == p_tree)
    {
        return ERROR;
    }

    return height_rec(p_tree->p_root);
}

int
bst_is_empty (const bst_t *p_tree)
{
    if (NULL == p_tree)
    {
        return ERROR;
    }

    return (0 == p_tree->size) ? SUCCESS : FAILURE;
}

static void traverse_inorder(node_t *p_node, visit_f visitfunc)
{
    if (!p_node)
    {
        return;
    }

    traverse_inorder(p_node->p_left, visitfunc);
    visitfunc(p_node->p_data);
    traverse_inorder(p_node->p_right, visitfunc);
}

void bst_traverse_inorder(const bst_t *p_tree, visit_f visit_func)
{
    if (!p_tree || !visit_func) //? should visit func be included here?
    {
        return;
    }

    traverse_inorder(p_tree->p_root, visit_func);
}

static void traverse_preorder(node_t *p_node, visit_f visitfunc)
{
    if (!p_node)
    {
        return;
    }

    visitfunc(p_node->p_data);
    traverse_preorder(p_node->p_left, visitfunc);
    traverse_preorder(p_node->p_right, visitfunc);
}

void bst_traverse_preorder(const bst_t *p_tree, visit_f visit_func)
{
    if (!p_tree || !visit_func)
    {
        return;
    }

    traverse_preorder(p_tree->p_root, visit_func);
}

static void traverse_post(node_t *p_node, visit_f visitfunc)
{
    if (!p_node)
    {
        return;
    }

    traverse_post(p_node->p_left, visitfunc);
    traverse_post(p_node->p_right, visitfunc);
    visitfunc(p_node->p_data);
}

void bst_traverse_postorder(const bst_t *p_tree, visit_f visit_func)
{
    if (!p_tree || !visit_func)
    {
        return;
    }

    traverse_post(p_tree->p_root, visit_func);
}

void bst_traverse_levelorder(const bst_t *p_tree, visit_f visit_func);

void *
bst_find_min (const bst_t *p_tree)
{
    if ((NULL == p_tree) || (NULL == p_tree->p_root))
    {
        return NULL;
    }
    return find_min_rec(p_tree->p_root);
    // return find_min_iter(p_tree->p_root);
}

static void *
find_min_rec (node_t *p_root)
{
    if (NULL == p_root->p_left)
    {
        return p_root->p_data;
    }

    return find_min_rec(p_root->p_left);
}

static void *
find_min_iter (node_t *p_root)
{
    node_t *p_current = p_root;

    while (NULL != p_current->p_left)
    {
        p_current = p_current->p_left;
    }

    return p_current->p_data;
}

void *
bst_find_max (const bst_t *p_tree)
{
    if ((NULL == p_tree) || (NULL == p_tree->p_root))
    {
        return NULL;
    }
    return find_max_rec(p_tree->p_root);
    // return find_max_iter(p_tree->p_root);
}

static void *
find_max_rec (node_t *p_root)
{
    if (NULL == p_root->p_right)
    {
        return p_root->p_data;
    }

    return find_max_rec(p_root->p_right);
}

static void *
find_max_iter (node_t *p_root)
{
    node_t *p_current = p_root;

    while (NULL != p_current->p_right)
    {
        p_current = p_current->p_right;
    }

    return p_current->p_data;
}

void
bst_print (const bst_t *p_tree, print_f print_func)
{
    if (NULL == print_func)
    {
        return;
    }

    if (NULL == p_tree)
    {
        puts("(empty)");
    }
    else
    {
        print_tree_rec(p_tree->p_root, print_func, NULL, IS_RIGHT);
        puts("");
    }
}

static void
print_trunks (trunk_t *p_trunk)
{
    if (NULL == p_trunk)
    {
        return;
    }

    print_trunks(p_trunk->p_prev);
    printf("%s", p_trunk->p_str);
}

static void
print_tree_rec (node_t  *p_root,
                print_f  print_func,
                trunk_t *p_prev,
                int      b_is_left)
{
    if (NULL == p_root)
    {
        return;
    }

    trunk_t this_trunk = { p_prev, "     " };
    char   *p_prev_str = this_trunk.p_str;

    print_tree_rec(p_root->p_right, print_func, &this_trunk, IS_LEFT);

    if (NULL == p_prev)
    {
        this_trunk.p_str = "---";
    }
    else if (b_is_left)
    {
        this_trunk.p_str = ".--";
        p_prev_str       = "    |";
    }
    else
    {
        this_trunk.p_str = "`--";
        p_prev->p_str    = p_prev_str;
    }

    print_trunks(&this_trunk);
    print_func(p_root->p_data);
    puts("");

    if (NULL != p_prev)
    {
        p_prev->p_str = p_prev_str;
    }
    this_trunk.p_str = "    |";

    print_tree_rec(p_root->p_left, print_func, &this_trunk, IS_RIGHT);
}

static int validate_rec(node_t *p_root, void *min_val, void *max_val, cmp_f cmp_func)
{
    if (NULL == p_root)
    {
        return SUCCESS;
    }
    
    if ((min_val && cmp_func(p_root->p_data, min_val) <= 0) ||
        (max_val && cmp_func(p_root->p_data, max_val) >= 0))
    {
        return FAILURE;
    }
    
    // Recursively validate left and right subtrees
    return validate_rec(p_root->p_left, min_val, p_root->p_data, cmp_func) &&
            validate_rec(p_root->p_right, p_root->p_data, max_val, cmp_func);
}

int bst_is_valid(const bst_t *p_tree)
{
    if (NULL == p_tree)
    {
        return ERROR;
    }

    return validate_rec(p_tree->p_root, NULL, NULL, p_tree->cmp);
}

static node_t *copy_node(node_t *p_source)
{
    if (NULL == p_source)
    {
        return NULL;
    }
    
    node_t *p_new = calloc(1, sizeof(*p_new));
    if (NULL == p_new)
    {
        return NULL;
    }
    
    p_new->p_data = p_source->p_data; // Shallow copy of data
    p_new->p_left = copy_node(p_source->p_left);
    p_new->p_right = copy_node(p_source->p_right);
    
    return p_new;
}

bst_t *bst_copy(const bst_t *p_tree)
{
    if (NULL == p_tree)
    {
        return NULL;
    }

    bst_t *newtree = calloc(1, sizeof(bst_t));
    if (!newtree)
    {
        return NULL;
    }

    newtree->cmp = p_tree->cmp;
    newtree->size = p_tree->size;
    newtree->p_root = copy_node(p_tree->p_root);

    return newtree;
}

void bst_clear(bst_t *p_tree, del_f del_func)
{
    if (!p_tree || !p_tree->p_root)
    {
        return;
    }
    
    destroy_node(p_tree->p_root, del_func);
    p_tree->p_root = NULL;
    p_tree->size = 0;
}
