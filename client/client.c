#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // disable close() warning
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "client.h"
#include "game.h"


int main(int argc, char const *argv[])
{

    show_menu();
    proceed_selection();

    return 0;
}

void show_menu()
{
    printf("Please select an option: \n\n");
    printf("1) Connect to a server \n");
    printf("2) Create a server \n");
    printf("3) Play against the machine \n\n");
}

void proceed_selection()
{
    int choice;
    scanf("%d",&choice);

    if (choice == 1)
    {
        connect_to_server();
    }
    else if (choice == 2)
    {
        printf("\n You selected option 2 \n");
    }
    else if (choice == 3)
    {
        printf("\n You selected option 2 \n");
    }
}

int socket_connection(char* ip, int port)
{
    int network_socket = 0;
    //socket(AF_INET, SOCK_STREAM, 0);

    // ajout d'une adresse à la socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // check si aucun probleme lors de la connexion
    if (connection_status < 0)
    {
        printf("Error while creating / connecting the socket");
        return -1;
    }

    return network_socket;
}



void connect_to_server()
{
    char ip[256];
    int port;
    // printf("\n Enter the IP address of the server please: ");
    // scanf("%s", ip);

    // printf("\n Enter the port number of the server please: ");
    // scanf("%d", &port);

    strcpy(ip, "127.0.0.1");
    port = 9002;

    // try to connect to the server with the IP and port
    int network_socket = socket_connection(ip, port);

    char server_reponse[256];
    char *message = malloc(sizeof(char)*1024);
    // clean string
    memset(server_reponse, 0, sizeof(server_reponse));


    recv(network_socket, &server_reponse, sizeof(server_reponse), 0);

    if (strcmp(server_reponse, "instruction: make board") == 0)
    {
        construct_game_board();
        show_game_board();

        // memset(message, 0, sizeof(message));
        put_ship(&message);
        printf("Message = %s\n", message);

        printf("Finish to make board\n");
    }

    if(send(network_socket, message, strlen(message), 0) < 0)
    {
        puts("Send failed");
    }

    while (1) {
        memset(server_reponse, 0, sizeof(server_reponse));
        recv(network_socket, server_reponse, sizeof(server_reponse), 0);
        //printf("Reponse server : %s\n", server_reponse);
        //Final condition
        if (strcmp(server_reponse, "You win") == 0) {
            printf("You win");
            return;
        }
        if (strcmp(server_reponse, "You loose") == 0) {
            printf("You loose");
            return;
        }

        if (strcmp(server_reponse, "Your shoot?") == 0) {
            show_game_board();

            char position1_read[20];
            int position_y;
            int position_x;

            int position1_y;
            int position1_x;
            int position2_y;
            int position2_x;

            do
            {
                printf("\033[1;34m"); // print to blue
                printf("Your shoot?");
                printf("\033[0m"); //reset color

                scanf("%s", position1_read);
                position_x = position1_read[0] - 65; // convert ASCII CHAR LETTER to int (from 0 to 9 max)
                position_y = position1_read[1] - 48; // convert ASCII CHAR NUMBER to int (from 0 to 9 max)
            } while (position_x >= 'A' && position_x <= 'J' && position_y >= 0 && position_y <= 9);
            message = position1_read;
            // strcat(message, (char)position_y);
            // message = position_y;/

            send(network_socket, message, strlen(message), 0);
            //On attends la reponse du serveur (tir à l'eau, touché, coulé)
            memset(server_reponse, 0, sizeof(server_reponse));                     // clean string
            recv(network_socket, &server_reponse, sizeof(server_reponse), 0);
            if (strcmp(server_reponse, "A l'eau") == 0) {
                enemy_game_board[position_y][position_x] = 'X';
            }
            if (strcmp(server_reponse, "Touché") == 0) {
                enemy_game_board[position_y][position_x] = 'x';
                enemy_game_board_color[position_y][position_x] = 'g';
            }
            if (strcmp(server_reponse, "Coulé") == 0) {
                enemy_game_board[position_y][position_x] = 'x';
                enemy_game_board_color[position_y][position_x] = 'x';
                //on recoit maintenant les coordonnées du bateau
                recv(network_socket, &server_reponse, sizeof(server_reponse), 0);
                position1_x = server_reponse[0] - 65;
                position1_y = server_reponse[1] - 48;
                position2_x = server_reponse[3] - 65;
                position2_y = server_reponse[4] - 48;
                // on colorie toutes les petites croix entre ces 2 coordonnées en rouge :)
                if (position1_y == position2_y)//the boat is on a line
                {
                    if (position1_x < position2_x)//boat left to right
                    {
                        for (size_t j = position1_x; j <= position2_x; j++)
                        {
                            enemy_game_board_color[position1_y][j] = 'r';
                        }
                    }
                    else//boat right to left
                    {
                        for (size_t j = position2_x; j <= position1_x; j++)
                        {
                            enemy_game_board_color[position1_y][j] = 'r';
                        }
                    }
                }
                else//the boat is on a column
                {
                    if (position1_y < position2_y)//boat top to bot
                    {
                        for (size_t j = position1_y; j <= position2_y; j++)
                        {
                            enemy_game_board_color[j][position1_x] = 'r';
                        }
                    }
                    else//boat bot to top
                    {
                        for (size_t j = position2_y; j <= position1_y; j++)
                        {
                            enemy_game_board_color[j][position1_x] = 'r';
                        }
                    }
                }
            }
            show_game_board();
        }

        if (strcmp(server_reponse, "Ya koi ici?") == 0) { //on doit renvoyer la case de notre tableau

            memset(server_reponse, 0, sizeof(server_reponse));         // clean string
            recv(network_socket, &server_reponse, sizeof(server_reponse), 0);

            int position_y;
            int position_x;

            position_x = server_reponse[0] - 65;
            position_y = server_reponse[1] - 48;

            //memset(message, 0, 1024);             // clean string
            //message = "+\0";
            printf("%c\n", game_board[position_x][position_y]);
            // strcpy(message, game_board[position_x][position_y]);
            message = (char*)malloc(sizeof(char));
            *message = game_board[position_y][position_x];
            if (game_board[position_y][position_x] == '-') {
                game_board[position_y][position_x] = 'X';
            }
            game_board_color[position_y][position_x] = 'r';
            printf("We have that at %c%c : %s\n",server_reponse[0],server_reponse[1], message);
            send(network_socket, message, strlen(message), 0);
        }
    }
}
