#include "commands.h"

int main(int argc, char *argv[]){

    if(argc > 1){
        ErrorHandler(1);
    }

    initializeSearchPaths();

    do{
    EnterShell();
    }
    while(1);
    
    return 0;
}