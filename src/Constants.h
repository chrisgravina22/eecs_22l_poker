#include <netinet/in.h>
#include <netdb.h>
#ifndef CONSTANTS_H
#define CONSTANTS_H

// define suits
#define CLUB 0
#define DIAMOND 1
#define HEART 2
#define SPADE 3

// define ranks
#define TWO 0
#define THREE 1
#define FOUR 2
#define FIVE 3
#define SIX 4
#define SEVEN 5
#define EIGHT 6
#define NINE 7
#define TEN 8
#define JACK 9
#define QUEEN 10
#define KING 11
#define ACE 12

#define MAXPLAYER 2
typedef struct Card{
	int suit;
	int rank;
} CARD;

typedef struct Player{
    struct PLAYER* prev;
    struct PLAYER *next;
    int points;
    char *name;
    int bet;
    struct sockaddr_in address;
    socklen_t length;
    int socketFD;
    int fold; //if fold is set to 1, the player will not compete with other players in the round
    int raiseConst; //raiseConst is turned to 1 when others make a raise
    int callConst; //callConst is turned to 1 when others make a call
    int dealer; //if dealer is set to 1, then this player is the dealer
    CARD* hand [2];
} PLAYER;

typedef struct Board
{
	CARD *cards[5]; //the communal cards, represented by a pointer array
	int bet; //the total bet of each round, called "pot"
    int roundCounter; //round counter for raise functions. 
    int CurrBet; //the current bet placed by that player
} BOARD;
typedef struct seat SEAT;
typedef struct table TABLE;

typedef struct seat
{
    TABLE* table;
    SEAT* prev;
    SEAT* next;
    PLAYER* player;
}SEAT;

typedef struct table
{
    SEAT* first;
    SEAT* last;
    int length;

}TABLE;
#endif
