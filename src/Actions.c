#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Actions.h"
#include "players.h"
#include "Constants.h"

void RaiseBet(PLAYER *player, BOARD *board)
{
    int bet = 0;
    printf("Input the value the bet will be raised to: ");
	scanf("%d", &bet);

    playerMove(player, 3, bet, board);
}

void Call(PLAYER *player, BOARD *board)
{
    playerMove(player, 2, 0);
}

void Fold(PLAYER *player, BOARD *board)
{
    playerMove(player, 1, 0);
}

void PromptInput(PLAYER *player, BOARD *board)
{
    int input; //variable for input
if(player->fold == 0) //if the current player hasnt chosen to fold
{
    if (board->roundCounter > 0) //if the round has not ended yet
    {   
        

        printf("please enter your selection followed by an ENTER");
        printf("1 to fold, 2 to call, 3 to raise");
        printf("enter here: ");
        scanf("%d\n", &input);
        if (input == 1)
        {
            Fold(player);
            board->roundCounter -= 1; //decrement the round counter by 1
        }
        else if (input == 2)
        {
            Call(player);
            board->roundCounter -= 1; //decrement the round counter by 1

        }
        else if(input == 3)
        {
            RaiseBet(player);
            board->roundCounter == 4; //set round counter to 4. Start a new round of bet raising and calling
        }
        
    }
    else if(player->fold == 1)
    {
        printf("You have folded. Skip\n");
    }
    

}

}