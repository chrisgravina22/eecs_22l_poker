#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Constants.h"


#include "players.h"
#include "Deck.h"
//Sends messages to all players telling them the server has shut down
void ShutdownServer(TABLE *table, char *argv);

// receives shutdown message from client then closes the server
void CheckShutdown(char* message, TABLE *table);

//reset all folds and bets from before
void ResetTable(TABLE *table);

//gives the winner the pot and deducts bets from the loser
//also sets players with zero points to null
void RewardPot(BOARD *board, TABLE *table);

//get bets from clients for that round
int getBets(TABLE *table);

//gives each player a pair of cards
int DealCards(CARD *deck[52], TABLE *table);

//Card value to string value formatted as <rank>-<suit>
char* CardToString(CARD *card);

//Just an error thrower
void FatalError(const char *Program, const char *ErrorMsg);

//Shuffles the deck of cards
void ShuffleDeck(CARD* deck[52]);

//picks the next three cards on the deck to make the flop
int DistributeFlop(BOARD *board, CARD *deck[52], int startingIndex);

//sends cards from any array arr[] to all players
int SendCardsToClient(CARD *arr[], TABLE *table, char* message);

//makes the dealer the next player
void RotateRoles(TABLE *table);

//returns a pointer to a an entry at a list TABLE. Already updates the table
SEAT *CreateSeat(PLAYER *player, TABLE *table);

//Returns a double linked list that contains all players in game
TABLE *CreateTable();

//waits for the ok from the client that it got the message
void WaitForResponse(TABLE *table);

int AskSeat(PLAYER *players[4], int socketFD);

int main(int argc, char *argv[])
{
	BOARD *board = CreateBoard();
	TABLE *table = CreateTable();
	PLAYER *players[4];
	for(int i = 0; i < 4; i++)
	{
		players[i] = NULL;
	}
    // initialize random number generator 
    srand(time(NULL));
	//declaring cards and assigning them to a position in the deck
	CARD *deck[52];
	int index = 0;
	for(int suit = 0; suit <=3; suit++)
	{
		for(int value = 0; value <= 12; value++)
		{
			deck[index]  = CreateCard(suit, value);
			index++;
		}
	}
	/*Print all cards for testing purposes*/
	/*
	for(int i = 0; i < 52; i++)
	{
		printf("the suit is %d and the rank is %d\n", deck[i]->suit, deck[i]->rank);
	}
	*/
	
	int l, n;
	int ServSocketFD,	// socket file descriptor for service 
		DataSocketFD,	// socket file descriptor for data 
		PortNo;		// port number 
	socklen_t ClientLen;
	struct 	sockaddr_in ServerAddress,	// server address (this host) 
						ClientAddress;	// client address we connect with 
	char RecvBuf[256];	// message buffer for receiving a message 
	char SendBuf[256];
	int Bye = 0,
	Shutdown = 0;

    printf("%s: Starting...\n", argv[0]);
    if (argc < 2)
    {   
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(10);
	}
    PortNo = atoi(argv[1]);	// get the port number 
    if (PortNo <= 2000)
    {   
		fprintf(stderr, "%s: invalid port number %d, should be greater 2000\n",
		argv[0], PortNo);
        exit(10);
    }
    ServSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   
		FatalError(argv[0], "service socket creation failed");
    }
    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(ServSocketFD, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {   
		FatalError(argv[0], "binding the server to a socket failed");
    }
    printf("%s: Listening on port %d...\n", argv[0], PortNo);
    if (listen(ServSocketFD, 5) < 0)	// max 5 clients in backlog 
    {   
		FatalError(argv[0], "listening on socket failed");
    }
	int start = 0;
	int playerCount = 0;
	PLAYER *player;
	SEAT *seat;
	//assigning players
    do{ 
		strcpy(RecvBuf, "");
		Shutdown = 0;
		ClientLen = sizeof(ClientAddress);
		DataSocketFD = accept(ServSocketFD, (struct sockaddr*)&ClientAddress, &ClientLen);
		if (DataSocketFD < 0)
		{   FatalError(argv[0], "data socket creation (accept) failed");
		}
		n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
		CheckShutdown(RecvBuf, table);
		if (n < 0) 
		{   
			FatalError(argv[0], "reading from data socket failed");
		}
		
		RecvBuf[n] = 0;
		printf("%s: Received message: %s\n", argv[0], RecvBuf);
		player = createPlayer(RecvBuf, ClientAddress, ClientLen, DataSocketFD);
		int spot = AskSeat(players, DataSocketFD);
		players[spot] = player;
		printf("%s: Accepted connection from client.\n", argv[0]);
		if(playerCount == 0)
		{
			playerCount++;
			continue;
		}
		
		//send message to host to see if they wanna start the game
		strcpy(SendBuf, "CONNECTION-");
		strcat(SendBuf, player->name);
		strcat(SendBuf, "-");
		char spotString[2];
		sprintf(spotString, "%d", spot);
		strcat(SendBuf, spotString);
		printf("%s\n", SendBuf);
		SendBuf[sizeof(SendBuf)-1] = 0;
		l = strlen(SendBuf);
	    printf("%s: Sending response: %s.\n", argv[0], SendBuf);
	    n = write(players[0]->socketFD, SendBuf, l);
	    if (n < 0)
	    {   
			FatalError(argv[0], "writing to data socket failed");
	    }
		
		n = read(players[0]->socketFD, RecvBuf, sizeof(RecvBuf)-1);
		if (n < 0) 
		{   
			FatalError(argv[0], "reading from data socket failed");
		}
		
		RecvBuf[n] = 0;
		printf("%s: Received message: %s\n", argv[0], RecvBuf);
		int saidYes = strcmp(RecvBuf, "y");
		if (saidYes == 0)
	    {
			start = 1;
		}
		
		playerCount++;
    } while(playerCount < MAXPLAYER && !start);

	for(int i = 0; i< 4; i++)
	{
		if(players[i]!=NULL)
		{
			CreateSeat(players[i], table);
		}
	}

	while(playerCount>1)
	{
		board->bet = 0;
		//shuffling the deck of cards
		ShuffleDeck(deck);
		// for(int i = 0; i < 52; i++)
		// {
		// 	printf("the suit is %d and the rank is %d\n", deck[i]->suit, deck[i]->rank);
		// }

		int pointer = DealCards(deck, table);
		WaitForResponse(table);
		board->bet += getBets(table);
		//Pre flop
		//board->bet += getBets(table);

		//FLOP
		pointer = DistributeFlop(board, deck, pointer);

		SendCardsToClient(board->cards, table, "FLOP-");
		WaitForResponse(table);
		board->bet += getBets(table);

		//TURNER
		board->cards[3] = deck[pointer];
		pointer++;
		SendCardsToClient(board->cards, table, "TURNER-");
		WaitForResponse(table);
		board->bet += getBets(table);

		//RIVER
		board->cards[4] = deck[pointer];
		SendCardsToClient(board->cards, table, "RIVER-");
		WaitForResponse(table);
		board->bet += getBets(table);
		
		//compares hand and rewards pot to winner
		//could someone add code to display a message showing a player is out when they're set to NULL?
		RewardPot(board, table);
		//rotates the dealer
		RotateRoles(table);
		playerCount = 0;
	}
	
	ShutdownServer(table, argv[0]);
}

int AskSeat(PLAYER *players[4], int socketFD)
{
	char* SendBuf = malloc(100);
	char* RecBuf = malloc(200);
	int n, l;
	int spot;
	strcpy(SendBuf, "SEAT-");
	for(int i = 0; i < 4; i++)
	{
		if(players[i] != NULL)
		{
			continue;
		}
		char *tempInt = malloc(40);
		sprintf(tempInt, "%d", i);
		if(i<3){strcat(tempInt, "-");}
		strcat(SendBuf, tempInt);
	}
	SendBuf[strlen(SendBuf)] = 0;
	l = strlen(SendBuf);
	printf("%s of size %d\n", SendBuf, l);
	do{
		n = write(socketFD, SendBuf, l);
		//printf("%s\n", SendBuf);
		if (n < 0)
		{   
			FatalError("ask seat", "writing to data socket failed");
		}
		//l = strlen(RecBuf);
		//printf("%d\n", l);
		n = read(socketFD, RecBuf, 1);
		printf("%s\n", RecBuf);
		spot = atoi(RecBuf);
		//check if the spot is taken
		for(int i = 0; i < 5; i++)
		{
			if(players[i] != NULL && spot == i)
			{
				spot = -1;
				break;
			}
		}
	}while(spot < 0 && spot>= 4);

	return spot;




}
// checks if shutdown message is sent by client
void CheckShutdown(char* message, TABLE *table)
{
	if(strcmp(message, "shutdown") == 0)
	{
		ShutdownServer(table, "stop");
	}
}

//Should only be called when player Count is one
void ShutdownServer(TABLE *table, char *argv)
{
	int n, l;
	char SendBuf[256];	// message buffer for sending a response 
	strncpy(SendBuf, "server shutdown", sizeof(SendBuf)-1);
	SendBuf[sizeof(SendBuf)-1] = 0;
	l = strlen(SendBuf);
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	int counter = 0;
	while(counter < table->length)
	{
		if(!seat->player)
		{
			continue;
		}
		int n = write(seat->player->socketFD, SendBuf, l);
		close(seat->player->socketFD);
		seat = nextSeat;
		nextSeat = seat->next;
	}
    printf("%s: Shutting down.\n", argv);

}

void ResetTable(TABLE *table)
{
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	//go through the list of players seated at the table
	for(int i =0; i < table->length; i++)
	{
		if(seat->player == NULL)//if there is no player at the seat (if someone disconnected, for example), loop again
		{
			seat = nextSeat;
			nextSeat = seat->next;
			continue;
		}
		//reset the values of fold and bet
		seat->player->fold = 0;
		seat->player->bet = 0;
		seat = nextSeat;
		nextSeat = seat->next;
	}	

}
//get bets from players
int getBets(TABLE *table)
{
	int roundBet = 0;
	char* message = malloc(7);
	char* response = malloc(40);
	strcpy(message, "GETBET-");
	int l;
	message[strlen(message)] = 0;
	l = strlen(message);
	int n = 0;
	int minbet = 0;
	//loop through players
	int evenBets = 0;
	int PlayerBet = 0;
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	int counter = 0;
	while(counter < table->length)
	{
		//if no player or player has folded, skip 
		if(seat->player == NULL || seat->player->fold)
		{
			seat = nextSeat;
			nextSeat = seat->next;
			counter++;
			continue;
		}
		//asks the client for a bet. If it is less than the bet already on the table and more than 0, repeat.
		//If bets havent started and the player wants to check, he/she may check by sending a 0
		//Negative bets are counted as a fold
		strcpy(message, "GETBET-");
		char betMessage[10];
		sprintf(betMessage, "%d", minbet);
		strcat(message, betMessage);
		printf("%s\n", message);
		do{
			l = strlen(message);
			n = write(seat->player->socketFD, message, l);

			if(n<0)
			{
				FatalError("GETBET","Failed to get bets");
			}
			n = read(seat->player->socketFD, response, 40);
			printf("received %s\n", response);
			CheckShutdown(response, table);
			if(n<0)
			{
				FatalError("GETBET","Failed to get bets");
			}
			PlayerBet = atoi(response);
		}while(PlayerBet < minbet && PlayerBet > 0 );

		if(PlayerBet > minbet)
		{
			if(minbet>0)
			{
				counter = 0;
			}
			minbet = PlayerBet;
		}
		if(PlayerBet < 0)
		{
			seat->player->fold = 1;
		}
		//adding to the players total bet;
		seat->player->bet += PlayerBet;
		roundBet += PlayerBet;
		seat = nextSeat;
		nextSeat = seat->next;
		counter++;
	}
	free(message);
	free(response);
	return roundBet;
}
//returns the amount of cards dealt, just to keep as a pointer
int DealCards(CARD *deck[52], TABLE *table)
{
	int  index = 0;
	int  l;
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	int counter = 0;
	for(int i = 0; i < table->length; i++)
	{
		if(seat->player == NULL)
		{
			seat = nextSeat;
			nextSeat = seat->next;
			continue;
		}
		seat->player->hand[0] = deck[index];
		seat->player->hand[1] = deck[index+1];
		seat = nextSeat;
		nextSeat = seat->next;
		index +=2;
	}

	while(counter < table->length)
	{
		if(seat->player == NULL)
		{
			seat = nextSeat;
			nextSeat = seat->next;
			counter++;
			continue;
		}
		char* message = malloc(40);
		strcpy(message, "HAND-");
		char *first = CardToString(seat->player->hand[0]);
		char *second = CardToString(seat->player->hand[1]);
		strcat(first, "-");
		strcat(first, second);
		strcat(message, first);
		l = strlen(message);
		printf("%s", message);
		write(seat->player->socketFD, message, l);
		seat = nextSeat;
		nextSeat = seat->next;
		counter++;
	}
	return index;
}
//card <<rank>:<suit>>
char* CardToString(CARD *card)
{
	char *output = malloc(40);
	sprintf(output, "%d:%d", card->rank, card->suit);
	output[sizeof(output)-1] = 0;
	return output;
}
void FatalError(const char *Program, const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!", stderr);
    exit(20);
} /* end of FatalError */
//Function to shuffle the array that will dictate the order of the shuffled pieces
void ShuffleDeck(CARD* deck[52])
{
	int first, second;
	CARD *temp;
	for(int i = 0; i < 100; i++)
	{
		//get two random elements in the int[] array and switch them
		first = rand()%52;
		second = rand()%52;
		temp = deck[first];
		deck[first] = deck[second];
		deck[second] = temp;
	}	
}

int DistributeFlop(BOARD *board, CARD *deck[52], int startingIndex)
{
	for(int i = 0 ; i <3; i++)
	{
		board->cards[i] = deck[startingIndex];
		startingIndex++;
	}
	return startingIndex;
}
//arr is any array of cards of any length, players is the player array.
//this function sends the clients cards from arr[]
int SendCardsToClient(CARD *arr[], TABLE *table, char* m)
{
	char *message = malloc(255);
	strcpy(message, m);
	int card = 0;
	while(arr[card])
	{
		char *strCard = CardToString(arr[card]);
		strcat(message, strCard);
		strcat(message, "-");
		card++;
	}
	
	
	int l = strlen(message);
	message[l-1] = 0;
	
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	printf("\n%s\n", message);
	int counter = 0;
	while(counter < table->length)
	{
		if(seat->player != NULL)
		{
			write(seat->player->socketFD, message, l);
		}
		seat = nextSeat;
		nextSeat = seat->next;
		counter++;
	}

	return 0;
}


//function to rotate the values of 
void RotateRoles(TABLE *table)
{
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	int counter;
	while(counter < table->length)
	{
		if(seat->player != NULL && !seat->player->dealer)
		{
			seat->player->dealer = 0;
			seat->next->player->dealer = 1;
			break;
		}
		seat = nextSeat;
		nextSeat = seat->next;
		counter++;
	}
}

//double linked list that will hold the seats available
TABLE *CreateTable()
{
	TABLE *table = malloc(sizeof(TABLE));
	table->length = 0;
	table->first = NULL;
	table->last = NULL;

	return table;
}
//returns a seat to a player that is already ready to go 
//also updates table
SEAT *CreateSeat(PLAYER *player, TABLE *table)
{
	SEAT *seat = malloc(sizeof(SEAT));
	seat->table = table;
	seat->player = player;
	if(table->length > 0)
	{
		seat->prev = table->last;
		table->last->next = seat;
		seat->next = table->first;
		table->length++;
	}
	else if(table->length == 0)
	{
		table->first = seat;
		table->last = seat;
		table->length++;
	}
	return seat;
}

//rewards the pot to the winner
//compares the hands to determine the winner
//also deducts bets from the losers and sets them to NULL if their balance becomes 0
void RewardPot(BOARD *board, TABLE *table)
{
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;

	PLAYER *winner = seat->player;
	int counter = 0;
	while(counter < table->length-1)
	{
		//if no player or player has folded, skip 
		if(seat->player == NULL || seat->player->fold)
		{
			counter++;
			continue;
		}

		winner = CompareHands(winner, nextSeat->player);
		seat = nextSeat;
		nextSeat = seat->next;
	}

	//give pot to winner
	winner->points += board->bet;

	seat = table->first;
	nextSeat = seat->next;
	//go through the list of players seated at the table
	for(int i =0; i < table->length; i++)
	{
		if(seat->player == NULL || seat->player == winner)//if there is no player at the seat or they are the winner (if someone disconnected, for example), loop again
		{
			seat = nextSeat;
			nextSeat = seat->next;
			continue;
		}
		//deduct losing player's bet
		seat->player->points -= seat->player->bet;
		//if player's balance is empty, they leave the game
		if(seat->player->points == 0)
		{
			seat->player = NULL;
		}
		seat = nextSeat;
		nextSeat = seat->next;
	} 
}

void WaitForResponse(TABLE *table)
{
	int n = 0;
	char *response = malloc(3);
	int l = strlen(response);
	SEAT *seat = table->first;
	SEAT *nextSeat = seat->next;
	for(int i =0; i < table->length; i++)
	{
		if(seat->player == NULL)
		{
			continue;
		}
		do{
		n = read(seat->player->socketFD, response, 2);
		printf("response %s\n", response);
		CheckShutdown(response, table);
		}while(strcmp(response, "ok"));
		seat = nextSeat;
		nextSeat = seat->next;
	}
}