#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define COMMANDS_SIZE 10

void seekCommand(char * buff);
void callCommand(int i, char * command);
void help();
void time();
void div0();
void invalidOp();
void eliminator();

enum Commands {
    HELP,
    TIME,
    REGSTATE,
    DIVZERO,
    INVALIDOP,
    ELIMINATOR,
    INCFONT,
    DECFONT,
    FONTSIZE,
    CLEAR

};



#endif