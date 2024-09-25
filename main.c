#include "commands.h"

int main(int argc, char *argv[]){


    int ErrorFlag = 0;

    if(argc > 1){
        ErrorFlag = -1;
        ErrorHandler(ErrorFlag);

    }

    EnterShell();
    
    return 0;
}