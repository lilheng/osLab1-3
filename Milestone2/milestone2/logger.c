//
// Created by heng on 18//11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

FILE *file = NULL;
int sequence_number = 0;

int write_to_log_process(char *msg){
    time_t tm;
    time(&tm);
    char* time = ctime(&tm);
    time[strlen(time)-1] = '\0';
    fprintf(file, "%d - %s - %s\n", sequence_number, time, msg);
    sequence_number++;
    return 0;
}

int create_log_process(){
    file = fopen("gateway.log", "a");
    return 0;
}

int end_log_process(){
    return fclose(file);
}