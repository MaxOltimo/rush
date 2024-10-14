#include "commands.h"

void ErrorHandler(int eFlag){

    if(eFlag == 1){//1 passed as argument for when shell is invoked with arguments
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
    }
    else if  (eFlag == 0){ //Normal error handling will continue running
    char error_message[30] = "An Error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    return;

    }

}

void EnterShell(void){
    char *buffer = NULL;
    size_t bufferSize = 0;
    //int nread;

    //do
   //{
        printf("rush> ");
        //nread = 
        getline(&buffer, &bufferSize, stdin);

        /*
        if (nread == -1) {
        // Handle input error or EOF
        ErrorHandler(0);
        //continue;
        }*/
        

        buffer[strcspn(buffer, "\n")] = 0; // remove "\n" character from input

         // Process the command

        buffer = trimWhitespace(buffer); // Removes leading and trailing whitespaces
        
        if (strncmp(buffer, "exit", 4) == 0) {
            exitCommand(buffer);
            //break;
        }

        else if(strncmp(buffer, "cd", 2) == 0){
            char **args = setupCommands(buffer);
            changeDirectory(args);
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }

            free(args);
        }

        else if (strncmp(buffer, "path", 4) == 0) {
            pathCommand(buffer + 5);
        }
        else{
            if (strchr(buffer, '&') != NULL) {
                // Split the buffer into commands based on '&'
                char *command = strtok(buffer, "&");
                while (command != NULL) {
                    ExecuteCommand(command);
                    command = strtok(NULL, "&");
                }

                // Wait for all child processes
                while (wait(NULL) > 0);
            } else {
                // No '&' present, execute the command normally
                if(buffer[0] == '\0') return;
                ExecuteCommand(buffer);
                // Wait for the single child process to complete
                wait(NULL);
            }
        
         }

    //} while (1);

    //free(buffer);

}

void ExecuteCommand(char *command) {

    char **args = setupCommands(command);
    int outputRedirect = 0;
    char *outputFile = NULL;
    //changeDirectory(args);

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (outputRedirect) {
                // Multiple redirection operators detected
                ErrorHandler(0);
                return;
            }
            outputRedirect = 1;
            outputFile = args[i + 1];
            if (outputFile == NULL || args[i + 2] != NULL) {
                // No file specified or multiple arguments to the right of the redirection sign
                ErrorHandler(0);
                return;
            }
            args[i] = NULL;
        } else if (strcmp(args[i], ">>") == 0) {
            // ">>" is an error
            ErrorHandler(0);
            return;
        }
    }

    int rc = fork();
    if (rc < 0) { // fork failed
        //perror("fork failed");
        ErrorHandler(0);
    } else if (rc == 0) { // child process
        if (outputRedirect) {
            int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                //perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execv(args[0], args);
        //perror("execv failed");
        exit(1); // Terminate the child process with a non-zero status
    } //else {
      //  int wc = wait(NULL);
    //}
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);

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

char *trimWhitespace(char *str){
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if(*str ==0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    return str;

}

char **setupCommands(char *command) {
    command = trimWhitespace(command);

    int numArgs = CountArgs(command);

    char **args = malloc((numArgs + 1) * sizeof(char *));
    if (args == NULL) {
        return NULL;
    }

    int i = 0;
    char *token;
    while ((token = strsep(&command, " ")) != NULL) {
        if (*token != '\0') {
            args[i] = strdup(token);
            i++;
        }
    }
    args[i] = NULL;

    // Search for the executable in the search paths
    for (int j = 0; j < numPaths; j++) {
        char *path = malloc(strlen(searchPaths[j]) + strlen(args[0]) + 2);
        if (path == NULL) {
            ErrorHandler(0);
            return NULL;
        }
        strcpy(path, searchPaths[j]);
        strcat(path, "/");
        strcat(path, args[0]);

        if (access(path, X_OK) == 0) {
            free(args[0]);
            args[0] = path;
            return args;
        }
        free(path);
    }

    // If the executable is not found in the search paths, return the original args
    return args;
}

void changeDirectory(char **args) {
    if ((strcmp(args[0], "cd") == 0)) {
        if (args[1] == NULL) {
            //printf("No directory specified\n");
            ErrorHandler(0);
        } else {
            if (chdir(args[1]) != 0) {
                //perror("chdir failed");
                ErrorHandler(0);
            }
        }
    } else if ((strcmp(args[0], "cd") == 0) && args[2] != NULL) {
        //printf("Too many arguments for cd\n");
        ErrorHandler(0);
    }
}

void exitCommand(char *command) {
    //command = trimWhitespace(command); // Removes leading and trailing whitespaces

    int numArgs = CountArgs(command); // Counts arguments in command to properly allocate size for args array

    char **args = malloc((numArgs + 1) * sizeof(char *));
    if (args == NULL) {
        ErrorHandler(0);
        return;
    }

    int i = 0;
    char *token;
    while ((token = strsep(&command, " ")) != NULL) {
        if (*token != '\0') { // Skip empty tokens
            args[i] = strdup(token); // Duplicate the token
            if (args[i] == NULL) {
                ErrorHandler(0);
                return;
            }
            i++;
        }
    }
    args[i] = NULL; // Null-terminate the array

    if ((strcmp(args[0], "exit") == 0) && args[1] == NULL) {
        for (int j = 0; args[j] != NULL; j++) {
            free(args[j]); // Free each duplicated token
        }
        free(args); // Free the args array itself
        exit(0);
    }

    //printf("Invalid usage of exit\n");
    ErrorHandler(0);

    for (int j = 0; args[j] != NULL; j++) {
        free(args[j]); // Free each duplicated token
    }
    free(args); // Free the args array itself
}

void pathCommand(char *command) {
    // Free the old search paths
    if (searchPaths != NULL) {
        for (int i = 0; i < numPaths; i++) {
            free(searchPaths[i]);
        }
        free(searchPaths);
    }

    // Count the number of new paths
    numPaths = CountArgs(command);

    // Allocate memory for the new search paths
    searchPaths = malloc((numPaths + 1) * sizeof(char *));
    if (searchPaths == NULL) {
        ErrorHandler(0);
        return;
    }

    // Tokenize the command to extract the paths
    int i = 0;
    char *token;
    while ((token = strsep(&command, " ")) != NULL) {
        if (*token != '\0') {
            searchPaths[i] = strdup(token);
            if (searchPaths[i] == NULL) {
                ErrorHandler(0);
                return;
            }
            i++;
        }
    }
    searchPaths[i] = NULL; // Null-terminate the array
}














