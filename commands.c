#include "commands.h"

char **searchPaths = NULL;
int numPaths = 0;

void ErrorHandler(int eFlag) {
    if (eFlag == 1) { // 1 passed as argument for when shell is invoked with arguments
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (eFlag == 0) { // Normal error handling will continue running
        char error_message[30] = "An Error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
}

void EnterShell(void) {
    char *buffer = NULL;
    size_t bufferSize = 0;

    printf("rush> ");
    getline(&buffer, &bufferSize, stdin);

    buffer[strcspn(buffer, "\n")] = 0; // remove "\n" character from input
    buffer = trimWhitespace(buffer); // Removes leading and trailing whitespaces

    if (strncmp(buffer, "exit", 4) == 0) {
        exitCommand(buffer);
    } else if (strncmp(buffer, "cd", 2) == 0) {
        char **args = setupCommands(buffer);
        changeDirectory(args);
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    } else if (strncmp(buffer, "path", 4) == 0) {
        pathCommand(buffer + 5);
        //printSearchPaths();
    } else {
        if (strchr(buffer, '&') != NULL) {
            char *command = strtok(buffer, "&");
            int numCommands = 0;

            while (command != NULL) {
                command = trimWhitespace(command); // Trim whitespace from each command
                if (command[0] != '\0') {
                    pid_t pid = fork();
                    if (pid == 0) {
                        // Child process
                        ExecuteCommand(command);
                        exit(0);
                    } else if (pid > 0) {
                        // Parent process
                        numCommands++;
                    } else {
                        // Fork failed
                        ErrorHandler(0);
                    }
                }
                command = strtok(NULL, "&");
            }

            // Wait for all child processes to complete
            for (int i = 0; i < numCommands; i++) {
                wait(NULL);
            }
        } else {
            if (buffer[0] == '\0') return;
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                ExecuteCommand(buffer);
                exit(0);
            } else if (pid > 0) {
                // Parent process
                wait(NULL);
            } else {
                // Fork failed
                ErrorHandler(0);
            }
        }
    }
}

void ExecuteCommand(char *command) {
    char **args = setupCommands(command);
    int outputRedirect = 0;
    char *outputFile = NULL;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (outputRedirect) {
                ErrorHandler(0);
                return;
            }
            outputRedirect = 1;
            outputFile = args[i + 1];
            if (outputFile == NULL || args[i + 2] != NULL) {
                ErrorHandler(0);
                return;
            }
            args[i] = NULL;
        } else if (strcmp(args[i], ">>") == 0) {
            ErrorHandler(0);
            return;
        }
    }

    if (outputRedirect) {
        int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    execv(args[0], args);
    // If execv returns, there was an error
    ErrorHandler(0);

    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

int CountArgs(char *command) {
    int count = 0;
    char *temp = strdup(command);
    char *token;

    while ((token = strsep(&temp, " ")) != NULL) {
        if (*token != '\0') {
            count++;
        }
    }

    free(temp);
    return count;
}

char *trimWhitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
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

    return args;
}

void changeDirectory(char **args) {
    if ((strcmp(args[0], "cd") == 0)) {
        if (args[1] == NULL) {
            ErrorHandler(0);
        } else {
            if (chdir(args[1]) != 0) {
                ErrorHandler(0);
            }
        }
    } else if ((strcmp(args[0], "cd") == 0) && args[2] != NULL) {
        ErrorHandler(0);
    }
}

void exitCommand(char *command) {
    char **args = setupCommands(command);

    if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            ErrorHandler(0);
            free(args);
            return;
        }
        free(args);
        exit(0);
    }
}

void pathCommand(char *command) {
    // Free the old search paths
    if (searchPaths != NULL) {
        for (int i = 0; i < numPaths; i++) {
            free(searchPaths[i]);
        }
        free(searchPaths);
    }

    // Check if the command is empty
    if (trimWhitespace(command)[0] == '\0') {
        searchPaths = NULL;
        numPaths = 0;
        return;
    }

    // Count the number of new paths
    numPaths = CountArgs(command);

    // Allocate memory for the new search paths
    searchPaths = malloc((numPaths + 1) * sizeof(char *));
    if (searchPaths == NULL) {
        //ErrorHandler(0);
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

/*
void printSearchPaths() {
    if (searchPaths == NULL || numPaths == 0) {
        printf("No search paths set.\n");
        return;
    }

    printf("Current search paths:\n");
    for (int i = 0; i < numPaths; i++) {
        printf("%s\n", searchPaths[i]);
    }
}
*/

void initializeSearchPaths(void) {
     // Initialize default search paths
    numPaths = 2;
    searchPaths = malloc(numPaths * sizeof(char *));
    if (searchPaths == NULL) {
        ErrorHandler(0);
        return;
    }
    searchPaths[0] = strdup("/bin");
    searchPaths[1] = strdup("/usr/bin");

}