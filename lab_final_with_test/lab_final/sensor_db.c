/**
* \author Heng
 */

#include <unistd.h>
#include <inttypes.h>
#include "sensor_db.h"

int storage_mgr_fd = -1;
char storage_mgr_log_message[SIZE];

int open_db(){
    //empty the csv file
    if(fclose(fopen("data.csv", "w")) != 0){
        return -1;
    }
    send_log_message("A new data.csv file has been created");
    return 0;
}



void *init_storage_manager(void *vargp){

    // Reading out the provided variables
    storage_mgr_data_t *data = (storage_mgr_data_t*)vargp;
    sbuffer_t *buffer = data->buffer;
    storage_mgr_fd = data->pipe_write_end;
    sensor_data_t *sensor_data = malloc(sizeof(sensor_data_t));

    // Start database
    open_db();

    // STEP 2 FIX: Open the file ONCE before the loop
    FILE *file = fopen("data.csv", "a");
    if (file == NULL) {
        // Handle error: log and exit thread
        send_log_message("ERROR: Could not open data.csv for appending");
        free(sensor_data);
        return NULL;
    }

    // STEP 1 FIX: Correct the loop condition
    while(sbuffer_remove(buffer, sensor_data) == SBUFFER_SUCCESS) {

        // Write to the already open file
        fprintf(file, "%hu, %f, %ld\n", sensor_data->id, sensor_data->value, sensor_data->ts);

        // Log that data is inserted
        sprintf(storage_mgr_log_message, "Data insertion from sensor %" PRIu16 " succeeded", sensor_data->id);
        write(storage_mgr_fd, storage_mgr_log_message, SIZE);
    }

    // STEP 2 FIX: Close the file ONCE after the loop
    fclose(file);

    // Stop the db and free all malloced data
    close_db();
    free(sensor_data);
    return NULL;
}
int close_db(){

    send_log_message("The data.csv file has been closed");

    return 0;
}