#ifndef GAME_H_  
#define GAME_H_

// global variable for the client board
extern char game_board[10][10];

void construct_game_board();
void show_game_board();
void put_ship(char* ship_type, char* message); 


#endif 