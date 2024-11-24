//
// Created by heng on 11/16/24.
//

#include "sensor_db.h"

FILE * open_db(char * filename, bool append){
    FILE *file = NULL;
    if (append) {
        file = fopen(filename,"a");
    }
    else {
        file = fopen(filename,"w");
    }
    return file;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (fprintf(f,"%d,%f,%ld\n", id, value, ts) <0 ) {
        perror("Failed to write to file");
        return -1;
    }
    return 0;
}

int close_db(FILE * f) {
    return fclose(f);
}
