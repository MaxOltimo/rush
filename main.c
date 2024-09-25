#include "commands.h"

int main(int argc, char *argv[]){

    if(argc > 1){
        ErrorHandler(1);
    }

    EnterShell();
    
    return 0;
}