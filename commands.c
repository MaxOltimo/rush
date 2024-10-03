#include "commands.h"

void ErrorHandler(int eFlag){

    if(eFlag == 1){//1 passed as argument for when shell is invoked with arguments
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
    }
    else if  (eFlag == 0){ //Normal error handling will continue running
    char error_message[30] = "AN Error has occurred 0\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    return;

    }

}

void EnterShell(void){
    char *buffer = NULL;
    size_t bufferSize = 0;
    int nread;

    //do
   //{
        printf("rush> ");
        nread = getline(&buffer, &bufferSize, stdin);

        
        if (nread == -1) {
        // Handle input error or EOF
        ErrorHandler(0);
        //continue;
        }
        

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
        else{
            ExecuteCommand(buffer);
         }

    //} while (1);

    free(buffer);

}

void ExecuteCommand(char *command) {

    char **args = setupCommands(command);
    //changeDirectory(args);

    int rc = fork();
    if (rc < 0) { // fork failed
        perror("fork failed");
        ErrorHandler(0);
    } else if (rc == 0) { // child process
        execv(args[0], args);
        perror("execv failed");
        exit(1); // Terminate the child process with a non-zero status
    } else {
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

char *trimWhitespace(char *str){
    char *end;

    while (isspace((char unsigned)*str)) str++;

    if(*str ==0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((char unsigned)*end)) end--;

    *(end + 1) = '\0';

    return str;

}

char **setupCommands(char *command) {

    int numArgs = CountArgs(command); // Counts arguments in command to properly allocate size for args array

    char **args = malloc((numArgs + 1) * sizeof(char *));
    if (args == NULL) {
        return NULL;
    }

    int i = 1;
    char *token;
    int setupFlag = 0;
    int firstCommandLength = 0;

    for (char *ptr = command; *ptr != ' ' && *ptr != '\0'; ptr++) {
        firstCommandLength++;
    }

    args[0] = malloc(strlen("/bin/") + firstCommandLength + 1); // allocate memory for first command to be concatenated with /bin/
    if (args[0] == NULL) {
        free(args);
        return NULL;
    }

    strcpy(args[0], "/bin/");
    strncat(args[0], command, firstCommandLength);

    if (access(args[0], X_OK) == 0) {
        while ((token = strsep(&command, " ")) != NULL) {
            if (*token != '\0' && setupFlag == 0) { // Initial concatenation of first command with /bin/
                setupFlag++;
            } else {
                args[i] = token;
                i++;
            }
        }
        args[i] = NULL; // final element must be NULL terminated for execv() to work
    } else {
        size_t size = strlen("/usr/bin/") + firstCommandLength + 1;

        args[0] = realloc(args[0], size);
        if (args[0] == NULL) {
            free(args);
            return NULL;
        }

        strcpy(args[0], "/usr/bin/");
        strncat(args[0], command, firstCommandLength);

        while ((token = strsep(&command, " ")) != NULL) {
            if (*token != '\0' && setupFlag == 0) { // Initial concatenation of first command with /usr/bin/
                setupFlag++;
            } else {
                args[i] = token;
                i++;
            }
        }
        args[i] = NULL; // final element must be NULL terminated for execv() to work
    }

    return args;
}

void changeDirectory(char **args) {
    if ((strcmp(args[0], "/usr/bin/cd") == 0 || strcmp(args[0], "/bin/cd") == 0) && args[2] == NULL) {
        if (args[1] == NULL) {
            printf("No directory specified\n");
            ErrorHandler(0);
        } else {
            if (chdir(args[1]) != 0) {
                perror("chdir failed");
                ErrorHandler(0);
            }
        }
    } else if ((strcmp(args[0], "/usr/bin/cd") == 0 || strcmp(args[0], "/bin/cd") == 0) && args[2] != NULL) {
        printf("Too many arguments for cd\n");
        ErrorHandler(0);
    }
}

void exitCommand(char *command) {
    command = trimWhitespace(command); // Removes leading and trailing whitespaces

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
            args[i] = token;
            i++;
        }
    }
    args[i] = NULL; // Null-terminate the array

    if ((strcmp(args[0], "exit") == 0) && args[1] == NULL) {
        free(args);
        exit(0);
    }

    printf("Invalid usage of exit\n");
    ErrorHandler(0);

    free(args);
}

















