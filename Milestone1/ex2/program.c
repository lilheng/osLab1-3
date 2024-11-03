#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"

// void ck_assert_msg(bool result, char * msg){
//     if(!result) printf("%s\n", msg);
// }
//void dpl_size_test(void){
//
//    ck_assert_msg(dpl_size(NULL) == -1, "List is NULL.");
//
//    dplist_t *numbers = NULL;
//    numbers = dpl_create();
//
//    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
//    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");
//
//    dpl_insert_at_index(numbers, '\e', -1);
//    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");
//
//    dpl_insert_at_index(numbers, 'b', 100);
//    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");
//
//    dpl_free(&numbers);
//}
//
//void dpl_get_reference_at_index_test(void){
//
//    ck_assert_msg(dpl_get_reference_at_index(NULL,0) == NULL, "List should be NULL.");
//
//    dplist_t *numbers = NULL;
//    numbers = dpl_create();
//
//    ck_assert_msg(dpl_get_reference_at_index(numbers,0) == NULL, "List should be empty.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//
//    /*
//    ck_assert_msg(dpl_get_reference_at_index(numbers,0) == numbers->head, "Should return pointer in head of list.");
//    ck_assert_msg(dpl_get_reference_at_index(numbers,1) == numbers->head, "Should return pointer in head of list.");
//    ck_assert_msg(dpl_get_reference_at_index(numbers,-1) == numbers->head, "Should return pointer in head of list.");
//    */
//
//    dpl_free(&numbers);
//}
//
//void dpl_get_element_at_index_test(void){
//
//    ck_assert_msg(dpl_get_element_at_index(NULL,0) == 0, "Element should be zero.");
//
//    dplist_t *numbers = NULL;
//    numbers = dpl_create();
//
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 0, "Element should be zero.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 'a', "Element should be 'a'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,-150) == 'a', "Element should be 'a'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,10) == 'a', "Element should be 'a'.");
//
//    dpl_insert_at_index(numbers, 'b', 1);
//    ck_assert_msg(dpl_get_element_at_index(numbers,1) == 'b', "Element should be 'b'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 'a', "Element should be 'a'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,10) == 'b', "Element should be 'b'.");
//
//    dpl_insert_at_index(numbers, '\e', 0);
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == '\e', "Element should be '\e'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,-150) == '\e', "Element should be '\e'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,1) == 'a', "Element should be 'a'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,2) == 'b', "Element should be 'b'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,10) == 'b', "Element should be 'b'.");
//
//    dpl_free(&numbers);
//}
//
//void dpl_get_index_of_element_test(void){
//
//    ck_assert_msg(dpl_get_index_of_element(NULL,0) == -1, "There is no list.");
//
//    dplist_t *numbers = NULL;
//    numbers = dpl_create();
//
//    ck_assert_msg(dpl_get_index_of_element(numbers,0) == -1, "There are no elements.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//    ck_assert_msg(dpl_get_index_of_element(numbers,'a') == 0, "Index should be 0.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'x') == -1, "Element does not exist.");
//
//    dpl_insert_at_index(numbers, 'b', 1);
//    ck_assert_msg(dpl_get_index_of_element(numbers,'b') == 1, "Index should be 1.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'a') == 0, "Index should be 0.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'\e') == -1, "Element does not exist.");
//
//    dpl_insert_at_index(numbers, '\e', 0);
//    ck_assert_msg(dpl_get_index_of_element(numbers,'\e') == 0, "Index should be 0.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'a') == 1, "Index should be 1.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'b') == 2, "Index should be 2.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'x') == -1, "Element does not exist.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//    ck_assert_msg(dpl_get_index_of_element(numbers,'\e') == 1, "Index should be 1.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'a') == 0, "Index should be 0.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'b') == 3, "Index should be 3.");
//    ck_assert_msg(dpl_get_index_of_element(numbers,'x') == -1, "Element does not exist.");
//
//    dpl_free(&numbers);
//}
//
//void dpl_remove_at_index_test(void){
//
//    ck_assert_msg(dpl_remove_at_index(NULL,0) == NULL, "There is no list.");
//
//    dplist_t *numbers = NULL;
//    numbers = dpl_create();
//
//    ck_assert_msg(dpl_remove_at_index(numbers,0) == numbers, "There are no elements.");
//
//    dpl_insert_at_index(numbers, 'a', 0);
//    dpl_insert_at_index(numbers, 'b', 1);
//    dpl_insert_at_index(numbers, '1', 2);
//    dpl_insert_at_index(numbers, '\e', 3);
//    dpl_insert_at_index(numbers, 'c', 4);
//
//    dplist_t *list = dpl_remove_at_index(numbers,-10);
//    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 4.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 'b', "Element should be 'b'.");
//
//    list = dpl_remove_at_index(numbers,10);
//    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 3.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,2) == '\e', "Element should be '\e'.");
//
//    list = dpl_remove_at_index(numbers,1);
//    ck_assert_msg(dpl_size(list) == dpl_size(numbers), "Size should be 2.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 'b', "Element should be 'b'.");
//    ck_assert_msg(dpl_get_element_at_index(numbers,1) == '\e', "Element should be '\e'.");
//
//    dpl_free(&numbers);
//}


int main(void)
{
//    dpl_size_test();
//    dpl_get_reference_at_index_test();
//    dpl_get_element_at_index_test();
//    dpl_get_index_of_element_test();
//    dpl_remove_at_index_test();
    return 0;
}
