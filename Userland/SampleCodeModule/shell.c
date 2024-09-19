#include <shell.h>
#include <userSyscalls.h>
#include <commands.h>
#include <buffer.h>
#include "colors.h"
#include "utils.h"


static char buff[BUFFERLIMIT] = {0};
static char prev_commands[COMMAND_SIZE][BUFFERLIMIT] = {{0}};
static int prev_num = 0;

void welcomeMessage(){
    call_paint_screen(BLACK);
    char welcome_message[] = "Welcome to imaginariOS.\nInput a command: \n(Enter HELP to see full list of commands)\n";
    for(int i = 0; welcome_message[i] != 0; i++){
        putCh(welcome_message[i]);
    }

}

void lineStart(){
    putCh('$');
    clearBuff(buff);
}


void bufferize(){
    
    int i = 0;
    int end_buff = 0;
    int flag = 0;
    int max_pos = 0;
    while (i < BUFFERLIMIT){
        char c = getCh();
        end_buff = (i == BUFFERLIMIT-1);
        if(c == '\b'){
            if(i > 0){
                if (buff[--i] == '\t') {
                    putCh('\b');
                }

                int j = i;
                while (buff[j] != 0) {
                    buff[j] = buff[j+1];
                    prev_commands[0][j] = prev_commands[0][j+1];
                    j++;
                }
                max_pos--;
                putCh(c);
            }
            flag = 1;
        }
        else if(c == '\n'){
            max_pos = 0;
            putCh(c);
            if( i == 0 && buff[i] == 0){
               clearBuff(buff);
                return;
            }
            if (prev_num == 0) {
                movePrevCommands();
            }

            prev_num = 0;

            readCommand(buff);
            return;
        } else if (c == 0x7C){
            if (i > 0) {
                if (buff[--i] == '\t') {
                    putCh(c);
                }
                putCh(c);
            }
            flag = 1;
        }
        else if(c == 0x7D) {
            if (i < BUFFERLIMIT && i < max_pos) {
                if (buff[++i] == '\t') {
                    putCh(c);
                }
                putCh(c);
            }
            flag = 1;
        }
        else if (c == 0x7F) {
            if (prev_num < COMMAND_SIZE-1 && prev_commands[prev_num+1][0] != 0) {
                delete(i);
                clearBuff(buff);
                i = 0;
                prev_num++;
                while(prev_commands[prev_num][i] != 0) {
                    buff[i] = prev_commands[prev_num][i];
                    putCh(buff[i]);
                    i++;
                }
            }
            flag = 0;
        }
        else if (c == 0x7B) {
            if (prev_num > 0) {
                delete(i);
                clearBuff(buff);
                i = 0;
                prev_num--;
                while(prev_commands[prev_num][i] != 0) {
                    buff[i] = prev_commands[prev_num][i];
                    putCh(buff[i]);
                    i++;
                }
            }
            flag = 0;
        } else if (c == 0x1B) {
            flag = 1;
        }
        else{
            if(!end_buff && c != 0 && max_pos < BUFFERLIMIT-1) {
                max_pos++;
                int z = BUFFERLIMIT-1;
                while(z != i) {
                    buff[z] = buff[z-1];
                    z--;
                }
                prev_commands[0][i] = c;
                buff[i++] = c;

            }else {
                flag = 1;
            }
        }
        if(!flag && c != 0) {
            putCh(c);
        }

        flag = 0;
    }


    return;


}

void movePrevCommands() {
    for (int i = COMMAND_SIZE-1; i > 0; i--) {
        for (int j = 0; j < BUFFERLIMIT; j++) {
            prev_commands[i][j] = prev_commands[i-1][j];
        }
    }

    for (int z = 0; z < BUFFERLIMIT; z++) {
        prev_commands[0][z] = 0;
    }

}

void readCommand(char * buff){
    seekCommand(buff);
    clearBuff(buff);

}


void delete(int i) {
    while (buff[i] != 0) {
        putCh(0x7D);
        i++;
    }
    int z = i;
    while (z > 0) {
        if (buff[z] == '\t') {
            putCh('\b');
        }
        putCh('\b');
        z--;
    }
}


int shellStart(){
    call_clear();
    welcomeMessage();
    while(1){
        lineStart();
        bufferize();
    }
    return 0;
}