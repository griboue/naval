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

// Option 3

#endif
