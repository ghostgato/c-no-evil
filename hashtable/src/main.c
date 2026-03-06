#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ht.h"

static char *
dup_string(const char *p_src)
{
    if (NULL == p_src)
    {
        return NULL;
    }

    size_t src_len = strlen(p_src) + 1U;
    char  *p_copy  = calloc(1U, src_len);

    if (NULL != p_copy)
    {
        memcpy(p_copy, p_src, src_len);
    }

    return p_copy;
}

static bool
put_owned_string(ht_t *p_htable, const char *p_key, const char *p_value)
{
    bool status = true;
    char *p_value_copy = dup_string(p_value);

    if (NULL == p_value_copy)
    {
        (void)fprintf(stderr, "Failed to allocate value for key '%s'\n", p_key);
        status = false;
        goto END_PUT_OWNED;
    }

    if (false == ht_put(p_htable, p_key, p_value_copy))
    {
        free(p_value_copy);
        status = false;
    }

END_PUT_OWNED:
    return status;
}

static void
del_string(void *p_data)
{
    free(p_data);
}

static void
print_value(void *p_value)
{
    printf("Value: %s\n", (char *)p_value);
}

int
main(void)
{
    int status_code = EXIT_SUCCESS;

    printf("Creating hash table...\n");
    ht_t *p_htable = ht_create(16U, NULL);
    if (NULL == p_htable)
    {
        (void)fprintf(stderr, "Failed to create hash table\n");
        status_code = EXIT_FAILURE;
        goto END_MAIN;
    }

    printf("Inserting key-value pairs...\n");
    if ((false == put_owned_string(p_htable, "apple", "fruit"))
        || (false == put_owned_string(p_htable, "carrot", "vegetable"))
        || (false == put_owned_string(p_htable, "banana", "fruit"))
        || (false == put_owned_string(p_htable, "pear", "fruit")))
    {
        (void)fprintf(stderr, "One or more inserts failed\n");
    }

    if (false == put_owned_string(p_htable, "apple", "green fruit"))
    {
        printf("Duplicate key 'apple' rejected as expected\n");
    }

    const char *p_keys[] = {"apple", "carrot", "banana", "pear", "orange"};
    size_t key_count = sizeof(p_keys) / sizeof(p_keys[0]);

    for (size_t idx = 0U; idx < key_count; idx++)
    {
        char *p_value = ht_get(p_htable, p_keys[idx]);
        if (NULL != p_value)
        {
            printf("Key '%s' => Value '%s'\n", p_keys[idx], p_value);
        }
        else
        {
            printf("Key '%s' not found\n", p_keys[idx]);
        }
    }

    char *p_new_value = dup_string("yellow fruit");
    if (NULL == p_new_value)
    {
        (void)fprintf(stderr, "Failed to allocate updated value\n");
    }
    else
    {
        char *p_old_value = ht_update(p_htable, "banana", p_new_value);
        if (NULL != p_old_value)
        {
            printf("Updated 'banana' from '%s' to 'yellow fruit'\n", p_old_value);
            free(p_old_value);
        }
        else
        {
            printf("Failed to update 'banana'\n");
            free(p_new_value);
        }
    }

    char *p_removed_value = ht_remove(p_htable, "carrot");
    if (NULL != p_removed_value)
    {
        printf("Removed 'carrot' with value '%s'\n", p_removed_value);
        free(p_removed_value);
    }
    else
    {
        printf("'carrot' not found to remove\n");
    }

    printf("Current keys in hash table:\n");
    ht_print_keys(p_htable);

    printf("Iterating over values:\n");
    ht_iter_values(p_htable, print_value);

    printf("Hash table size: %zu\n", ht_size(p_htable));

    printf("Destroying hash table...\n");
    ht_destroy(&p_htable, del_string);
    if (NULL == p_htable)
    {
        printf("Hash table destroyed successfully\n");
    }
    else
    {
        printf("Error: hash table not null after destroy\n");
    }

END_MAIN:
    return status_code;
}
