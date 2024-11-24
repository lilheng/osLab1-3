//
// Created by heng on 11/18/24.
//
#include <unistd.h>
#include <string.h>
#include "sensor_db.h"
#include "logger.h"

#define SIZE 1
#define READ_END 0
#define WRITE_END 1

int fd[2];

bool child_created = false;

#define OPEN 1
#define OPEN_ERROR 2
#define INSERT 3
#define INSERT_ERROR 4
#define CLOSE 5
#define CLOSE_ERROR 6
#define NO_MESSAGE 7
#define END 8
#define FILENAME_EMPTY 9
#define ALREADY_CHILD 10

int child_process() {
    pid_t pid;
    char msg = NO_MESSAGE;

    // create the pipe
    if (pipe(fd) == -1){
        printf("Pipe failed\n");
        return -1;
    }

    // fork the child
    pid = fork();

    if (pid < 0){ // fork error
        printf("fork failed\n");
        return -1;
    }
    if (pid > 0){ // parent process
        close(fd[READ_END]);
    }
    else{ // child process
        close(fd[WRITE_END]);

        create_log_process();

        //read(fd[READ_END], &msg, SIZE);
        while (msg != END){
            long length = read(fd[READ_END], &msg, SIZE);
            if (length <= 0){
                msg = NO_MESSAGE;
            }

            if (msg == INSERT){
                write_to_log_process("Data inserted.");
            }
            else if (msg == CLOSE){
                write_to_log_process("Data file closed.");
                msg = END;
            }
            else if (msg == OPEN){
                write_to_log_process("Data file opened.");
            }
            else if (msg == OPEN_ERROR){
                write_to_log_process("Error while opening file.");
            }
            else if (msg == FILENAME_EMPTY){
                write_to_log_process("No filename.");
            }
            else if (msg == FILENAME_EMPTY){
                write_to_log_process("Already child running.");
            }
        }
        close(fd[READ_END]);
        end_log_process();
    }

    return 0;
}



FILE * open_db(char * filename, bool append){

    if (!child_created){
        if(child_process() != -1){
            child_created = true;
        } else{
            return NULL;
        }
    } else{
        char message = ALREADY_CHILD;
        write(fd[WRITE_END], &message, SIZE);
        message = END;
        write(fd[WRITE_END], &message, SIZE);
        return NULL;
    }

    if (filename == NULL){
        if (child_created){
            char message = FILENAME_EMPTY;
            write(fd[WRITE_END], &message, SIZE);
            message = END;
            write(fd[WRITE_END], &message, SIZE);
        } else{
            printf("Error: No filename.");
        }
        return NULL;
    }

    FILE *file = NULL;
    if (append){
        file = fopen(filename, "a");
    }
    else{
        file = fopen(filename, "w");
    }

    if(file != NULL){
        char message = OPEN;
        write(fd[WRITE_END], &message, SIZE);
    } else{
        char message = OPEN_ERROR;
        write(fd[WRITE_END], &message, SIZE);
        message = END;
        write(fd[WRITE_END], &message, SIZE);
    }
    return file;
}



int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    char message = INSERT;
    if (f == NULL){
        return -1;
    }

    int code = fprintf(f, "%hu, %f, %ld\n", id, value, ts);
    write(fd[WRITE_END], &message, SIZE);
    return code;
}

int close_db(FILE * f){
    char message = CLOSE;
    if (f == NULL){
        return -1;
    }

    int code = fclose(f);
    write(fd[WRITE_END], &message, SIZE);
    close(fd[WRITE_END]);
    return code;
}