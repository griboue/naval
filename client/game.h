#ifndef GAME_H_
#define GAME_H_

// global variable for the client board
extern char game_board[10][10];
extern char game_board_color[10][10];
extern char enemy_game_board[10][10];
extern char enemy_game_board_color[10][10];
char *boats_coordinates[4][2];
extern char ia_game_board[10][10];
extern int ia_boats_coordinates[4][4]; // the 4 positions (p1x, p1y, p2x, p2y) of the 4 boats
extern int player_boats_coordinates[4][4];

void construct_game_board();
void show_game_board();
void put_ship(char **message);
int length_of_ship(int position1_y, int position1_x, int position2_y, int position2_x);
int get_coordinates(char sign, char **message);
void generate_ia_board();
void construct_ia_game_board();
void show_ia_game_board();
void make_ia_plays();

#endif
