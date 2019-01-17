#ifndef CLIENT_H_
#define CLIENT_H_

int main(int argc, char const *argv[]);

void show_menu();
void proceed_selection();
int estCoule(int position1_x, int position1_y, int position2_x, int position2_y);

// Option 1
int socket_connection(char *ip, int port);
void connect_to_server();

// Option 2
void *launch_server(void *context);

// Option 3
void play_against_ia();
void play();
int all_ia_boats_sunk();
int all_player_boats_sunk();

#endif
