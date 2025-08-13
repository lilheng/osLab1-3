/**
* \author Heng
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "sbuffer.h"

// A basic node for the buffer
typedef struct sbuffer_node {
    struct sbuffer_node *next;
    sensor_data_t data;
} sbuffer_node_t;

// The buffer structure
struct sbuffer {
    sbuffer_node_t *head;
    sbuffer_node_t *tail;
    pthread_mutex_t lock;       // A single mutex to protect the entire buffer
    pthread_cond_t not_empty;  // A condition variable to signal when the buffer has data
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;

    // Initialize the single mutex and condition variable
    if (pthread_mutex_init(&((*buffer)->lock), NULL) != 0) {
        free(*buffer);
        return SBUFFER_FAILURE;
    }
    if (pthread_cond_init(&((*buffer)->not_empty), NULL) != 0) {
        pthread_mutex_destroy(&((*buffer)->lock));
        free(*buffer);
        return SBUFFER_FAILURE;
    }

    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }

    // Clean up the mutex and condition variable
    pthread_mutex_destroy(&((*buffer)->lock));
    pthread_cond_destroy(&((*buffer)->not_empty));

    // Free all nodes in the buffer
    sbuffer_node_t *current = (*buffer)->head;
    while (current) {
        sbuffer_node_t *dummy = current;
        current = current->next;
        free(dummy);
    }

    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    if (buffer == NULL) return SBUFFER_FAILURE;

    sbuffer_node_t *dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;

    dummy->data = *data;
    dummy->next = NULL;

    // Lock the mutex to safely modify the buffer
    pthread_mutex_lock(&(buffer->lock));

    if (buffer->tail == NULL) { // Buffer is empty
        buffer->head = buffer->tail = dummy;
    } else { // Buffer is not empty
        buffer->tail->next = dummy;
        buffer->tail = dummy;
    }

    // Signal any waiting consumer threads that the buffer is no longer empty.
    // Use broadcast in case multiple consumers are waiting.
    pthread_cond_broadcast(&(buffer->not_empty));

    // Unlock the mutex
    pthread_mutex_unlock(&(buffer->lock));

    return SBUFFER_SUCCESS;
}

// A generic 'remove' function for any consumer (datamgr or sensor_db)
int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    if (buffer == NULL) return SBUFFER_FAILURE;

    // Lock the mutex to safely access the buffer
    pthread_mutex_lock(&(buffer->lock));

    // Wait while the buffer is empty
    while (buffer->head == NULL) {
        pthread_cond_wait(&(buffer->not_empty), &(buffer->lock));
    }

    // If we are here, the buffer is not empty.
    // Copy the data from the head of the buffer
    *data = buffer->head->data;

    // Check if it's the end-of-stream signal.
    if (data->id == 0) {
        pthread_mutex_unlock(&(buffer->lock));
        return SBUFFER_NO_DATA; // Signal to terminate
    }

    // It's normal data, so remove the node from the head
    sbuffer_node_t *dummy = buffer->head;
    if (buffer->head == buffer->tail) { // Only one node
        buffer->head = buffer->tail = NULL;
    } else {
        buffer->head = buffer->head->next;
    }

    pthread_mutex_unlock(&(buffer->lock));

    free(dummy);
    return SBUFFER_SUCCESS;
}