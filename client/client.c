#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // disable close() warning
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

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

void *launch_server(void *context)
{
    system("cd ../server/ && ./server");
    printf("Server closed");
    return NULL;
}

void proceed_selection()
{
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        connect_to_server();
    }
    else if (choice == 2)
    {
        pthread_t server;
        pthread_create(&server, NULL, launch_server, NULL);
        connect_to_server();
        // printf("\n You selected option 2 \n");
    }
    else if (choice == 3)
    {
        play_against_ia();
    }
}

int socket_connection(char *ip, int port)
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

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

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
    char *message = malloc(sizeof(char) * 1024);
    // clean string
    memset(server_reponse, 0, sizeof(server_reponse));

    recv(network_socket, &server_reponse, sizeof(server_reponse), 0);

    if (strcmp(server_reponse, "instruction: make board") == 0)
    {
        construct_game_board();
        show_game_board();
        put_ship(&message);
    }

    if (send(network_socket, message, strlen(message), 0) < 0)
    {
        puts("Send failed");
    }

    while (1)
    {
        memset(server_reponse, 0, sizeof(server_reponse));
        recv(network_socket, server_reponse, sizeof(server_reponse), 0);

        if (strcmp(server_reponse, "You win") == 0)
        {
            printf("You win");
            return;
        }
        if (strcmp(server_reponse, "You loose") == 0)
        {
            printf("You loose");
            return;
        }

        if (strcmp(server_reponse, "Your shoot?") == 0)
        {
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
                memset(position1_read, 0, sizeof position1_read);

                // position1_read = "";
                printf("\033[1;34m"); // print to blue
                printf("Your shoot?");
                printf("\033[0m"); //reset color

                scanf("%s", position1_read);
                position_x = position1_read[0] - 65; // convert ASCII CHAR LETTER to int (from 0 to 9 max)
                position_y = position1_read[1] - 48; // convert ASCII CHAR NUMBER to int (from 0 to 9 max)
            } while ((position_x < ('A' - 65)) || (position_x > ('J' - 65)) || (position_y < 0) || (position_y > 9));
            message = position1_read;

            send(network_socket, message, strlen(message), 0);
            //On attends la reponse du serveur (tir à l'eau, touché, coulé)
            memset(server_reponse, 0, sizeof(server_reponse)); // clean string
            recv(network_socket, &server_reponse, sizeof(server_reponse), 0);
            if (strcmp(server_reponse, "A l'eau") == 0)
            {
                enemy_game_board[position_y][position_x] = 'X';
            }
            if (strcmp(server_reponse, "Touché") == 0)
            {
                enemy_game_board[position_y][position_x] = 'x';
                enemy_game_board_color[position_y][position_x] = 'g';

                //on recoit les coordonnées complètes maintenant
                memset(server_reponse, 0, sizeof(server_reponse)); // clean string
                recv(network_socket, &server_reponse, sizeof(server_reponse), 0);

                position1_x = server_reponse[0] - 65;
                position1_y = server_reponse[1] - 48;
                position2_x = server_reponse[2] - 65;
                position2_y = server_reponse[3] - 48;

                //On vérifie que toutes les cases soit check entre les coordonnées du bateau
                if (estCoule(position1_x, position1_y, position2_x, position2_y) == 1) //bateau coulé
                {
                    // on colorie toutes les petites croix entre ces 2 coordonnées en rouge :)
                    if (position1_y == position2_y) //the boat is on a line
                    {
                        if (position1_x < position2_x) //boat left to right
                        {
                            for (size_t j = position1_x; j <= position2_x; j++)
                            {
                                enemy_game_board_color[position1_y][j] = 'r';
                            }
                        }
                        else //boat right to left
                        {
                            for (size_t j = position2_x; j <= position1_x; j++)
                            {
                                enemy_game_board_color[position1_y][j] = 'r';
                            }
                        }
                    }
                    else //the boat is on a column
                    {
                        if (position1_y < position2_y) //boat top to bot
                        {
                            for (size_t j = position1_y; j <= position2_y; j++)
                            {
                                enemy_game_board_color[j][position1_x] = 'r';
                            }
                        }
                        else //boat bot to top
                        {
                            for (size_t j = position2_y; j <= position1_y; j++)
                            {
                                enemy_game_board_color[j][position1_x] = 'r';
                            }
                        }
                    }
                }

                if (all_player_boats_sunk() == 1) //Le joueur a gagné
                {
                    system("clear");
                    printf("********************************************************************\n");
                    printf("  #     #  #######  #     #      #     #  ###  #     #      ### \n");
                    printf("   #   #   #     #  #     #      #  #  #   #   ##    #      ### \n");
                    printf("    # #    #     #  #     #      #  #  #   #   # #   #      ### \n");
                    printf("     #     #     #  #     #      #  #  #   #   #  #  #       #  \n");
                    printf("     #     #     #  #     #      #  #  #   #   #   # #          \n");
                    printf("     #     #     #  #     #      #  #  #   #   #    ##      ### \n");
                    printf("     #     #######   #####        ## ##   ###  #     #      ### \n");
                    printf("********************************************************************\n");
                    return;
                }
            }

            show_game_board();
        }

        if (strcmp(server_reponse, "Ya koi ici?") == 0) //on doit renvoyer la case de notre tableau
        {

            memset(server_reponse, 0, sizeof(server_reponse)); // clean string
            recv(network_socket, &server_reponse, sizeof(server_reponse), 0);

            int position_y;
            int position_x;

            position_x = server_reponse[0] - 65;
            position_y = server_reponse[1] - 48;

            message = (char *)malloc(4 * sizeof(char));

            if (game_board[position_y][position_x] == 'x' || game_board[position_y][position_x] == 'X') //Si le joueur est con et tire 2 fois au même endroit
            {
                *message = '-';
            }
            else
            {
                *message = game_board[position_y][position_x];
            }

            if (game_board[position_y][position_x] == '-')
            {
                game_board[position_y][position_x] = 'X';
            }
            game_board_color[position_y][position_x] = 'r';
            send(network_socket, message, strlen(message), 0);
            sleep(1);

            if (*message != '-') //il a touché un truc donc faut envoyer les coordonnées du bateau
            {
                get_coordinates(message[0], &message);
                send(network_socket, message, strlen(message), 0);
            }

            //We loose ?
            int counter = 0;
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                {
                    if (game_board_color[i][j] == 'r' && game_board[i][j] != '-' && game_board[i][j] != 'x' && game_board[i][j] != 'X')
                    {
                        counter++;
                    }
                }
            }
            if (counter == 2)
            {
                system("clear");
                printf("You loose ...");
                return;
            }
        }
    }
}

/**
 * @brief Return 1 if the boat is down or 0 else
 **/
int estCoule(int position1_x, int position1_y, int position2_x, int position2_y)
{
    if (position1_y == position2_y) //the boat is on a line
    {
        if (position1_x < position2_x) //boat left to right
        {
            for (size_t j = position1_x; j <= position2_x; j++)
            {
                if (enemy_game_board[position1_y][j] != 'x')
                {
                    return 0;
                }
            }
        }
        else //boat right to left
        {
            for (size_t j = position2_x; j <= position1_x; j++)
            {
                if (enemy_game_board[position1_y][j] != 'x')
                {
                    return 0;
                }
            }
        }
    }
    else //the boat is on a column
    {
        if (position1_y < position2_y) //boat top to bot
        {
            for (size_t j = position1_y; j <= position2_y; j++)
            {
                if (enemy_game_board[j][position1_x] != 'x')
                {
                    return 0;
                }
            }
        }
        else //boat bot to top
        {
            for (size_t j = position2_y; j <= position1_y; j++)
            {
                if (enemy_game_board[j][position1_x] != 'x')
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void play_against_ia()
{
    printf("\n in the function play against IA \n");

    // Generate IA board and place this ships
    construct_ia_game_board();
    generate_ia_board();

    // Generate the player board and ask him to place the ships
    construct_game_board();
    show_game_board();
    char *message; // useless variable
    put_ship(&message);

    // Launch the game
    play();

    //show_ia_game_board(); //this is for debug purposes only
}

void play()
{
    fflush(stdout);
    show_game_board();
    fflush(stdout);
    char position1_read[20];
    int position_y;
    int position_x;

    int game_finish = 0;

    while (game_finish != 1)
    {
        do
        {
            printf("\033[1;34m"); // print to blue
            printf("\n Your shoot? \n");
            printf("\033[0m"); //reset color

            scanf("%s", position1_read);
            position_x = position1_read[0] - 65; // convert ASCII CHAR LETTER to int (from 0 to 9 max)
            position_y = position1_read[1] - 48; // convert ASCII CHAR NUMBER to int (from 0 to 9 max)
        } while ((position_x < ('A' - 65)) || (position_x > ('J' - 65)) || (position_y < 0) || (position_y > 9));

        if (ia_game_board[position_y][position_x] == '-')
        {
            enemy_game_board[position_y][position_x] = 'X';
        }
        if (ia_game_board[position_y][position_x] != '-')
        {
            enemy_game_board[position_y][position_x] = 'x';
            enemy_game_board_color[position_y][position_x] = 'g';
            for (int v = 0; v < 4; v++)
            {
                if (estCoule(ia_boats_coordinates[v][0], ia_boats_coordinates[v][1], ia_boats_coordinates[v][2], ia_boats_coordinates[v][3]))
                {
                    // on colorie toutes les petites croix entre ces 2 coordonnées en rouge :)
                    if (ia_boats_coordinates[v][1] == ia_boats_coordinates[v][3]) //the boat is on a line
                    {
                        if (ia_boats_coordinates[v][0] < ia_boats_coordinates[v][2]) //boat left to right
                        {
                            for (size_t j = ia_boats_coordinates[v][0]; j <= ia_boats_coordinates[v][2]; j++)
                            {
                                enemy_game_board_color[ia_boats_coordinates[v][1]][j] = 'r';
                            }
                        }
                        else //boat right to left
                        {
                            for (size_t j = ia_boats_coordinates[v][2]; j <= ia_boats_coordinates[v][0]; j++)
                            {
                                enemy_game_board_color[ia_boats_coordinates[v][1]][j] = 'r';
                            }
                        }
                    }
                    else //the boat is on a column
                    {
                        if (ia_boats_coordinates[v][1] < ia_boats_coordinates[v][3]) //boat top to bot
                        {
                            for (size_t j = ia_boats_coordinates[v][1]; j <= ia_boats_coordinates[v][3]; j++)
                            {
                                enemy_game_board_color[j][ia_boats_coordinates[v][0]] = 'r';
                            }
                        }
                        else //boat bot to top
                        {
                            for (size_t j = ia_boats_coordinates[v][3]; j <= ia_boats_coordinates[v][1]; j++)
                            {
                                enemy_game_board_color[j][ia_boats_coordinates[v][0]] = 'r';
                            }
                        }
                    }
                    enemy_game_board_color[ia_boats_coordinates[v][1]][ia_boats_coordinates[v][0]] = 'r';
                    enemy_game_board_color[ia_boats_coordinates[v][3]][ia_boats_coordinates[v][2]] = 'r';
                }
            }
        }
        if (all_ia_boats_sunk())
        {
            game_finish = 1;
            printf("\n \nGAME TERMINATED - YOU WIN !!! \n");
        }
        else
        {
            // IA TURN
            make_ia_plays();
            fflush(stdout);
            show_game_board();
            fflush(stdout);
        }
        if (all_player_boats_sunk())
        {
            game_finish = 1;
            printf("\n \nGAME TERMINATED - YOU LOST !!! \n");
        }
    }
}

int all_ia_boats_sunk()
{
    int counter = 0;
    for (int v = 0; v < 4; v++)
    {
        if (estCoule(ia_boats_coordinates[v][0], ia_boats_coordinates[v][1], ia_boats_coordinates[v][2], ia_boats_coordinates[v][3]))
        {
            counter++;
        }
    }
    if (counter == 14)
        return 1;

    return 0;
}

int all_player_boats_sunk()
{
    int counter = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (game_board_color[i][j] == 'r')
            {
                counter++;
            }
        }
    }
    if (counter == 14)
        return 1;

    return 0;
}