/**
* \author Heng
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>

#include "config.h"
#include "connmgr.h"
#include "lib/tcpsock.h"
#include "sbuffer.h" // You need to include sbuffer.h to use sbuffer_t and sbuffer_insert

typedef struct {
    tcpsock_t *client;
    sbuffer_t *buffer;
} connection_handler_args_t;

void *connectionHandler(void *vargp);

void *init_connection_manager(void *vargp) {
    // Reading out the provided variables
    conn_mgr_data_t *conn_mgr_data = (conn_mgr_data_t *)vargp;
    int MAX_CONN = conn_mgr_data->max_conn;
    int PORT = conn_mgr_data->port;
    sbuffer_t *buffer = conn_mgr_data->buffer; // Use a local variable

    // init variables
    pthread_t tid[MAX_CONN];
    tcpsock_t *server;
    int conn_counter = 0;

    // Open tcp connection
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) {
        send_log_message("ERROR: Unable to open passive TCP socket");
        exit(EXIT_FAILURE);
    }
    send_log_message("Connection manager is listening for new connections.");

    // Assign a tcp connection to a thread
    do {
        // STEP 2: Create the argument struct for each thread
        connection_handler_args_t *args = malloc(sizeof(connection_handler_args_t));
        if (!args) {
            perror("Couldn't allocate memory for thread args");
            continue; // Skip this connection if malloc fails
        }

        args->buffer = buffer; // Pass the buffer pointer

        if (tcp_wait_for_connection(server, &(args->client)) != TCP_NO_ERROR) {
            free(args); // Clean up on error
            continue;
        }

        // Pass the struct to the new thread
        pthread_create(&tid[conn_counter], NULL, connectionHandler, args);
        conn_counter++;
    } while (conn_counter < MAX_CONN);

    // Wait until all threads have finished
    for (int i = 0; i < MAX_CONN; i++) {
        pthread_join(tid[i], NULL);
    }

    // Close the server socket
    if (tcp_close(&server) != TCP_NO_ERROR) {
        send_log_message("ERROR: Unable to close server socket");
        exit(EXIT_FAILURE);
    }
    send_log_message("Connection manager has served max clients and is shutting down.");

    // Init end-of-stream node to signal consumers to stop
    sensor_data_t end_data;
    end_data.id = 0; // Use a special ID to signify end of data

    // Insert the end-of-stream node into the buffer
    sbuffer_insert(buffer, &end_data);
    sbuffer_insert(buffer, &end_data);

    pthread_exit(NULL);
}


void *connectionHandler(void *vargp) {
    // STEP 3: Receive the struct and unpack the arguments
    connection_handler_args_t *args = (connection_handler_args_t *)vargp;
    tcpsock_t *client = args->client;
    sbuffer_t *buffer = args->buffer;

    int bytes, result;
    sensor_data_t data;
    bool first_data = true;

    // Read out tcp data
    do {
        // Read sensor ID and check for timeout
        bytes = sizeof(data.id);
        result = tcp_receive_timeout(client, (void *) &data.id, &bytes, TIMEOUT);
        if (result != TCP_NO_ERROR) break;

        // Read temperature and check for timeout
        bytes = sizeof(data.value);
        result = tcp_receive_timeout(client, (void *) &data.value, &bytes, TIMEOUT);
        if (result != TCP_NO_ERROR) break;

        // Read timestamp and check for timeout
        bytes = sizeof(data.ts);
        result = tcp_receive_timeout(client, (void *) &data.ts, &bytes, TIMEOUT);
        if (result != TCP_NO_ERROR) break;

        if (first_data) {
            first_data = false;
            char log_msg[LOG_MESSAGE_SIZE];
            sprintf(log_msg, "Sensor node %" PRIu16 " has opened a new connection", data.id);
            // STEP 4: Use the thread-safe logging function
            send_log_message(log_msg);
        }

        // Insert tcp data into the buffer
        sbuffer_insert(buffer, &data);

    } while (1); // The loop will be broken by tcp_receive errors

    // Log message based on why the loop exited
    char log_msg[LOG_MESSAGE_SIZE];
    if (result == TCP_CONNECTION_CLOSED) {
        sprintf(log_msg, "Sensor node %" PRIu16 " has closed the connection", data.id);
    } else {
        sprintf(log_msg, "Connection to sensor node %" PRIu16 " lost (error %d)", data.id, result);
    }
    send_log_message(log_msg);

    // Clean up
    tcp_close(&client);
    free(args); // Free the memory allocated for the arguments
    pthread_exit(NULL);
}