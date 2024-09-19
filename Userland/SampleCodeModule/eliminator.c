#include "colors.h"
#include "utils.h"
#include "eliminator.h"
#include "userSyscalls.h"
#include <stdint.h>
#include <stdbool.h>
#include "sounds.h"

uint64_t DEFAULT_BCOLOR = BLACK;
uint64_t DEFAULT_FCOLOR = RED;

uint64_t DEFAULT_PLAYER1_COLOR = RED;
uint64_t DEFAULT_PLAYER2_COLOR = GREEN;

uint32_t MENU_X = 100;
uint32_t MENU_Y = 40;

uint32_t START_X = 100;
uint32_t START_Y = 20;


uint64_t player1Up = 0x11;
uint64_t player1Down = 0x1F;
uint64_t player1Left = 0x1E;
uint64_t player1Right = 0x20;

uint64_t player2Up = 0x17;
uint64_t player2Down = 0x25;
uint64_t player2Left = 0x24;
uint64_t player2Right = 0x26;

uint64_t QUIT = 0x10;
uint64_t SPACE = 0x39;
uint64_t ENTER = 0x1C;
uint64_t SPEEDKEYUP = 0x20;
uint64_t SPEEDKEYDOWN = 0x1F;
uint64_t PLAYERSKEY = 0x19;
uint64_t ESC = 0x01;
uint64_t ONEPLAYER = 0x02;
uint64_t TWOPLAYERS = 0x03;

uint32_t CONFIG_X;
uint32_t CONFIG_Y;
unsigned int players = DEFAULT_PLAYERS;
unsigned int speed = DEFAULT_SPEED;
unsigned int level = DEFAULT_LEVEL;
unsigned int player1Deaths = 0;
unsigned int player2Deaths = 0;




Snake player1;
Snake player2;
uint64_t board[SCREEN_WIDTH][SCREEN_HEIGHT];
int speeds[5] = {50, 35, 25, 13, 1};
bool quit = false;


void eliminator(){
    speed = DEFAULT_SPEED; 
    call_size_up();
    printStart();
    getCh();
    menu();
    call_size_down();
    }


void printStart(){
    call_paint_screen(DEFAULT_BCOLOR);
    call_drawWordColorAt("ELIMINATOR\n",DEFAULT_FCOLOR, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100);
    call_drawWordColorAt("Press any key to start\n", DEFAULT_FCOLOR, SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 );
}

void printMenu(){
    char playerStr[5], speedStr[5];
    intToStr(players, playerStr, 10);
    intToStr(speed, speedStr, 10);
    call_paint_screen(BLACK);
    call_drawWordColorAt("SPEED: ", DEFAULT_FCOLOR, MENU_X, MENU_Y);
    call_drawWordColorAt(speedStr, DEFAULT_FCOLOR, MENU_X + 175, MENU_Y);
    call_drawWordColorAt("[1] to begin single player game\n", DEFAULT_FCOLOR, MENU_X, MENU_Y + 256);
    call_drawWordColorAt("[2] to begin multiplayer local game\n", DEFAULT_FCOLOR, MENU_X, MENU_Y + 320);
    call_drawWordColorAt("[ENTER] to change settings\n", DEFAULT_FCOLOR, MENU_X, MENU_Y + 384);
    call_drawWordColorAt("[ESC] to return to the shell\n", DEFAULT_FCOLOR, MENU_X, MENU_Y + 448);
}

void printSettings(){
    call_paint_screen(BLACK);
    call_drawWordColorAt("[S] to decrease game speed ", DEFAULT_FCOLOR, MENU_X, MENU_Y + 64);
    call_drawWordColorAt("[D] to increase game speed ", DEFAULT_FCOLOR, MENU_X, MENU_Y + 128);
    call_drawWordColorAt("[Q] to return to menu", DEFAULT_FCOLOR, MENU_X, MENU_Y + 192);
}

void menu(){
    player1Deaths = 0;
    player2Deaths = 0;
    char option;
    int posMenu;
    printMenu();
    while(1){
        posMenu = call_get_pos();
        quit = false;
        option = call_get_charAt(posMenu-1);
        if(option == ESC){
            return;
        }else if (option == ONEPLAYER){
            if(players != 1){
                player1Deaths = 0;
            }
            players = 1;
            startGameOnePlayer();
            printMenu();
        }else if (option == TWOPLAYERS){
            if(players != 2){
                player1Deaths = 0;
                player2Deaths = 0;
            }
            players = 2;
            startGameTwoPlayers();
            printMenu();
        }else if(option == ENTER){
            changeSettings();
            printMenu();
        }
            }
}

void changeSettings(){
    printSettings();
    char settingsOption;
    int settingsPos;
    while(1){
        settingsPos = call_get_pos();
        settingsOption = call_get_charAt(settingsPos - 1);
        if(settingsOption == SPEEDKEYUP){
            if(speed < 5)
                speed += 1;
            return;
        }
        if(settingsOption == SPEEDKEYDOWN){
            if(speed > 1)
                speed -= 1;
            return;
        }
        if(settingsOption == QUIT)
            return;
    }
}

void startGameTwoPlayers(){
    initializeGameTwoPlayers();
    
    int pos = call_get_pos();
    char key;
    
    while (1) {
        key = call_get_charAt(pos-1);  // Obtener entrada del teclado (bloqueante)
        if(key != 0){
            pos++;
        }
        handleInput(key);
        updateGameTwoPlayers();
        if(quit){
            return;
        }
        call_sleepms(speeds[speed-1]);
    }
    return;
}
void startGameOnePlayer(){
    initializeGame();
    
    int pos = call_get_pos();
    char key;
    
    while (1) {
        key = call_get_charAt(pos-1);  // Obtener entrada del teclado (bloqueante)
        if(key != 0){
            pos++;
        }
        handleInput(key);
        updateGameOnePlayer();
        if(quit){
            //printMenu();
            return;
        }
        call_sleepms(speeds[speed-1]);
    }
    return;

}

void drawSegment(Segment seg, uint64_t color) {
    call_put_square(seg.x, seg.y, PLAYER_SIZE, color);
    for(int i = 0; i < PLAYER_SIZE; i++){
        for(int j = 0; j < PLAYER_SIZE; j++){
            board[seg.x + i][seg.y + j] = color;
        }
    }
}


void handleInput(char key) {
        if(key == player1Up){
            if(player1.direction != DOWN){
                player1.direction = UP;
                return;
            }
        }
        else if(key == player1Down){
            if(player1.direction != UP){
               player1.direction = DOWN; 
               return;
            }
        }
        else if(key == player1Left){
            if(player1.direction != RIGHT){
                player1.direction = LEFT;
                return;
            }
            }
        else if(key == player1Right){
            if(player1.direction != LEFT){
                player1.direction = RIGHT;
                return;
            }

        }
        else if(key == player2Up){
            if(player2.direction != DOWN){
                player2.direction = UP;
                return;
            }

        }
        else if(key == player2Down){
                if(player2.direction != UP){
                player2.direction = DOWN;
                return;
            }
        }
        else if(key == player2Left){
            if(player2.direction != RIGHT){
                player2.direction = LEFT;
                return;
            }
        }
        else if(key == player2Right){
            if(player2.direction != LEFT){
                player2.direction = RIGHT;
                return;
            }
        }
}


void initializeGame(){
    for(int i = 0; i < SCREEN_WIDTH; i++){
        for(int j = 0; j < SCREEN_HEIGHT; j++){
            board[i][j] = 0;
        }
    }

    player1.direction = UP;
    player1.color = RED;
    player1.head.x = SCREEN_WIDTH / 2;
    player1.head.y = SCREEN_HEIGHT - 25;

    call_paint_screen(BLACK);

    drawMargins();
    drawSegment(player1.head, player1.color);

}

void initializeGameTwoPlayers() {

    for(int i = 0; i < SCREEN_WIDTH; i++){
        for(int j = 0; j < SCREEN_HEIGHT; j++){
            board[i][j] = 0;
        }
    }

    player1.direction = UP;
    player1.color = RED;
    player1.head.x = SCREEN_WIDTH / 2;
    player1.head.y = SCREEN_HEIGHT - 25;

    player2.direction = DOWN;
    player2.color = GREEN;
    player2.head.x = SCREEN_WIDTH / 2;
    player2.head.y = 25;

    call_paint_screen(BLACK);

    drawMargins();

    drawSegment(player1.head, player1.color);
    drawSegment(player2.head, player2.color);
}

bool updateSnake(Snake *snake) {

    bool collision = false;

    switch (snake->direction){
        case UP:
            snake->head.y -= PLAYER_SIZE ;
            break;
        case DOWN:
            snake->head.y += PLAYER_SIZE;
            break;
        case LEFT:
            snake->head.x -= PLAYER_SIZE;
            break;
        case RIGHT:
            snake->head.x += PLAYER_SIZE;
            break;
    }

    for(int i = 0; i < PLAYER_SIZE; i++){
        for(int j = 0; j < PLAYER_SIZE; j++){
            if(board[snake->head.x + i][snake->head.y + j] != 0){
                collision = true;
            }
        }
    }
    drawSegment(snake->head, snake->color);
    return collision;
}

void updateGameOnePlayer(){
    if(updateSnake(&player1)){
        player1Deaths++;
        gameOver();
        if(quit){
            return;
        }
    }
}

void updateGameTwoPlayers() {
    bool player1Status = updateSnake(&player1);
    bool player2Status = updateSnake(&player2);
        if(player1Status){
            player1Deaths++;
            gameOver();
            if(quit){
                return;
            }
        }
        else if(player2Status){
            player2Deaths++;
            gameOver();
            if(quit){

                return;
            }
        }
    }

void drawDeathCounter(){
    char deathCount1[5], deathCount2[5];
    intToStr(player1Deaths,deathCount1, 10);
    intToStr(player2Deaths, deathCount2, 10);

    if(players == 1){ //1 jugador
        call_drawWordColorAt("PLAYER 1 DEATHS: ", DEFAULT_FCOLOR, SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT / 2 - 300);
        call_drawWordColorAt(deathCount1, DEFAULT_FCOLOR, SCREEN_WIDTH/2 + 75, SCREEN_HEIGHT /2 - 300);
    }else{
        call_drawWordColorAt("PLAYER 1 DEATHS: ", DEFAULT_FCOLOR, SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT / 2 - 300);
        call_drawWordColorAt(deathCount1, DEFAULT_FCOLOR, SCREEN_WIDTH/2 + 75, SCREEN_HEIGHT / 2 - 300);
        call_drawWordColorAt("PLAYER 2 DEATHS: ",DEFAULT_FCOLOR, SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT / 2 - 200);
        call_drawWordColorAt(deathCount2,  DEFAULT_FCOLOR, SCREEN_WIDTH/2 + 75, SCREEN_HEIGHT / 2 - 200);
    }    
}


void drawMargins(){
    for(int i = 20; i < SCREEN_WIDTH; i++){
        for(int j = 10; j < SCREEN_HEIGHT; j++){
            if((i == 20 && j >= 10 && j <= SCREEN_HEIGHT - 10) ||
               (i == SCREEN_WIDTH - 20 && j >= 10 && j <= SCREEN_HEIGHT - 10) ||
               (j == 10 && i >= 20 && i <= SCREEN_WIDTH - 20) ||
               (j == SCREEN_HEIGHT - 10 && i >= 20 && i <= SCREEN_WIDTH - 20)){
                board[i][j] = RED;
                call_put_square(i, j, 5, RED);
            }
        }
    }
}


void gameOver(){
    collision();
    call_paint_screen(BLACK);
    call_drawWordColorAt("GAME OVER\n", DEFAULT_FCOLOR, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100);
    call_drawWordColorAt("Press [Q] to return to menu\n", DEFAULT_FCOLOR, SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 50);
    call_drawWordColorAt("Press Space to continue\n", DEFAULT_FCOLOR, SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 10);
    int gameOverPos;
    char e;
    drawDeathCounter();
    while(1){
        gameOverPos = call_get_pos();
        e = call_get_charAt(gameOverPos - 1);
        if(e == QUIT){
            quit = true;
            return;
        }else if (e == SPACE){
            players==1? startGameOnePlayer() : startGameTwoPlayers();
        }
    }
    
}
