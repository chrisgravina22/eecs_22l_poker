#ifndef ACTIONS_H
#define ACTIONS_H
#include "Constants.h"
#include "players.h"

// define actions
void RaiseBet(PLAYER *player, BOARD *board);

void Fold(PLAYER *player, BOARD *board);

void Call(PLAYER *player, BOARD *board);

void PromptInput(PLAYER *player, BOARD *board);


#endif