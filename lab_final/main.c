/**
* \author Heng
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "connmgr.h"
#include "sensor_db.h"
#include "datamgr.h"
#include "sbuffer.h"

#define READ_END 0
#define WRITE_END 1
#define LOG_MSG_SIZE 256

sbuffer_t *buffer;
int fd[2];  // Pipe for log communication
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ---------------- LOG PROCESS ---------------- */

void log_process_loop(void) {
    FILE *file = fopen("gateway.log", "w");  // Create new empty log file
    if (!file) {
        perror("Error opening gateway.log");
        exit(EXIT_FAILURE);
    }

    int sequence_number = 0;
    char msg[LOG_MSG_SIZE];

    while (1) {
        ssize_t length = read(fd[READ_END], msg, sizeof(msg));
        if (length <= 0) {
            break; // EOF or error
        }

        msg[length] = '\0'; // Ensure null-terminated string

        if (strcmp(msg, "END") == 0) {
            break;
        }

        // Timestamp
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // remove newline

        fprintf(file, "%d %s %s\n", sequence_number++, time_str, msg);
        fflush(file);
    }

    fclose(file);
    exit(EXIT_SUCCESS);
}

int create_log_process(void) {
    if (pipe(fd) == -1) {
        perror("Pipe failed");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return -1;
    }

    if (pid == 0) {
        // Child process (log process)
        close(fd[WRITE_END]); // child only reads
        log_process_loop();
    } else {
        // Parent process
        close(fd[READ_END]);  // parent only writes
    }

    return 0;
}

void send_log_message(const char *msg) {
    pthread_mutex_lock(&log_mutex);
    write(fd[WRITE_END], msg, strlen(msg) + 1); // +1 for null terminator
    pthread_mutex_unlock(&log_mutex);
}

/* ---------------- MAIN ---------------- */

int main(int argc, char *argv[]) {
    pthread_t conn_mgr_thread_id, data_mgr_thread_id, store_mgr_thread_id;
    sbuffer_t *buffer = NULL; // Initialize pointers to NULL
    conn_mgr_data_t *conn_mgr_data = NULL;
    storage_mgr_data_t *storage_mgr_data = NULL;
    data_mgr_data_t *data_mgr_data = NULL;
    int result = 0; // Use a variable for the final return value

    if (argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients\n");
        result = -1;
        goto cleanup;
    }

    int MAX_CONN = atoi(argv[2]);
    int PORT = atoi(argv[1]);

    if (create_log_process() != 0) {
        fprintf(stderr, "Failed to create log process\n");
        result = -1;
        goto cleanup;
    }

    if (sbuffer_init(&buffer) == SBUFFER_FAILURE) {
        result = -1;
        goto cleanup;
    }

    // Prepare thread arguments
    conn_mgr_data = malloc(sizeof(conn_mgr_data_t));
    storage_mgr_data = malloc(sizeof(storage_mgr_data_t));
    data_mgr_data = malloc(sizeof(data_mgr_data_t));

    // Check if any malloc failed
    if (!conn_mgr_data || !storage_mgr_data || !data_mgr_data) {
        fprintf(stderr, "Failed to allocate memory for thread data\n");
        result = -1;
        goto cleanup;
    }

    conn_mgr_data->max_conn = MAX_CONN;
    conn_mgr_data->port = PORT;
    conn_mgr_data->pipe_write_end = fd[WRITE_END];
    conn_mgr_data->buffer = buffer;

    storage_mgr_data->buffer = buffer;
    storage_mgr_data->pipe_write_end = fd[WRITE_END];

    data_mgr_data->buffer = buffer;
    data_mgr_data->pipe_write_end = fd[WRITE_END];

    // Start threads
    pthread_create(&conn_mgr_thread_id, NULL, init_connection_manager, conn_mgr_data);
    pthread_create(&store_mgr_thread_id, NULL, init_storage_manager, storage_mgr_data);
    pthread_create(&data_mgr_thread_id, NULL, init_data_manager, data_mgr_data);

    // Wait for threads to finish
    pthread_join(conn_mgr_thread_id, NULL);
    pthread_join(store_mgr_thread_id, NULL);
    pthread_join(data_mgr_thread_id, NULL);

cleanup:
    // This cleanup block is now reached from every exit path
    printf("Main: Cleaning up resources...\n");

    // Tell log process to stop
    if (fd[WRITE_END] != 0) { // Check if pipe was created
        send_log_message("END");
        close(fd[WRITE_END]);
    }

    // Free allocated structs only if they were successfully allocated
    if (conn_mgr_data) free(conn_mgr_data);
    if (storage_mgr_data) free(storage_mgr_data);
    if (data_mgr_data) free(data_mgr_data);

    // Clean up buffer if it was initialized
    if (buffer) sbuffer_free(&buffer);

    return result;
}