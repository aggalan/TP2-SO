#include "include/usr_sys_calls.h"
#include "include/lib.h"
#include "include/Buffer.h"
#include "include/eliminator.h"
#define MOVE 8
#define RAND_SEED_DEFAULT 42 // Default seed value
#define BUFFER 10
#define TITLE 0  //Flags to know in which state the game is
#define CONFIGURATION 1
#define GAME 2
#define MIDGAME 3

void initializePositions();
void title();
void configuration();
void game(int players);
void midGame();
void gameSpeed();
void fillWithZeros();
void checkPrevKey1();
void initGame();
void checkPrevKey2();
void buffReadTitle();
void buffReadMidGame();
void clearBufferEliminator();
void pcDirChange();
void buffRead(int len, int players);

int state,flag;
int flagConfig;
char speed, players;
static char buffer[BUFFER] = {0};
static uint16_t PositionMatrix[HEIGHT/MOVE][WIDTH/MOVE];

typedef struct {
    int posX, posY, points;
    char prevKey;
}player;

void movePlayer(int x,int y, player * playerA);

player player1, player2;


//Reads the buffer int the state TITLE
void buffReadTitle(){
    while(1) {
        char c;
        getC(&c);
        if (c == '\n') {
            call_clear();
            state++;
            configuration();
            return;
        } else if (c == 'x') {
            call_clear();
            call_setFontSize(1);
            flag = 0;
            return;
        }
    }
}

//Reads the buffer int the state MIDGAME
void buffReadMidGame(){
    while(1) {
        char c;
        getC(&c);
        if(c == 'y'){
            state=GAME;
            game(players);
            return;
        }else if(c == 'n'){
            call_clear();
            call_setFontSize(1);
            state=TITLE;
            flag = 0;
            return;
        }
    }
}

//Starts the game in the TITLE state
void startEliminator(){
    state=TITLE;
    player1.prevKey = 's';
    player2.prevKey = 'u';
    player1.points=player2.points=0;
    flag=1;
    call_clear();
    title();
    while(flag){
        if(state == TITLE)
            buffReadTitle();
        if(state == MIDGAME)
            buffReadMidGame();
    }
}

//Clears the buffer
void clearBufferEliminator(){
    for(int i=0;i<BUFFER;i++){
        buffer[i]=0;
    }
}

//Reads the buffer
void buffRead(int len, int players){
    char c;
    int i=len * 4000;
    while (i > 0) {
        getC(&c);
        if (state == GAME) {
            if (c == 'w' && player1.prevKey != 's') {
                player1.prevKey = c;

            } else if (c == 's' && player1.prevKey != 'w') {
                player1.prevKey = c;


            } else if (c == 'd' && player1.prevKey != 'a') {
                player1.prevKey = c;


            } else if (c == 'a' && player1.prevKey != 'd') {
                player1.prevKey = c;

            }
            if (players == 2){
                if (c == 'u' && player2.prevKey != 'j') {
                    player2.prevKey = c;

                } else if (c == 'j' && player2.prevKey != 'u') {
                    player2.prevKey = c;

                } else if (c == 'k' && player2.prevKey != 'h') {
                    player2.prevKey = c;
                } else if (c == 'h' && player2.prevKey != 'k') {
                    player2.prevKey = c;

                }
            }
        }
        i--;
    }
}

//Prints the title
void title(){
    call_setFontSize(2);
    call_moveCursorX((WIDTH/2)-(strlen("ELIMINATOR")/2) *8 * 2);
    call_moveCursorY(HEIGHT/3);
    print(RED,"ELIMINATOR\n");
    call_moveCursorX((WIDTH/2)-(strlen("PRESS [ENTER] TO CONTINUE")/2) *8 * 2);
    print(RED,"PRESS [ENTER] TO CONTINUE\n");
    call_moveCursorX((WIDTH/2)-(strlen("PRESS [X] TO EXIT")/2) *8 * 2);
    print(RED,"PRESS [X] TO EXIT\n");
}

//Prints the configuration and starts the game
void configuration(){
    call_moveCursorX((WIDTH/2)-(strlen("CONFIGURATION")/2) *8 * 2);
    print(RED,"CONFIGURATION\n");
    call_moveCursorX((WIDTH/2)-(strlen("PLAYER 1 (RED) MOVES WITH A-W-S-D")/2) *8 * 2);
    print(RED,"PLAYER 1 (RED) MOVES WITH A-W-S-D\n");
    call_moveCursorX((WIDTH/2)-(strlen("PLAYER 2 (GREEN) MOVES WITH H-U-J-K")/2) *8 * 2);
    print(RED,"PLAYER 2 (GREEN) MOVES WITH H-U-J-K\n");
    call_moveCursorX((WIDTH/2)-(strlen("PRESS [ENTER] TO START")/2) *8 * 2);
    print(RED,"PRESS [ENTER] TO START\n");
    flagConfig = 0;
    while(state == CONFIGURATION){
        if(flagConfig == 0){
            print(RED,"GAME SPEED (1-4): ");
            gameSpeed();
            if(state == CONFIGURATION && flagConfig == 0){
                print(RED,"INVALID SPEED\n");
            }
        }else{
            print(RED,"PLAYERS (1-2): ");
            gameSpeed();
            if(state == CONFIGURATION){
                print(RED,"INVALID PLAYERS\n");
            }
        }
    }
    game(players);
    return;
}

//Reads the buffer to check the game speed
void gameSpeed(){
    int i = 0;
    char c;
    while (1) {
        getC(&c);
        if(c != 0){
            if (c == '\n'){
                putC(c,RED);
                if (i == 0){
                    clearBufferEliminator();
                    return;
                }
                if((buffer[0] == '1' || buffer[0] == '2' || buffer[0] == '3' || buffer[0] == '4') && i == 1 && flagConfig == 0){
                    speed = strToInt(buffer);
                    flagConfig++;
                }else if((buffer[0] == '1' || buffer[0] == '2') && i == 1 && flagConfig == 1){
                    players = strToInt(buffer);
                    state++;
                }
                buffer[i]=0;
                clearBufferEliminator();
                return;
            }else if (c == '\b'){
                if (i > 0){
                    i--;
                    putC(c,RED);
                }
            }
            else{
                if (i < RED){
                    buffer[i++] = c;
                    putC(c,RED);
                }
            }
        }
    }
}

//Checks if the player has won
int checkMat( player* playerA, player* playerB){
    if(PositionMatrix[playerA->posY /MOVE][playerA->posX/MOVE]==1){
        playerB->points++;
        state++;
        midGame();
        return 1;
    }
    return 0;
}

//Sets the matrix with the player position
void setMat(player playerA){
    PositionMatrix[playerA.posY/MOVE][playerA.posX/MOVE]=1;
}

//Initializes the game
void initGame(){
    call_clear();
    call_drawRectangle(RED,0,0,HEIGHT,WIDTH);
    call_drawRectangle(BLACK, MOVE, MOVE, HEIGHT - (MOVE*2), WIDTH - (MOVE*2));
    fillWithZeros();
    initializePositions();
    state = GAME;
}

//Starts the game
void game(int players){
    initGame();
    while(state == GAME){
        buffRead(120/(3+speed), players);
        if(players == 1){
            pcDirChange();
        }
        checkPrevKey1();
        checkPrevKey2();
        if(checkMat(&player1, &player2)){
            return;
        }else if(checkMat(&player2, &player1)){
            return;
        }
        call_drawRectangle(RED,player1.posX,player1.posY,MOVE,MOVE);
        call_drawRectangle(GREEN,player2.posX,player2.posY,MOVE,MOVE);
        setMat(player2);
        setMat(player1);
    }
}

//Moves the player
void movePlayer(int x,int y, player * playerA){
    playerA->posY+=y;
    playerA->posX+=x;
}

//Checks the previous key of the player
void checkPrevKey1(){
    switch (player1.prevKey) {
        case 'w':
            movePlayer(0,-MOVE, &player1);
            return;
        case 's':
            movePlayer(0,MOVE, &player1);
            return;
        case 'd':
            movePlayer(MOVE,0, &player1);
            return;
        case 'a':
            movePlayer(-MOVE,0, &player1);
            return;
    }
}

//Checks the previous key of the player
void checkPrevKey2(){
    switch (player2.prevKey) {
        case 'u':
            movePlayer(0,-MOVE,&player2);
            return;
        case 'j':
            movePlayer(0,MOVE, &player2);
            return;
        case 'k':
            movePlayer(MOVE,0, &player2);
            return;
        case 'h':
            movePlayer(-MOVE,0,&player2);
            return;
    }

}

//Fills the matrix with zeros
void fillWithZeros(){
    for(int i=0;i<HEIGHT/MOVE;i++){
        for(int j=0;j<WIDTH/MOVE;j++){
            if(i==0 || i==HEIGHT/MOVE-1 || j==0 || j==WIDTH/MOVE-1){
                PositionMatrix[i][j]=1;
            }else
                PositionMatrix[i][j]=0;
        }
    }
}

//Initializes the positions of the players
void initializePositions(){
    player1.prevKey='s';
    player2.prevKey='u';
    player1.posX=WIDTH/2;
    player2.posX=WIDTH/2;
    player1.posY=0;
    player2.posY=HEIGHT-MOVE;
}


//Prints the midgame
void midGame(){
    call_beep();
    call_moveCursorX((WIDTH/2)-(strlen("Player 1:")/2) *8 * 2);
    call_moveCursorY(HEIGHT/4);
    print(RED,"Player 1: %d\n", player1.points);
    call_moveCursorX((WIDTH/2)-(strlen("Player 2:")/2) *8 * 2);
    print(GREEN,"Player 2: %d\n", player2.points);
    call_moveCursorX((WIDTH/2)-(strlen("Do you want to continue? [Y/N]")/2) *8 * 2);
    print(RED,"Do you want to continue? [Y/N]\n");

    return;
}


//Changes the direction of the PC (BOT)
void pcDirChange(){
    // Possible moves and corresponding keys
    int possibleMoves[4][2] = {{0, -MOVE},  {MOVE, 0}, {0, MOVE}, {-MOVE, 0}};
    char possibleKeys[4] = {'u','k','j', 'h'};

    // Array to store valid moves
    int validMoves[4] = {0, 0, 0, 0};
    int validMoveCount = 0;

    // Check each possible move for validity
    for (int i = 0; i < 4; i++) {
        if ((possibleKeys[i] == 'u' && player2.prevKey != 'j') ||
            (possibleKeys[i] == 'j' && player2.prevKey != 'u') ||
            (possibleKeys[i] == 'k' && player2.prevKey != 'h') ||
            (possibleKeys[i] == 'h' && player2.prevKey != 'k')) {
            int newX = player2.posX + possibleMoves[i][0];
            int newY = player2.posY + possibleMoves[i][1];

            // Check if the new position is within bounds and not colliding with Player 1
            if (newX >= MOVE && newX < WIDTH - MOVE && newY >= MOVE && newY < HEIGHT - MOVE &&
                PositionMatrix[newY / MOVE][newX / MOVE] == 0 &&
                !(newX == player1.posX && newY == player1.posY)) {
                validMoves[i] = 1;
                validMoveCount++;
            }

        }
    }
    int i =0;
    if (validMoveCount > 0) {
        int selectedMove;
        if(validMoveCount == 3 ){
            return;
        }

        do {
            selectedMove = i++;
        } while (validMoves[selectedMove] == 0 && i <4);
        player2.prevKey = possibleKeys[selectedMove];
    }
}