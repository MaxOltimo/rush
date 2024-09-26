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

   int numArgs =  CountArgs(command);

    char *args[numArgs + 1];
    int i = 0;
    char *token;
    while((token = strsep(&command, " ")) != NULL){
        if(*token != '\0'){
            args[i] = token;
            printf("%s\n", args[i]);
            i++;
        }
    }
    args[i] = NULL;

    int rc = fork();
    if (rc < 0){ //fork failed
        ErrorHandler(0);
    }
    else if(rc == 0){
        execv(args[0], args);
        printf("child proccess might need work");
        ErrorHandler(0);
    }
    else{
        int wc = wait(NULL);
    }


}


int CountArgs(char *command) {
    int count = 0;
    char *temp = strdup(command); // Duplicate the command string to avoid modifying the original
    char *token;

    // Tokenize the command string and count non-empty tokens
    while ((token = strsep(&temp, " ")) != NULL) {
        if (*token != '\0') { // Skip empty tokens
            count++;
        }
    }

    free(temp); // Free the duplicated string
    return count;
}