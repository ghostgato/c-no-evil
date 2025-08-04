#include <stdio.h>
#include "bst.h"

int compare_int(const void *a, const void *b)
{
    int int_a = *(int*)a;
    int int_b = *(int*)b;
    return int_a - int_b;
}

void print_int(const void *data)
{
    // turn into int pointer, then dereferenced as an int
    int num = *(int*)data;
    printf("%d ", num);
}


int main()
{
    int num1 = 5;
    int num2 = 4;
    int num3 = 9;
    int num4 = 3;
    int num5 = 6;
    int num6 = 8;
    int num7 = 10;
    int num8 = 5;

    bst_t *mytree = bst_create(compare_int);

    bst_insert(mytree, &num1);
    bst_insert(mytree, &num2);
    bst_insert(mytree, &num3);
    bst_insert(mytree, &num4);
    bst_insert(mytree, &num5);
    bst_insert(mytree, &num6);
    bst_insert(mytree, &num7);
    bst_insert(mytree, &num8);

    bst_print(mytree, print_int);

    puts("In Order: ");
    bst_traverse_inorder(mytree, print_int);
    puts("\nPost Traversal: ");
    bst_traverse_postorder(mytree, print_int);

    bst_destroy(&mytree, NULL);
}