#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "data.h"

char game_board[10][10];
char game_board_color[10][10];
char enemy_game_board[10][10];
char enemy_game_board_color[10][10];

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

    // for (size_t i = 0; i < number_boats; i++)
    for (size_t i = 0; i < 1; i++)
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
        boats_coordinates[i][0] = position1_read;
        boats_coordinates[i][1] = position2_read;

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
    if (position1_y == position2_y)
    {                                  //the boat is on a line
        if (position1_x < position2_x) //boat left to right
            return position2_x - position1_x + 1;
        else //boat right to left
            return position1_x - position2_x + 1;
    }
    else
    {                                  //the boat is on a column
        if (position1_y < position2_y) //boat top to bot
            return position2_y - position1_y + 1;
        else //boat bot to top
            return position1_y - position2_y + 1;
    }
}

int get_coordinates(char sign, char **message)
{
    for (size_t i = 0; i < number_boats; i++)
    {
        if (sign == boats_sign[i])
        {
            // free(message);
            // message = (char*)malloc(4*sizeof(char));
            message = boats_coordinates[i];
            return 0;
            break;
        }
    }
    return -1;
}
