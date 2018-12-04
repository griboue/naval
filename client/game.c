#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"


char game_board[10][10];

void construct_game_board()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            game_board[i][j] = '-';
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
    printf("Your game board: \n \n");
    printf("  ");
    for (int j = 0; j < 10; j++)
    {
        printf(" ");
        printf("%c", (j+65));         // for UPPERCASE asci table
    }
    printf("\n");
    for (int i = 0; i < 10; i++)
    {
        printf(" %d ", i);
        for (int j = 0; j < 10; j++)
        {
            printf("%c", game_board[i][j]);
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
void put_ship(char* ship_type, char* message)
{
    char position1_read[20];
    char position2_read[20];
    int position1_y;
    int position1_x;
    int position2_y;
    int position2_x;

    if (strcmp(ship_type, "destroyer") == 0)
    {
        printf("You have to put a DESTROYER on the board !");
    }

    printf("\n \n Pease indicate the FIRST cell of the ship:(ex. 'B6') \n");
    scanf("%s", position1_read);
    position1_x = position1_read[0]-65;     // convert ASCII CHAR LETTER to int (from 0 to 9 max)
    position1_y = position1_read[1]-48;     // convert ASCII CHAR NUMBER to int (from 0 to 9 max)

    printf("\n \n Pease indicate the LAST cell of the ship:(ex. 'B6') \n");
    scanf("%s", position2_read);
    position2_x = position2_read[0]-65;     // convert ASCII CHAR LETTER to int (from 0 to 9 max)
    position2_y = position2_read[1]-48;     // convert ASCII CHAR NUMBER to int (from 0 to 9 max)

    if (strcmp(ship_type, "destroyer") == 0)
    {
        game_board[position1_y][position1_x] = 'D';
        game_board[position2_y][position2_x] = 'D';
    }

    // message to send to the server (serialized in a string message)
    message[0] = position1_read[0];
    message[1] = position1_read[1];
    message[2] = '-';
    message[3] = position2_read[0];
    message[4] = position2_read[1];
    message[5] = '\0';

}

/**
 * @brief
 * This function returns the length between the points passed in parameters
 * Returns -1 if it is not possbile (ex: diagonals, outside the table ...)
 **/
int length_of_ship(int position1_y, int position1_x, int position2_y, int position2_x){
    if(position1_x!=position2_x && position1_y!=position2_y)     //diagonals
        return -1;
    if (position1_x==position2_x) { //the boat is on a line
        if (position1_y<position2_y)  //boat left to right
            return position2_x-position1_x;
        else  //boat right to left
            return position1_x-position2_x;
    }
    else { //the boat is on a column
        if (position1_x<position2_x) //boat top to bot
            return position2_y-position1_y;
        else //boat bot to top
            return position1_y-position2_y;
    }
}
