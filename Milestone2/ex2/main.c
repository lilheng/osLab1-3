//
// Created by heng on 11/16/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main() {
    int fd[2];//fd[0] for reading and fd[1] for write in the pipe.
    int returnstatus;
    int pid;
    char *writemessage = "Hi There MY name IS heNG";
    char readmessage[30];

    returnstatus = pipe(fd);

   if(returnstatus == -1){
       printf("Unable to create pipe\n");
       return 1;
   }

   pid = fork(); // fork is used to create a child process
   //Child Process
    if (pid == 0) {
        read(fd[0], readmessage, sizeof(readmessage));

        for (int count = 0; count < strlen(readmessage); count++) {
            if(islower(readmessage[count])){
                readmessage[count] = (signed char)toupper(readmessage[count]);
            }
            else{
                readmessage[count] = (signed char)tolower(readmessage[count]);
            }
        }
        printf("Child Process - Reading from pipe – Message 1 is %s\n", readmessage);
    }
    //Parent Process
    else {
      printf("Parent Process - Writing to pipe - Message 1 is %s\n", writemessage);
      write(fd[1], writemessage, strlen(writemessage));
    }
    return 0;
 }