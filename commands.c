#include "commands.h"

void ErrorHandler(int eFlag){

    if(eFlag == -1){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
    }
    else if  (eFlag > 0){}

}

void EnterShell(void){
    int i = 0;
    do
    {
        printf("rush> ");
        i++;


    } while (i < 10);



}