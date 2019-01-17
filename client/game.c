#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "data.h"

char game_board[10][10];
char game_board_color[10][10];
char enemy_game_board[10][10];
char enemy_game_board_color[10][10];
int player_boats_coordinates[4][4];

char ia_game_board[10][10];
int ia_boats_coordinates[4][4];

/**
 * @brief
 * Initialize an empty game board
 * */
void construct_game_board()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            game_board[i][j] = '-';
            game_board_color[i][j] = 'w';
            enemy_game_board[i][j] = '-';
            enemy_game_board_color[i][j] = 'w';
        }
    }
}

/**
 * @brief
 * Show the global main board on the console
 */
void show_game_board()
{
    fflush(stdout);
    printf("\n \n \n");
    printf("Your game board: ");
    printf("\t\t");
    printf("The enemy board: ");
    printf("\n \n  ");

    for (int j = 0; j < 10; j++)
    {
        printf(" ");
        printf("%c", (j + 65)); // for UPPERCASE asci table
    }
    printf("\t\t  ");
    for (int j = 0; j < 10; j++)
    {
        printf(" ");
        printf("%c", (j + 65)); // for UPPERCASE asci table
    }

    printf("\n");
    for (int i = 0; i < 10; i++)
    {
        printf(" %d ", i);
        for (int j = 0; j < 10; j++)
        {
            if (game_board_color[i][j] == 'r')
            {
                printf("\033[31m"); // print to red
            }
            if (game_board_color[i][j] == 'g')
            {
                printf("\033[32m"); // print to green
            }
            printf("%c", game_board[i][j]);
            printf("\033[0m"); //reset color
            printf(" ");
        }
        printf("\t\t");
        printf(" %d ", i);
        for (int j = 0; j < 10; j++)
        {
            if (enemy_game_board_color[i][j] == 'r')
            {
                printf("\033[31m"); // print to red
            }
            if (enemy_game_board_color[i][j] == 'g')
            {
                printf("\033[32m"); // print to green
            }
            printf("%c", enemy_game_board[i][j]);
            printf("\033[0m"); //reset color
            printf(" ");
        }

        printf("\n");
    }
    printf("\n \n");
    fflush(stdout);
}

/**
 * @brief
 * Put a ship on the global client board
 *
 * @param ship_type
 * @param message: the serialized message to send to the server (to verify correctness of ship placement)
 */
void put_ship(char **message)
{
    char position1_read[20];
    char position2_read[20];
    int position1_y;
    int position1_x;
    int position2_y;
    int position2_x;
    int error;

    for (size_t i = 0; i < number_boats; i++)
    // for (size_t i = 0; i < 1; i++)
    {
        printf("\033[1;34m"); // print to blue
        printf("You have to put a %s on the board ! (size of %d)", boats[i], boats_size[i]);
        printf("\033[0m;"); //reset color

        do
        {
            printf("\n \n Pease indicate the FIRST cell of the ship:(ex. 'B6') \n");
            scanf("%s", position1_read);
            position1_x = position1_read[0] - 65; // convert ASCII CHAR LETTER to int (from 0 to 9 max)
            position1_y = position1_read[1] - 48; // convert ASCII CHAR NUMBER to int (from 0 to 9 max)

            printf("\n \n Pease indicate the LAST cell of the ship:(ex. 'B6') \n");
            scanf("%s", position2_read);
            position2_x = position2_read[0] - 65; // convert ASCII CHAR LETTER to int (from 0 to 9 max)
            position2_y = position2_read[1] - 48; // convert ASCII CHAR NUMBER to int (from 0 to 9 max)

            //Check is size is legit
            int size = length_of_ship(position1_y, position1_x, position2_y, position2_x);
            if (size != boats_size[i])
            {
                error = 1;
                printf("\033[1;34m"); // print to red
                printf("\n This ship must have a size of %d and the actual size is %d", boats_size[i], size);
                printf("\033[0m;"); //reset color
            }
            else
                error = 0;
        } while (error);

        //On inscrit les coordonnées dans un tableau pour pouvoir les avoirs facilement par la suite
        boats_coordinates[i][0] = malloc(2 * sizeof(char));
        boats_coordinates[i][1] = malloc(2 * sizeof(char));
        strcpy(boats_coordinates[i][0], position1_read);
        strcpy(boats_coordinates[i][1], position2_read);

        player_boats_coordinates[i][0] = position1_x;
        player_boats_coordinates[i][1] = position1_y;
        player_boats_coordinates[i][2] = position2_x;
        player_boats_coordinates[i][3] = position2_y;

        //Inscription des signes du bateau dans le tableau
        if (position1_y == position2_y) //the boat is on a line
        {
            if (position1_x < position2_x) //boat left to right
            {
                for (size_t j = position1_x; j <= position2_x; j++)
                {
                    game_board[position1_y][j] = boats_sign[i];
                }
            }
            else //boat right to left
            {
                for (size_t j = position2_x; j <= position1_x; j++)
                {
                    game_board[position1_y][j] = boats_sign[i];
                }
            }
        }
        else //the boat is on a column
        {
            if (position1_y < position2_y) //boat top to bot
            {
                for (size_t j = position1_y; j <= position2_y; j++)
                {
                    game_board[j][position1_x] = boats_sign[i];
                }
            }
            else //boat bot to top
            {
                for (size_t j = position2_y; j <= position1_y; j++)
                {
                    game_board[j][position1_x] = boats_sign[i];
                }
            }
        }
        show_game_board();
    }

    // message to send to the server (serialized in a string message)
    *message = "I'm ready \0";
}

/**
 * @brief
 * This function returns the length between the points passed in parameters
 * Returns -1 if it is not possbile (ex: diagonals, outside the table ...)
 **/
int length_of_ship(int position1_y, int position1_x, int position2_y, int position2_x)
{
    if (position1_x != position2_x && position1_y != position2_y) //diagonals
        return -1;
    if (position1_y == position2_y) //the boat is on a line
    {
        if (position1_x < position2_x) //boat left to right
            return position2_x - position1_x + 1;
        else //boat right to left
            return position1_x - position2_x + 1;
    }
    else //the boat is on a column
    {
        if (position1_y < position2_y) //boat top to bot
            return position2_y - position1_y + 1;
        else //boat bot to top
            return position1_y - position2_y + 1;
    }
}

/**
 * @brief
 * Return in the param message the coordinates of 
 * the ship that have the symbol put in param
 * 
 * @param sign : symbol of the ship search
 * @param message : variable where will be store the result (eg : B5B6)
 * 
 **/
int get_coordinates(char sign, char **message)
{
    for (size_t i = 0; i < 1; i++)
    {
        printf("Sign = %c, X=%s, Y=%s", boats_sign[i], boats_coordinates[i][0], boats_coordinates[i][1]);
    }
    for (size_t i = 0; i < 1; i++) //TODO
    {
        if (sign == boats_sign[i])
        {
            *message = (char *)malloc(8 * sizeof(char));
            strcpy(*message, boats_coordinates[i][0]);
            strcat(*message, boats_coordinates[i][1]);
            return 0;
        }
    }
    return -1;
}

// ------- UNDER IS THE IA PART ----------
// ---------------------------------------
// ---------------------------------------

// Only for debug purposes
void show_ia_game_board()
{
    fflush(stdout);
    printf("\n \n \n");
    printf("AI game board: \n \n");
    printf("  ");
    for (int j = 0; j < 10; j++)
    {
        printf(" ");
        printf("%c", (j + 65)); // for UPPERCASE asci table
    }
    printf("\n");
    for (int i = 0; i < 10; i++)
    {
        printf(" %d ", i);
        for (int j = 0; j < 10; j++)
        {
            printf("%c", ia_game_board[i][j]);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n \n");
    fflush(stdout);
}

void construct_ia_game_board()
{
    fflush(stdout);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            ia_game_board[i][j] = '-';
        }
    }
    fflush(stdout);
}

/**
 * @brief
 * This function generate a random board 
 **/
void generate_ia_board()
{
    // Construct an Empty board first
    construct_ia_game_board();
    srand(time(NULL));
    int position1_x = 0;
    int position1_y = 0;

    int position2_x = 0;
    int position2_y = 0;
    int error = 0;
    int b = 0;
    for (b = 0; b < 4; b++)
    {
        do
        {
            // Generate the first end of the boat
            position1_x = (rand() % 9) + 1;
            position1_y = (rand() % 9) + 1;

            // Generate the second end of the boat
            position2_x = (rand() % 9) + 1;
            position2_y = (rand() % 9) + 1;

            int size = length_of_ship(position1_y, position1_x, position2_y, position2_x);
            if (size != boats_size[b] || ia_game_board[position2_y][position2_x] != '-')
            {
                error = 1;
            }
            else
            {
                error = 0;

                // CHECK IF BETWEEN THE 2 POINTS IT IS EMPTY !
                if (position1_y == position2_y) //the boat is on a line
                {
                    if (position1_x < position2_x) //boat left to right
                    {
                        for (size_t j = position1_x; j <= position2_x; j++)
                        {
                            if (ia_game_board[position1_y][j] != '-')
                                error = 1;
                        }
                    }
                    else //boat right to left
                    {
                        for (size_t j = position2_x; j <= position1_x; j++)
                        {
                            if (ia_game_board[position1_y][j] != '-')
                                error = 1;
                        }
                    }
                }
                else //the boat is on a column
                {
                    if (position1_y < position2_y) //boat top to bot
                    {
                        for (size_t j = position1_y; j <= position2_y; j++)
                        {
                            if (ia_game_board[j][position1_x] != '-')
                                error = 1;
                        }
                    }
                    else //boat to top
                    {
                        for (size_t j = position2_y; j <= position1_y; j++)
                        {
                            if (ia_game_board[j][position1_x] != '-')
                                error = 1;
                        }
                    }
                }
            }

        } while (ia_game_board[position1_y][position1_x] != '-' || ia_game_board[position2_y][position2_x] != '-' || error == 1);

        // FILL BETWEEN THE LINES OF THE 2 POINTS
        if (position1_y == position2_y) //the boat is on a line
        {
            if (position1_x < position2_x) //boat left to right
            {
                for (size_t j = position1_x; j <= position2_x; j++)
                {
                    ia_game_board[position1_y][j] = boats_sign[b];
                }
            }
            else //boat right to left
            {
                for (size_t j = position2_x; j <= position1_x; j++)
                {
                    ia_game_board[position1_y][j] = boats_sign[b];
                }
            }
        }
        else //the boat is on a column
        {
            if (position1_y < position2_y) //boat top to bot
            {
                for (size_t j = position1_y; j <= position2_y; j++)
                {
                    ia_game_board[j][position1_x] = boats_sign[b];
                }
            }
            else //boat bot to top
            {
                for (size_t j = position2_y; j <= position1_y; j++)
                {
                    ia_game_board[j][position1_x] = boats_sign[b];
                }
            }
        }

        ia_game_board[position1_y][position1_x] = boats_sign[b];
        ia_game_board[position2_y][position2_x] = boats_sign[b];

        ia_boats_coordinates[b][0] = position1_x;
        ia_boats_coordinates[b][1] = position1_y;
        ia_boats_coordinates[b][2] = position2_x;
        ia_boats_coordinates[b][3] = position2_y;
    }
}

void make_ia_plays()
{
    srand(time(NULL));
    int position_x = 0;
    int position_y = 0;

    int error = 1;

    while (error == 1)
    {
        // Select random point
        position_x = (rand() % 10);
        position_y = (rand() % 10);

        // If cell is empty (-)
        if (game_board[position_y][position_x] == '-')
        {
            game_board[position_y][position_x] = 'X';
            error = 0;
        }
        // if cell is not empty
        else if (game_board[position_y][position_x] != '-')
        {
            // if cell not already visited (a X or a boat already shot)
            if (game_board[position_y][position_x] != 'X' && game_board_color[position_y][position_x] != 'r' && game_board[position_y][position_x] != 'x')
            {
                game_board_color[position_y][position_x] = 'r';
                error = 0;
            }
        }
    }
}
