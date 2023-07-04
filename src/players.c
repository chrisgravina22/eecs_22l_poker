#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include <netdb.h>

#include "players.h"
#include "Deck.h"
#include "Constants.h"

PLAYER *createPlayer(char name[20], struct sockaddr_in address, socklen_t length, int socketFD){
    PLAYER *player = malloc(sizeof(PLAYER));
    player->prev = NULL;
    player->next = NULL;
    player->points = 1000;
    player -> name = name; /* setting to "test" for testing */
    player->address = address;
    player->length = length;
    player->socketFD = socketFD; 
    player -> bet = 0;
    player->fold = 0;
    player->raiseConst = 0;
    player->callConst = 0;
    player->dealer = 0;

    for(int i = 0; i < 7; i++)
    {
    player->hand[i] = malloc( sizeof(CARD)); //allocate space
    
    }

    return player;
}

void deletePlayer(PLAYER *player){
    free(player);
}

/* this function handles the player's move.*/
/* different integer numbers stand for different moves */
void playerMove (PLAYER *player, int input, int betAmount, BOARD *board)
{
    if (input == 1)
    //if the player chooses to fold
    {
        player->fold = 1;
        player->bet = 0;
    }
    else if(input == 2)
    //if the player makes a call
    {
        player->callConst = 1;//callConst = 1 indicates that the player has made a call
        player->bet = board->CurrBet; //the player's bet equals to the previous player's bet
    }
    //if the player makes a raise
    else if(input == 3)
    {
        player->raiseConst = 1;
        player->bet = betAmount; //the player's bet should be the amount of bet he set
        board->CurrBet = player->bet; //store player's bet into the current bet of the board
    }
}