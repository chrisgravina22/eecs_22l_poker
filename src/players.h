#ifndef PLAYERS_H
#define PLAYERS_H

#include <netinet/in.h>
#include <netdb.h>
#include "Constants.h"

#define maxLength 20

PLAYER *createPlayer(char name[20], struct sockaddr_in address, socklen_t length, int socketFD);
//delete player from the game
void deletePlayer (PLAYER *player);

//player makes a move
void playerMove (PLAYER *player, int input, int betAmount, BOARD *board);

#endif