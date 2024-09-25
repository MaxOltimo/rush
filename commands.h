#ifndef COMMANDS_H
#define COMMANDS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


void ErrorHandler(int ErrorFlag);

void EnterShell(void);

void ExecuteCommand(char *command);






#endif