#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ht.h"

void print_string(void *data) {
    printf("%s ", (char*)data);
}

// Simple del function to free dynamically allocated strings
void del_string(void *p_data)
{
    free(p_data);
}

// Print function for iterating values (assumes values are strings)
void print_value(void *p_value)
{
    printf("Value: %s\n", (char *)p_value);
}

int main() {
    
    printf("Creating hash table...\n");
    ht_t *htable = ht_create(16, NULL);
    if (!htable)
    {
        fprintf(stderr, "Failed to create hash table\n");
        return 1;
    }

    // Insert some key-value pairs (keys are literals, values dynamically allocated strings)
    printf("Inserting key-value pairs...\n");
    ht_put(htable, "apple", strdup("fruit"));
    ht_put(htable, "carrot", strdup("vegetable"));
    ht_put(htable, "banana", strdup("fruit"));
    ht_put(htable, "pear", strdup("fruit"));

    // Attempt duplicate insert
    // if (!ht_put(htable, "apple", strdup("green fruit")))
    //     printf("Duplicate key 'apple' rejected as expected\n");

    // Get values for keys
    const char *keys[] = {"apple", "carrot", "banana", "pear", "orange"};
    for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
    {
        char *val = ht_get(htable, keys[i]);
        if (val)
            printf("Key '%s' => Value '%s'\n", keys[i], val);
        else
            printf("Key '%s' not found\n", keys[i]);
    }

    // Update value for existing key
    char *old_val = ht_update(htable, "banana", strdup("yellow fruit"));
    if (old_val)
    {
        printf("Updated 'banana' from '%s' to 'yellow fruit'\n", old_val);
        free(old_val);
    }
    else
        printf("Failed to update 'banana'\n");

    // Remove a key
    char *removed_val = ht_remove(htable, "carrot");
    if (removed_val)
    {
        printf("Removed 'carrot' with value '%s'\n", removed_val);
        free(removed_val);
    }
    else
        printf("'carrot' not found to remove\n");

    // Print keys
    printf("Current keys in hash table:\n");
    ht_print_keys(htable);

    // Iterate over values
    printf("Iterating over values:\n");
    ht_iter_values(htable, print_value);

    // Size of the table
    printf("Hash table size: %zu\n", ht_size(htable));

    // Clear and destroy
    printf("Destroying hash table...\n");
    ht_destroy(&htable, del_string);
    if (htable == NULL)
        printf("Hash table destroyed successfully\n");
    else
        printf("Error: hash table not null after destroy\n");

    return 0;
}
