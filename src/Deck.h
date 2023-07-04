#ifndef DECK_H
#define DECK_H
#include "Constants.h"
#include "players.h"




CARD *CreateCard(int suit, int rank);

void DeleteCard(CARD *card);

BOARD *CreateBoard(void);

int largest(int arr[7]);

int max(int num1, int num2);

int min(int num1, int num2);

void AssignPoints(PLAYER *player1);

PLAYER *CompareHands(PLAYER *p1, PLAYER *p2);

int CompHighCard(PLAYER *p1, PLAYER *p2);

int checkPair(PLAYER *player);

int FindElement(int array[52]);

void AssignPoints(PLAYER *player1);

int CompareOnePair(PLAYER *player1, PLAYER *player2);

int Check2Pairs(PLAYER *player);

int Comp2Pairs(PLAYER *player1, PLAYER *player2);

int CheckStraight(PLAYER *player1);

int CompStraight(PLAYER *player1, PLAYER *player2);

int CheckThreeCards(PLAYER *player);

int CompareThree1Kind(PLAYER *player1, PLAYER *player2);

int CheckFourCards(PLAYER *player);

int CompareFour1Kind(PLAYER *player1, PLAYER *player2);

int CheckFullHouse(PLAYER *player);

int CheckRoyalFlush(PLAYER *player);

int CompFullHouse(PLAYER *player1, PLAYER *player2);

int CheckFlush(PLAYER *player);

int EvaluateFlush(PLAYER *player);
#endif

