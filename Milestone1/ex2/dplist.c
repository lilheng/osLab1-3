/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"

#include <string.h>

/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
  return list;
}

void dpl_free(dplist_t **list) {

    //TODO: add your code here
    if((*list) == NULL) {
        return;
    }
    dplist_node_t *current = (*list)->head; // head pointer is now assigned to local variable current which points to the first node in the list
    dplist_node_t *next_node; //local variable to store the node while traversing through the list
    while (current != NULL) {
        next_node = current -> next; // stores next node
        free(current);  // frees the current node
        current = next_node; //set current node to the next node
    }
    free(*list);
    free(current->element);
    *list = NULL;
    //Do extensive testing with valgrind.
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {
    /** Removes the list node at index 'index' from the list.
     * - The list node itself should always be freed.
     * - If 'index' is 0 or negative, the first list node is removed.
     * - If 'index' is bigger than the number of elements in the list, the last list node is removed.
     * - If the list is empty, return the unmodified list.
     * - If 'list' is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position at which the node should be removed from the list
     * \return a pointer to the list or NULL
     */
    //TODO: add your code here
    if(list == NULL){
        return NULL;
    }
    if(list->head == NULL){
        return list;
    }
    dplist_node_t *node_to_remove = dpl_get_reference_at_index(list, index);
    if(index <= 0){
        list->head = node_to_remove->next;
        node_to_remove->next->prev = NULL;
    }
    else if(index >= dpl_size(list)-1){
        node_to_remove->prev->next = NULL;
    }
    else{
        node_to_remove->next->prev = node_to_remove->prev;
        node_to_remove->prev->next = node_to_remove->next;
    }
    free(node_to_remove);
    free(node_to_remove -> element);
    return list;
    //To remove the node B, it is needed to remove the pointer that points to  the previous node
    // and the pointer that points to the next node, removing two nodes essentially.
    //Check if B has a previous node:
    //If yes, update the next pointer of the previous node (A) to point to the next node (C). Removing node B

        //This assignment statement sets the next pointer of the previous node to point to the node after the current node.
        //Which removes the current node, it updates the prev node to the next node and pass by the current node

    //Ceck if B has a next node:
    //If yes, update the prev pointer of the next node (C) to point to the previous node (A). Removing node B

        //This assignment statement sets the next pointer of the previous node to point to the node after the current node.
        //Which removes the current node, it updates the next node to the next node and pass by the current node

}

int dpl_size(dplist_t *list) {
    //TODO: add your code here
    int size = 1; //taking head already into account
    if (list == NULL) {
        return -1;
    }
    if( list -> head == NULL) {
        return 0;
    }
    dplist_node_t *current_node = list -> head; // current points to the head
    while (current_node -> next != NULL) {
        size ++;
        current_node = current_node -> next;
    }
    return size;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    /** Returns a reference to the list node with index 'index' in the list.
     * - If 'index' is 0 or negative, a reference to the first list node is returned.
     * - If 'index' is bigger than the number of list nodes in the list, a reference to the last list node is returned.
     * - If the list is empty, NULL is returned.
     * - If 'list' is is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position of the node for which the reference is returned
     * \return a pointer to the list node at the given index or NULL
     */
    if (list == NULL || list->head == NULL) return NULL;

    dplist_node_t *current = list->head;
    int count = 0;

    while (current->next != NULL && count < index) {
        current = current->next;
        count++;
    }
    return current;

}

element_t dpl_get_element_at_index(dplist_t *list, int index) {
    /** Returns the list element contained in the list node with index 'index' in the list.
     * - return is not returning a copy of the element with index 'index', i.e. 'element_copy()' is not used.
     * - If 'index' is 0 or negative, the element of the first list node is returned.
     * - If 'index' is bigger than the number of elements in the list, the element of the last list node is returned.
     * - If the list is empty, NULL is returned.
     * - If 'list' is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position of the node for which the element is returned
     * \return the element at the given index
     */

    //TODO: add your code here
    dplist_node_t *current_node = dpl_get_reference_at_index(list, index); //dereference to get the value at the node pointer memory location
    if(current_node == NULL) {
        return '\0';
    }
    return current_node -> element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {
    /** Returns an index to the first list node in the list containing 'element'.
     * - the first list node has index 0.
     * - If 'element' is not found in the list, -1 is returned.
     * - If 'list' is NULL, -1 is returned.
     * \param list a pointer to the list
     * \param element the element to look for
     * \return the index of the element that matches 'element'
     */

    //TODO: add your code here
    if(list == NULL || list -> head == NULL) {
        return -1;
    }
    dplist_node_t *current_node = list -> head;
    for(int count = 0; count < dpl_size(list); count++) {
        if(!strcmp(current_node -> element , element) ) {
            return count;
        }
        current_node = current_node -> next;
    }
    return -1;
}



