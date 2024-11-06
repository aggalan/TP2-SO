// // This is a personal academic project. Dear PVS-Studio, please check it.
// // PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
// #include "include/usr_sys_calls.h"
// #include "include/lib.h"
// #include "include/Buffer.h"
// #include "include/eliminator.h"
// #define MOVE 8
// #define RAND_SEED_DEFAULT 42 // Default seed value
// #define BUFFER 10
// #define TITLE 0 // Flags to know in which state the game is
// #define CONFIGURATION 1
// #define GAME 2
// #define MID_GAME 3

// void initialize_positions();
// void title();
// void configuration();
// void game(int players);
// void mid_game();
// void game_speed();
// void fill_with_zeros();
// void ckeck_prev_key_1();
// void init_game();
// void check_prev_key_2();
// void buff_read_title();
// void buff_read_mid_game();
// void clear_buffer_eliminator();
// void pc_dir_change();
// void buff_read(int len, int players);

// int state, flag;
// int flag_config;
// char speed, players;
// static char buffer[BUFFER] = {0};
// static uint16_t position_matrix[HEIGHT / MOVE][WIDTH / MOVE];

// typedef struct
// {
//     int pos_x, pos_y, points;
//     char prev_key;
// } player;

// void move_player(int x, int y, player *player_a);

// player player1, player2;

// // Reads the buffer int the state TITLE
// void buff_read_title()
// {
//     while (1)
//     {
//         char c;
//         getC(&c);
//         if (c == '\n')
//         {
//             call_clear();
//             state++;
//             configuration();
//             return;
//         }
//         else if (c == 'x')
//         {
//             call_clear();
//             call_set_font_size(1);
//             flag = 0;
//             return;
//         }
//     }
// }

// // Reads the buffer int the state mid_game
// void buff_read_mid_game()
// {
//     while (1)
//     {
//         char c;
//         getC(&c);
//         if (c == 'y')
//         {
//             state = GAME;
//             game(players);
//             return;
//         }
//         else if (c == 'n')
//         {
//             call_clear();
//             call_set_font_size(1);
//             state = TITLE;
//             flag = 0;
//             return;
//         }
//     }
// }

// // Starts the game in the TITLE state
// void start_eliminator()
// {
//     state = TITLE;
//     player1.prev_key = 's';
//     player2.prev_key = 'u';
//     player1.points = player2.points = 0;
//     flag = 1;
//     call_clear();
//     title();
//     while (flag)
//     {
//         if (state == TITLE)
//             buff_read_title();
//         if (state == MID_GAME)
//             buff_read_mid_game();
//     }
// }

// // Clears the buffer
// void clear_buffer_eliminator()
// {
//     for (int i = 0; i < BUFFER; i++)
//     {
//         buffer[i] = 0;
//     }
// }

// // Reads the buffer
// void buff_read(int len, int players)
// {
//     char c;
//     int i = len * 4000;
//     while (i > 0)
//     {
//         getC(&c);
//         call_clear();
//         if (state == GAME)
//         {
//             if (c == 'w' && player1.prev_key != 's')
//             {
//                 player1.prev_key = c;
//             }
//             else if (c == 's' && player1.prev_key != 'w')
//             {
//                 player1.prev_key = c;
//             }
//             else if (c == 'd' && player1.prev_key != 'a')
//             {
//                 player1.prev_key = c;
//             }
//             else if (c == 'a' && player1.prev_key != 'd')
//             {
//                 player1.prev_key = c;
//             }
//             if (players == 2)
//             {
//                 if (c == 'u' && player2.prev_key != 'j')
//                 {
//                     player2.prev_key = c;
//                 }
//                 else if (c == 'j' && player2.prev_key != 'u')
//                 {
//                     player2.prev_key = c;
//                 }
//                 else if (c == 'k' && player2.prev_key != 'h')
//                 {
//                     player2.prev_key = c;
//                 }
//                 else if (c == 'h' && player2.prev_key != 'k')
//                 {
//                     player2.prev_key = c;
//                 }
//             }
//         }
//         i--;
//     }
// }

// // Prints the title
// void title()
// {
//     call_set_font_size(2);
//     call_move_cursor_x((WIDTH / 2) - (str_len("ELIMINATOR") / 2) * 8 * 2);
//     call_move_cursor_y(HEIGHT / 3);
//     print(RED, "ELIMINATOR\n");
//     call_move_cursor_x((WIDTH / 2) - (str_len("PRESS [ENTER] TO CONTINUE") / 2) * 8 * 2);
//     print(RED, "PRESS [ENTER] TO CONTINUE\n");
//     call_move_cursor_x((WIDTH / 2) - (str_len("PRESS [X] TO EXIT") / 2) * 8 * 2);
//     print(RED, "PRESS [X] TO EXIT\n");
// }

// // Prints the configuration and starts the game
// void configuration()
// {
//     call_move_cursor_x((WIDTH / 2) - (str_len("CONFIGURATION") / 2) * 8 * 2);
//     print(RED, "CONFIGURATION\n");
//     call_move_cursor_x((WIDTH / 2) - (str_len("PLAYER 1 (RED) MOVES WITH A-W-S-D") / 2) * 8 * 2);
//     print(RED, "PLAYER 1 (RED) MOVES WITH A-W-S-D\n");
//     call_move_cursor_x((WIDTH / 2) - (str_len("PLAYER 2 (GREEN) MOVES WITH H-U-J-K") / 2) * 8 * 2);
//     print(RED, "PLAYER 2 (GREEN) MOVES WITH H-U-J-K\n");
//     call_move_cursor_x((WIDTH / 2) - (str_len("PRESS [ENTER] TO START") / 2) * 8 * 2);
//     print(RED, "PRESS [ENTER] TO START\n");
//     flag_config = 0;
//     while (state == CONFIGURATION)
//     {
//         if (flag_config == 0)
//         {
//             print(RED, "GAME SPEED (1-4): ");
//             game_speed();
//             if (state == CONFIGURATION && flag_config == 0)
//             {
//                 print(RED, "INVALID SPEED\n");
//             }
//         }
//         else
//         {
//             print(RED, "PLAYERS (1-2): ");
//             game_speed();
//             if (state == CONFIGURATION)
//             {
//                 print(RED, "INVALID PLAYERS\n");
//             }
//         }
//     }
//     game(players);
//     return;
// }

// // Reads the buffer to check the game speed
// void game_speed()
// {
//     int i = 0;
//     char c;
//     while (1)
//     {
//         getC(&c);
//         if (c != 0)
//         {
//             if (c == '\n')
//             {
//                 putC(c, RED);
//                 if (i == 0)
//                 {
//                     clear_buffer_eliminator();
//                     return;
//                 }
//                 if ((buffer[0] == '1' || buffer[0] == '2' || buffer[0] == '3' || buffer[0] == '4') && i == 1 && flag_config == 0)
//                 {
//                     speed = str_to_int(buffer);
//                     flag_config++;
//                 }
//                 else if ((buffer[0] == '1' || buffer[0] == '2') && i == 1 && flag_config == 1)
//                 {
//                     players = str_to_int(buffer);
//                     state++;
//                 }
//                 buffer[i] = 0;
//                 clear_buffer_eliminator();
//                 return;
//             }
//             else if (c == '\b')
//             {
//                 if (i > 0)
//                 {
//                     i--;
//                     putC(c, RED);
//                 }
//             }
//             else
//             {
//                 if (i < RED)
//                 {
//                     buffer[i++] = c;
//                     putC(c, RED);
//                 }
//             }
//         }
//     }
// }

// // Checks if the player has won
// int check_mat(player *player_a, player *playerB)
// {
//     if (position_matrix[player_a->pos_y / MOVE][player_a->pos_x / MOVE] == 1)
//     {
//         playerB->points++;
//         state++;
//         mid_game();
//         return 1;
//     }
//     return 0;
// }

// // Sets the matrix with the player position
// void set_mat(player player_a)
// {
//     position_matrix[player_a.pos_y / MOVE][player_a.pos_x / MOVE] = 1;
// }

// // Initializes the game
// void init_game()
// {
//     call_clear();
//     call_draw_rectangle(RED, 0, 0, HEIGHT, WIDTH);
//     call_draw_rectangle(BLACK, MOVE, MOVE, HEIGHT - (MOVE * 2), WIDTH - (MOVE * 2));
//     fill_with_zeros();
//     initialize_positions();
//     state = GAME;
// }

// // Starts the game
// void game(int players)
// {
//     init_game();
//     while (state == GAME)
//     {
//         buff_read(120 / (3 + speed), players);
//         if (players == 1)
//         {
//             pc_dir_change();
//         }
//         ckeck_prev_key_1();
//         check_prev_key_2();
//         if (check_mat(&player1, &player2))
//         {
//             return;
//         }
//         else if (check_mat(&player2, &player1))
//         {
//             return;
//         }
//         call_draw_rectangle(RED, player1.pos_x, player1.pos_y, MOVE, MOVE);
//         call_draw_rectangle(GREEN, player2.pos_x, player2.pos_y, MOVE, MOVE);
//         set_mat(player2);
//         set_mat(player1);
//     }
// }

// // Moves the player
// void move_player(int x, int y, player *player_a)
// {
//     player_a->pos_y += y;
//     player_a->pos_x += x;
// }

// // Checks the previous key of the player
// void ckeck_prev_key_1()
// {
//     switch (player1.prev_key)
//     {
//     case 'w':
//         move_player(0, -MOVE, &player1);
//         return;
//     case 's':
//         move_player(0, MOVE, &player1);
//         return;
//     case 'd':
//         move_player(MOVE, 0, &player1);
//         return;
//     case 'a':
//         move_player(-MOVE, 0, &player1);
//         return;
//     }
// }

// // Checks the previous key of the player
// void check_prev_key_2()
// {
//     switch (player2.prev_key)
//     {
//     case 'u':
//         move_player(0, -MOVE, &player2);
//         return;
//     case 'j':
//         move_player(0, MOVE, &player2);
//         return;
//     case 'k':
//         move_player(MOVE, 0, &player2);
//         return;
//     case 'h':
//         move_player(-MOVE, 0, &player2);
//         return;
//     }
// }

// // Fills the matrix with zeros
// void fill_with_zeros()
// {
//     for (int i = 0; i < HEIGHT / MOVE; i++)
//     {
//         for (int j = 0; j < WIDTH / MOVE; j++)
//         {
//             if (i == 0 || i == HEIGHT / MOVE - 1 || j == 0 || j == WIDTH / MOVE - 1)
//             {
//                 position_matrix[i][j] = 1;
//             }
//             else
//                 position_matrix[i][j] = 0;
//         }
//     }
// }

// // Initializes the positions of the players
// void initialize_positions()
// {
//     player1.prev_key = 's';
//     player2.prev_key = 'u';
//     player1.pos_x = WIDTH / 2;
//     player2.pos_x = WIDTH / 2;
//     player1.pos_y = 0;
//     player2.pos_y = HEIGHT - MOVE;
// }

// // Prints the mid_game
// void mid_game()
// {
//     call_beep();
//     call_move_cursor_x((WIDTH / 2) - (str_len("Player 1:") / 2) * 8 * 2);
//     call_move_cursor_y(HEIGHT / 4);
//     print(RED, "Player 1: %d\n", player1.points);
//     call_move_cursor_x((WIDTH / 2) - (str_len("Player 2:") / 2) * 8 * 2);
//     print(GREEN, "Player 2: %d\n", player2.points);
//     call_move_cursor_x((WIDTH / 2) - (str_len("Do you want to continue? [Y/N]") / 2) * 8 * 2);
//     print(RED, "Do you want to continue? [Y/N]\n");

//     return;
// }

// // Changes the direction of the PC (BOT)
// void pc_dir_change()
// {
//     // Possible moves and corresponding keys
//     int possible_moves[4][2] = {{0, -MOVE}, {MOVE, 0}, {0, MOVE}, {-MOVE, 0}};
//     char possible_keys[4] = {'u', 'k', 'j', 'h'};

//     // Array to store valid moves
//     int valid_moves[4] = {0, 0, 0, 0};
//     int valid_move_count = 0;

//     // Check each possible move for validity
//     for (int i = 0; i < 4; i++)
//     {
//         if ((possible_keys[i] == 'u' && player2.prev_key != 'j') ||
//             (possible_keys[i] == 'j' && player2.prev_key != 'u') ||
//             (possible_keys[i] == 'k' && player2.prev_key != 'h') ||
//             (possible_keys[i] == 'h' && player2.prev_key != 'k'))
//         {
//             int new_x = player2.pos_x + possible_moves[i][0];
//             int new_y = player2.pos_y + possible_moves[i][1];

//             // Check if the new position is within bounds and not colliding with Player 1
//             if (new_x >= MOVE && new_x < WIDTH - MOVE && new_y >= MOVE && new_y < HEIGHT - MOVE &&
//                 position_matrix[new_y / MOVE][new_x / MOVE] == 0 &&
//                 !(new_x == player1.pos_x && new_y == player1.pos_y))
//             {
//                 valid_moves[i] = 1;
//                 valid_move_count++;
//             }
//         }
//     }
//     int i = 0;
//     if (valid_move_count > 0)
//     {
//         int selected_move;
//         if (valid_move_count == 3)
//         {
//             return;
//         }

//         do
//         {
//             selected_move = i++;
//         } while (valid_moves[selected_move] == 0 && i < 4);
//         player2.prev_key = possible_keys[selected_move];
//     }
// }