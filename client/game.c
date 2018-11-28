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

void show_game_board()
{
    fflush(stdout); 
    printf("\n \n \n");
    printf("Your game board: \n \n");
    printf("  ");
    for (int j = 0; j < 10; j++)
    {
        printf(" ");
        printf("%c", (j+65)); // for UPPERCASE asci table
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

void put_ship(char* ship_type)
{
    char position_read[20];
    int position1_y;
    int position1_x;

    if (strcmp(ship_type, "destroyer") == 0)
    {
        printf("\n \n Pease indicate the first cell of the destroyer: \n");
    }

    scanf("%s", position_read);

    position1_y = atoi(&position_read[0]);
    position1_x = atoi(&position_read[1]);

    printf("you entered %d", position1_y);

    game_board[position1_y][position1_x] = 'D';


}