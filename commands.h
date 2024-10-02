#ifndef COMMANDS_H
#define COMMANDS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


void ErrorHandler(int ErrorFlag);

void EnterShell(void);

void ExecuteCommand(char *command);

int CountArgs(char *command);

char *trimWhitespace(char *str);

char **setupCommands(char *command);

void changeDirectory(char **args);

void exitCommand(char *command);


#endif