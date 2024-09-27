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

    char **args = setupCommands(command); 

    // Check if the command is 'cd'
    if (strcmp(args[0], "/user/bin/cd") == 0 || strcmp(args[0], "/bin/cd") == 0) {
        changeDirectory(args);
        return;
    }

    int rc = fork();
    if (rc < 0){ //fork failed
        ErrorHandler(0);
    }
    else if(rc == 0){
        execv(args[0], args);
        //printf("child proccess might need work\n");
        exit(1);
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

char **setupCommands(char *command){

    command = trimWhitespace(command);//Removes leading and trailing whitespaces
    int numArgs =  CountArgs(command);//Counts arguments in command to properly allocate size for args array

    char **args = malloc((numArgs + 1) * sizeof(char *));
    if (args == NULL) {
        return NULL;
    }

    int i = 1;
    char *token;
    int setupFlag = 0;
    int firstCommandLength = 0; /*ls, mkdir, sleep | This variable tracks the length of the initial command passed to rush so that it can be 
    concatenated to /bin/
    */

    for (char *ptr = command; *ptr != ' ' && *ptr != '\0'; ptr++) {
        firstCommandLength++;
    }

    args[0] = malloc(strlen("/bin/") + firstCommandLength + 1);//allocate memory for first command to be concatenated with /bin/
    if (args[0] == NULL) {
        free(args);
        return NULL;
    }

    strcpy(args[0], "/bin/");

    if(access(args[0], X_OK) == 0){

    while((token = strsep(&command, " ")) != NULL){
        if(*token != '\0' && setupFlag == 0){//Initial concatanation of first command with /bin/
            strcat(args[0], token);
            setupFlag++;
        }
        else{
            args[i] = token;
            //printf("%s\n", args[i]);
            i++;
        }
            args[i] = NULL;//final element must be NULL terminated for execv() to work
        }
    }

    else{
    
        size_t size = strlen("user/bin/") + firstCommandLength + 1;

        args[0] = realloc(args[0], size);
        if (args[0] == NULL) {
            free(args);
            return NULL;
        }

        strcpy(args[0], "user/bin/");

        while((token = strsep(&command, " ")) != NULL){
            if(*token != '\0' && setupFlag == 0){//Initial concatanation of first command with /bin/
                strcat(args[0], token);
                setupFlag++;
            }
            else{
                args[i] = token;
                //printf("%s\n", args[i]);
                i++;
            }
                args[i] = NULL;//final element must be NULL terminated for execv() to work
            }

    }

    return args;

}

void changeDirectory(char **arguments){

    if (arguments[1] == NULL) {
            ErrorHandler(0);
    } 
    else {
        if (chdir(arguments[1]) != 0) {
                ErrorHandler(0);
            }
        }
    return; // Return early since 'cd' is handled

}














