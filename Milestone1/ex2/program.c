#define _GNU_SOURCE

/**

 * \author Bert Lagaisse

 *

 * Main method that executes some test functions (without check framework)

 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dplist.h"

void ck_assert_msg(bool result, char *msg) {
    if (!result) printf("%s\n", msg);
}

void dpl_size_test(void) {
    ck_assert_msg(dpl_size(NULL) == -1, "List is NULL.");

    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");

    // Use string literals instead of characters
    dpl_insert_at_index(numbers, strdup("string1"), 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(numbers, strdup("string2"), -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(numbers, strdup("string3"), 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");

    dpl_free(&numbers);
}

void dpl_get_reference_at_index_test(void) {
    ck_assert_msg(dpl_get_reference_at_index(NULL, 0) == NULL, "List should be NULL.");

    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(dpl_get_reference_at_index(numbers, 0) == NULL, "List should be empty.");

    dpl_insert_at_index(numbers, strdup("string1"), 0);

    /*
    ck_assert_msg(dpl_get_reference_at_index(numbers,0) == numbers->head, "Should return pointer in head of list.");
    ck_assert_msg(dpl_get_reference_at_index(numbers,1) == numbers->head, "Should return pointer in head of list.");
    ck_assert_msg(dpl_get_reference_at_index(numbers,-1) == numbers->head, "Should return pointer in head of list.");
    */

    dpl_free(&numbers);
}

void dpl_get_element_at_index_test(void) {
    ck_assert_msg(dpl_get_element_at_index(NULL, 0) == NULL, "Element should be NULL.");

    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == NULL, "Element should be NULL.");

    dpl_insert_at_index(numbers, strdup("string1"), 0);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "string1") == 0, "Element should be 'string1'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, -150), "string1") == 0, "Element should be 'string1'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 10), "string1") == 0, "Element should be 'string1'.");

    dpl_insert_at_index(numbers, strdup("string2"), 1);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 1), "string2") == 0, "Element should be 'string2'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "string1") == 0, "Element should be 'string1'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 10), "string2") == 0, "Element should be 'string2'.");

    dpl_insert_at_index(numbers, strdup("string3"), 0);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "string3") == 0, "Element should be 'string3'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, -150), "string3") == 0, "Element should be 'string3'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 1), "string1") == 0, "Element should be 'string1'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2), "string2") == 0, "Element should be 'string2'.");

    dpl_free(&numbers);
}

void dpl_get_index_of_element_test(void) {
    ck_assert_msg(dpl_get_index_of_element(NULL, NULL) == -1, "There is no list.");

    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(dpl_get_index_of_element(numbers, NULL) == -1, "There are no elements.");

    dpl_insert_at_index(numbers, strdup("string1"), 0);
    ck_assert_msg(dpl_get_index_of_element(numbers, "string1") == 0, "Index should be 0.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "stringX") == -1, "Element does not exist.");

    dpl_insert_at_index(numbers, strdup("string2"), 1);
    ck_assert_msg(dpl_get_index_of_element(numbers, "string2") == 1, "Index should be 1.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string1") == 0, "Index should be 0.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "stringX") == -1, "Element does not exist.");

    dpl_insert_at_index(numbers, strdup("string3"), 0);
    ck_assert_msg(dpl_get_index_of_element(numbers, "string3") == 0, "Index should be 0.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string1") == 1, "Index should be 1.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string2") == 2, "Index should be 2.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "stringX") == -1, "Element does not exist.");

    dpl_insert_at_index(numbers, strdup("string1"), 0);
    ck_assert_msg(dpl_get_index_of_element(numbers, "string3") == 1, "Index should be 1.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string1") == 0, "Index should be 0.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string2") == 3, "Index should be 3.");
    ck_assert_msg(dpl_get_index_of_element(numbers, "stringX") == -1, "Element does not exist.");

    dpl_free(&numbers);
}

void dpl_remove_at_index_test(void) {
    ck_assert_msg(dpl_remove_at_index(NULL, 0) == NULL, "There is no list.");

    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(dpl_remove_at_index(numbers, 0) == numbers, "There are no elements.");

    dpl_insert_at_index(numbers, strdup("string1"), 0);
    dpl_insert_at_index(numbers, strdup("string2"), 1);
    dpl_insert_at_index(numbers, strdup("string3"), 2);
    dpl_insert_at_index(numbers, strdup("string4"), 3);
    dpl_insert_at_index(numbers, strdup("string5"), 4);

    dplist_t *list = dpl_remove_at_index(numbers, -10);
    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 5.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "string1") == 0, "Element should be 'string1'.");

    list = dpl_remove_at_index(numbers, 10);
    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 4.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3), "string5") == 0, "Element should be 'string5'.");

    list = dpl_remove_at_index(numbers, 1);
    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 3.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "string1") == 0, "Element should be 'string1'.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 1), "string3") == 0, "Element should be 'string3'.");

    dpl_free(&numbers);
}

int main(void) {
    dpl_size_test();
    dpl_get_reference_at_index_test();
    dpl_get_element_at_index_test();
    dpl_get_index_of_element_test();
    dpl_remove_at_index_test();
    return 0;
}
