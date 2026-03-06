#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity/unity.h"

#include "ht.h"

void
setUp(void)
{
}

void
tearDown(void)
{
}

static size_t
constant_hash(const char *p_key)
{
    // all keys will have an index of 1, to cause collisions  and test chaining
    (void)p_key;
    return 1;
}

static void
noop_iter(void *p_value)
{
    (void)p_value;
}

static int g_iter_count = 0;

static void
test_create_valid(void)
{
    ht_t *p_table = ht_create(8U, NULL);

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_EQUAL_UINT64(0U, (uint64_t)ht_size(p_table));

    ht_destroy(&p_table, NULL);
    TEST_ASSERT_NULL(p_table);
}

static void
test_create_invalid_size(void)
{
    ht_t *p_table = ht_create(0U, NULL);

    TEST_ASSERT_NULL(p_table);
}

static void
test_destroy_invalid_params(void)
{
    ht_t *p_table = NULL;

    ht_destroy(NULL, NULL);
    ht_destroy(&p_table, NULL);

    TEST_ASSERT_NULL(p_table);
}

static void
test_put_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value = 42;

    TEST_ASSERT_NOT_NULL(p_table);

    TEST_ASSERT_FALSE(ht_put(NULL, "k", &value));
    TEST_ASSERT_FALSE(ht_put(p_table, NULL, &value));
    TEST_ASSERT_FALSE(ht_put(p_table, "k", NULL));

    ht_destroy(&p_table, NULL);
}

static void
test_get_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);

    TEST_ASSERT_NOT_NULL(p_table);

    TEST_ASSERT_NULL(ht_get(NULL, "k"));
    TEST_ASSERT_NULL(ht_get(p_table, NULL));

    ht_destroy(&p_table, NULL);
}

static void
test_remove_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);

    TEST_ASSERT_NOT_NULL(p_table);

    TEST_ASSERT_NULL(ht_remove(NULL, "k"));
    TEST_ASSERT_NULL(ht_remove(p_table, NULL));

    ht_destroy(&p_table, NULL);
}

static void
test_update_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value = 7;

    TEST_ASSERT_NOT_NULL(p_table);

    TEST_ASSERT_NULL(ht_update(NULL, "k", &value));
    TEST_ASSERT_NULL(ht_update(p_table, NULL, &value));
    TEST_ASSERT_NULL(ht_update(p_table, "k", NULL));

    ht_destroy(&p_table, NULL);
}

static void
test_contains_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);

    TEST_ASSERT_NOT_NULL(p_table);

    TEST_ASSERT_FALSE(ht_contains(NULL, "k"));
    TEST_ASSERT_FALSE(ht_contains(p_table, NULL));

    ht_destroy(&p_table, NULL);
}

static void
test_size_invalid_params(void)
{
    TEST_ASSERT_EQUAL_UINT64(0U, (uint64_t)ht_size(NULL));
}

static void
test_iter_values_invalid_params(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value = 1;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "k", &value));

    g_iter_count = 0;
    ht_iter_values(NULL, noop_iter);
    ht_iter_values(p_table, NULL);

    TEST_ASSERT_EQUAL_INT(0, g_iter_count);

    ht_destroy(&p_table, NULL);
}

static void
test_print_keys_invalid_params(void)
{
    ht_print_keys(NULL);
    TEST_PASS();
}

static void
test_put_get_contains_and_size(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value_a = 11;
    int value_b = 22;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "alpha", &value_a));
    TEST_ASSERT_TRUE(ht_put(p_table, "beta", &value_b));

    TEST_ASSERT_TRUE(ht_contains(p_table, "alpha"));
    TEST_ASSERT_TRUE(ht_contains(p_table, "beta"));
    TEST_ASSERT_FALSE(ht_contains(p_table, "gamma"));

    TEST_ASSERT_EQUAL_PTR(&value_a, ht_get(p_table, "alpha"));
    TEST_ASSERT_EQUAL_PTR(&value_b, ht_get(p_table, "beta"));
    TEST_ASSERT_NULL(ht_get(p_table, "gamma"));
    TEST_ASSERT_EQUAL_UINT64(2U, (uint64_t)ht_size(p_table));

    ht_destroy(&p_table, NULL);
}

static void
test_put_duplicate_key_rejected(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value_a = 1;
    int value_b = 2;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "dup", &value_a));
    TEST_ASSERT_FALSE(ht_put(p_table, "dup", &value_b));

    TEST_ASSERT_EQUAL_PTR(&value_a, ht_get(p_table, "dup"));
    TEST_ASSERT_EQUAL_UINT64(1U, (uint64_t)ht_size(p_table));

    ht_destroy(&p_table, NULL);
}

static void
test_collision_remove_head_and_middle(void)
{
    ht_t *p_table = ht_create(4U, constant_hash);
    int value_a = 10;
    int value_b = 20;
    int value_c = 30;
    void *p_removed = NULL;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "a", &value_a));
    TEST_ASSERT_TRUE(ht_put(p_table, "b", &value_b));
    TEST_ASSERT_TRUE(ht_put(p_table, "c", &value_c));

    p_removed = ht_remove(p_table, "c");
    TEST_ASSERT_EQUAL_PTR(&value_c, p_removed);
    TEST_ASSERT_FALSE(ht_contains(p_table, "c"));

    p_removed = ht_remove(p_table, "b");
    TEST_ASSERT_EQUAL_PTR(&value_b, p_removed);
    TEST_ASSERT_FALSE(ht_contains(p_table, "b"));

    TEST_ASSERT_TRUE(ht_contains(p_table, "a"));
    TEST_ASSERT_EQUAL_UINT64(1U, (uint64_t)ht_size(p_table));

    ht_destroy(&p_table, NULL);
}

static void
test_update_returns_old_value(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value_old = 100;
    int value_new = 200;
    void *p_old_value = NULL;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "k", &value_old));

    p_old_value = ht_update(p_table, "k", &value_new);
    TEST_ASSERT_EQUAL_PTR(&value_old, p_old_value);
    TEST_ASSERT_EQUAL_PTR(&value_new, ht_get(p_table, "k"));

    TEST_ASSERT_NULL(ht_update(p_table, "missing", &value_old));

    ht_destroy(&p_table, NULL);
}

static void
count_iter(void *p_value)
{
    if (NULL != p_value)
    {
        g_iter_count++;
    }
}

static void
test_iter_values_visits_each_entry(void)
{
    ht_t *p_table = ht_create(8U, NULL);
    int value_a = 3;
    int value_b = 6;
    int value_c = 9;

    TEST_ASSERT_NOT_NULL(p_table);
    TEST_ASSERT_TRUE(ht_put(p_table, "i1", &value_a));
    TEST_ASSERT_TRUE(ht_put(p_table, "i2", &value_b));
    TEST_ASSERT_TRUE(ht_put(p_table, "i3", &value_c));

    g_iter_count = 0;
    ht_iter_values(p_table, count_iter);

    TEST_ASSERT_EQUAL_INT(3, g_iter_count);

    ht_destroy(&p_table, NULL);
}

void
ht_tests(void)
{
    RUN_TEST(test_create_valid);
    RUN_TEST(test_create_invalid_size);
    RUN_TEST(test_destroy_invalid_params);
    RUN_TEST(test_put_invalid_params);
    RUN_TEST(test_get_invalid_params);
    RUN_TEST(test_remove_invalid_params);
    RUN_TEST(test_update_invalid_params);
    RUN_TEST(test_contains_invalid_params);
    RUN_TEST(test_size_invalid_params);
    RUN_TEST(test_iter_values_invalid_params);
    RUN_TEST(test_print_keys_invalid_params);
    RUN_TEST(test_put_get_contains_and_size);
    RUN_TEST(test_put_duplicate_key_rejected);
    RUN_TEST(test_collision_remove_head_and_middle);
    RUN_TEST(test_update_returns_old_value);
    RUN_TEST(test_iter_values_visits_each_entry);
}

int
main(void)
{
    UNITY_BEGIN();

    printf("||================ HASHTABLE TESTS ====================||\n");

    ht_tests();

    return UNITY_END();
}
