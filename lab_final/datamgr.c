/**
* \author Heng
 */

#include "datamgr.h"
#include "lib/dplist.h"
#include <unistd.h>
#include <inttypes.h>

typedef struct {
    uint16_t sensorID;
    uint16_t roomID;
    double avg[RUN_AVG_LENGTH];
    time_t last_modified;
    int total_values;
} element_t;

int data_mgr_fd = -1;
char data_mgr_log_message[SIZE];
dplist_t *list;

void * element_copy(void * element) {
    element_t* copy = malloc(sizeof (element_t));
    ERROR_HANDLER(!copy, "Malloc of the element_t copy struct failed");
    //assert(copy != NULL);
    copy->roomID = ((element_t*)element)->roomID;
    copy->sensorID = ((element_t*)element)->sensorID;
    for (int count = 0; count < RUN_AVG_LENGTH; count++) {
        copy->avg[count] = ((element_t*)element)->avg[count];
    }
    copy->last_modified = ((element_t*)element)->last_modified;
    copy->total_values = ((element_t*)element)->total_values;
    return (void *) copy;
}

void element_free(void ** element) {
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return (((element_t*)x)->sensorID == ((element_t*)y)->sensorID ? 0 : 1);
}

void *init_data_manager(void *vargp){
    data_mgr_data_t *data = (data_mgr_data_t *)vargp;
    sbuffer_t *buffer = data->buffer;
    sensor_data_t *sensor_data = malloc(sizeof(sensor_data_t));

    element_t *element = (element_t*)malloc(sizeof(element_t));
    if (!element) {
        send_log_message("ERROR: Malloc of the element_t struct failed");
        free(sensor_data);
        return NULL;
    }

    // Use a local list variable, not a global one
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // Reading room_sensor.map
    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    if (fp_sensor_map == NULL) {
        send_log_message("ERROR: Could not open room_sensor.map");
        // Clean up all allocated memory before exiting
        free(element);
        free(sensor_data);
        dpl_free(&list, true);
        return NULL;
    }
    char line[255];
    while (fgets(line, 255, fp_sensor_map) != NULL){
        sscanf(line,"%hu %hu", &(element->roomID), &(element->sensorID));
        element->total_values = 0;
        dpl_insert_at_index(list, element, dpl_size(list), true);
    }
    fclose(fp_sensor_map);

    // Main loop to process data from the buffer
    while(sbuffer_remove(buffer, sensor_data) == SBUFFER_SUCCESS) {
        bool sensor_found = false;
        for (int el = 0; el < dpl_size(list); el++) {
            element_t *element_at_index = (element_t*)dpl_get_element_at_index(list, el);

            if (element_at_index->sensorID == sensor_data->id){
                sensor_found = true;

                // ADD THIS BLOCK TO UPDATE SENSOR DATA
                int index = element_at_index->total_values % RUN_AVG_LENGTH;
                element_at_index->avg[index] = sensor_data->value;
                element_at_index->total_values++;
                element_at_index->last_modified = sensor_data->ts;
                // END OF ADDED BLOCK

                char log_message[LOG_MESSAGE_SIZE];
                if (element_at_index->total_values >= RUN_AVG_LENGTH){
                    double avg = 0;
                    for (int count = 0; count < RUN_AVG_LENGTH; count++) {
                        avg += element_at_index->avg[count];
                    }
                    avg = avg / RUN_AVG_LENGTH;

                    if (avg > SET_MAX_TEMP){
                        sprintf(log_message, "Sensor node %" PRIu16 " reports it’s too hot (avg temp = %f)", sensor_data->id, avg);
                        send_log_message(log_message);
                    } else if (avg < SET_MIN_TEMP){
                        sprintf(log_message, "Sensor node %" PRIu16 " reports it’s too cold (avg temp = %f)", sensor_data->id, avg);
                        send_log_message(log_message);
                    }
                }
                break; // Exit the for-loop once sensor is found
            }
        }

        if (!sensor_found) {
            char log_message[LOG_MESSAGE_SIZE];
            sprintf(log_message, "Received sensor data with invalid sensor node ID %" PRIu16, sensor_data->id);
            send_log_message(log_message);
        }
    }

    // Free all allocated memory
    free(element);
    free(sensor_data);
    dpl_free(&list, true);

    return NULL;
}