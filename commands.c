#include "commands.h"

void ErrorHandler(int eFlag){

    if(eFlag == 1){//1 passed as argument for when shell is invoked with arguments
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
    }
    else if  (eFlag == 0){ //Normal error handling will continue running
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    return;

    }

}

void EnterShell(void){
    char *buffer = NULL;
    size_t bufferSize = 0;
    int nread;

    do
    {
        printf("rush> ");
        nread = getline(&buffer, &bufferSize, stdin);

        if (nread == -1) {
        // Handle input error or EOF
        ErrorHandler(0);
        continue;
        }

        buffer[strcspn(buffer, "\n")] = 0; // remove "\n" character from input

         // Process the command
        if (strcmp(buffer, "exit") == 0) {
            exit(0);
        }

        else{
            ExecuteCommand(buffer);
        }

    } while (1);

    free(buffer);

}

void ExecuteCommand(char *command){







}