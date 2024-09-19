#ifndef _SHELL_H_
#define _SHELL_H_

#define COMMAND_SIZE 10
#include "utils.h"

void welcomeMessage();
void lineStart();
void bufferize();
void readCommand(char * buff);
int shellStart();
void movePrevCommands();
void delete(int i);
#endif