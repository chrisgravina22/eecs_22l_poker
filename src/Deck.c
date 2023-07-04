#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Deck.h"
#include "players.h"
#include "Constants.h"


CARD *CreateCard(int suit, int rank)
{
	CARD *card = malloc(sizeof(CARD));
	card->suit = suit;
	card->rank = rank;

	return card;
}

void DeleteCard(CARD *card)
{
	free(card);
}

BOARD *CreateBoard(void)
{
	BOARD *board = malloc(sizeof(BOARD));
	board->bet = 0;
	return board;
	
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int largest(int arr[7])
{
    int i;
	int a;
     
    // Initialize maximum element
    int max = arr[0];
 
    // Traverse array elements
    // from second and compare
    // every element with current max
    for (i = 1; i < 7; i++)

	{

        if (arr[i] > max)
		{
            max = arr[i];
			a = i;
		}
	}
    return a;
}

int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

int min(int num1, int num2)
{
    return (num1 < num2 ) ? num1 : num2;
}

/* this function looks for the smallest NULL element to store the index that represents deal cards */
int FindElement(int array[])
{
	int i;
	/* increment i until array[i] points to NULL */
	for(i = 0; array[i] != 0 && i < 52; i ++)
	{
		//do nothing here
	}
	return i;
}

void AssignPoints(PLAYER *player1)
{
	
	if(checkPair(player1) > 0)
	{
		if (CheckFullHouse(player1) > 0)
		{
			player1->points = 6;
		}
		else if(Check2Pairs(player1) > 0)
		{
			player1->points = 2;
		}
		else if(CheckThreeCards(player1) > 0)
		{
			player1->points = 3;
		}
		else if(CheckFourCards(player1 ) > 0)
		{
			player1->points = 7;
		}
		else if(checkPair(player1) == 1)
		{
			player1->points = 1;
		}
	}
	else if(CheckFlush(player1) > 0)
	{
		if (CheckStraight(player1) > 0)
		{
			player1->points = 8;
		}
		else
		{
			player1->points = 5;
		}
	}
	else if(CheckFlush(player1) == 0 && CheckStraight(player1) > 0)
	{
		player1->points = 4;
	}
}

PLAYER *CompareHands(PLAYER *p1, PLAYER *p2)
{
	PLAYER *returnPlayer;
	AssignPoints(p1);
	AssignPoints(p2);
	int x;
	if(p1->points == p2->points)
	{
		if(p1->points == 1)
		{
			x = CompareOnePair(p1, p2);
		}
		else if(p1->points == 2)
		{
			x = Comp2Pairs(p1, p2);
		}
		else if(p1->points == 3)
		{
			x = CompareThree1Kind(p1, p2);
		}
		else if(p1->points == 4)
		{
			x = CompStraight(p1, p2);
		}
		else if(p1->points == 5)
		{
			x = (EvaluateFlush(p1) > EvaluateFlush(p2) ? 1 : 2);

		}
		else if(p1->points == 6)
		{
			x = CompFullHouse(p1, p2);
		}
		else if(p1->points == 7)
		{
			x = CompareFour1Kind(p1, p2);
		}
		else if(p1->points == 8)
		{
			x = CompStraight(p1, p2);
		}
		else if(p1->points == 0)
		{
			x = CompHighCard(p1, p2);
		}

		//compare the ranks depend on which combination of cards the player has
	}
	else if (p1->points > p2->points) //if player 1 has better combination
	{
		x = 1;
	}
	else if (p2->points > p1->points) //if player 2 has better combination
	{
		x = 2;
	}

	returnPlayer = (x == 1 ? p1 : p2); //if player 1 has better hand, return p1. Else return p2

	return returnPlayer;
}

int CompHighCard(PLAYER *p1, PLAYER *p2)
{
	int p1Rank[7];
	int p2Rank[7];

	for(int i = 0; i < 7; i ++)
	{
		p1Rank[i] = p1->hand[i]->rank;
	}

	for(int i = 0; i < 7; i ++)
	{
		p2Rank[i] = p2->hand[i]->rank;
	}

	qsort(p1Rank, 7, sizeof(int), cmpfunc);
	qsort(p2Rank, 7, sizeof(int), cmpfunc);
	int x = p1Rank[6];
	int y = p2Rank[6];
	int returnvar = (x > y ? 1 : 2);

	return returnvar;

}

//this function returns the number of pairs within a player's hand
int checkPair(PLAYER *player)
{
	int Pair = 0;

	// determine whether there is a pair or not

  
  	for(int i = 0; i < 6; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank)
				{
        		Pair += 1;
				}

	  	}
	}
	int x = (Pair > 0 ? 1 : 0);
	return x;
}

int CompareOnePair(PLAYER *player1, PLAYER *player2)
{
	// temporary variables for storing the players' ranks
	int rank1;
	int rank2;

	for (int i = 0; i < 5; i++)
	{
		int tmp = i + 1; //create a tmp variable that equals to the index of the player's hand + 1;
		if(tmp == 6)
		{
			tmp = 5; //if the temporary variable = 6, set it to 5
		}

		for(int j = tmp; j < 6; j++)
		{
			if(player1->hand[i]->rank == player1->hand[j]->rank)
			{
				rank1 = player1->hand[i]->rank; //Pair = 1 indicates that there is at least a pair in the player's hand
				break; // jump out of the for loop "for(int j = tmp; j < 6; j++)"
			}
		}

	}

	for (int i = 0; i < 5; i++)
	{
		int tmp = i + 1; //create a tmp variable that equals to the index of the player's hand + 1;
		if(tmp == 6)
		{
			tmp = 5; //if the temporary variable = 6, set it to 5
		}

		for(int j = tmp; j < 6; j++)
		{
			if(player2->hand[i]->rank == player2->hand[j]->rank)
			{
				rank2 = player2->hand[i]->rank; //Pair = 1 indicates that there is at least a pair in the player's hand
				break; // jump out of the for loop "for(int j = tmp; j < 6; j++)"
			}
		}

	}

if(rank2 > rank1)
	{
		return 2;
	}
else if(rank2 > rank1)
{
	return 1;
}
	return 0; //2 players have equal ranks. If they are the winner, they split the pot
	
	
}

int Check2Pairs(PLAYER *player)
{
	int Pair = 0;
	int TmpRank = 0; //temp variable for storing the player's rank
	int TwoPair = 0; //variable indicating whether there is a two-pair in the player's hand
	for(int i = 0; i < 6; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank)
				{
        		Pair += 1;
				TmpRank = player->hand[i]->rank;
				}

	  	}
	}

	if (Pair == 1)
	{
		for(int i = 0; i < 6; i++)
  		{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank && player->hand[i]->rank != TmpRank)
				{
        		TwoPair += 1; //TwoPair = 1 means that there are exactly two pairs. 
				}
	  		}
		}

	}

	return TwoPair; //if TwoPair > 0, then there are two pairs. 

}

int Comp2Pairs(PLAYER *player1, PLAYER *player2)
{
	int Pair = 0;
	int Pair1Rank1 = 0; //temp variable for storing the player one's rank
	int Pair1Rank2 = 0;
	int Pair2Rank1 = 0;
	int Pair2Rank2 = 0;

	int p1Rank[7];
	int p2Rank[7];

	for(int i = 0; i < 7; i ++)
	{
		p1Rank[i] = player1->hand[i]->rank;
	}

	for(int i = 0; i < 7; i ++)
	{
		p2Rank[i] = player2->hand[i]->rank;
	}

	qsort(p1Rank, 7, sizeof(int), cmpfunc); //sort
	qsort(p1Rank, 7, sizeof(int), cmpfunc); //sort

	for(int i = 0; i < 6; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(p1Rank[i] == p1Rank[j] && Pair == 0)
				{
        		Pair = 1; //set Pair to 1. No more rank will be stored into the temporary variable Pair1Rank1
				Pair1Rank1 = p1Rank[i];
				}

	  	}
	}

	
	for(int i = 0; i < 6; i++)
  	{  
    for(int j = i+1; j < 7; j++){
      	if(p1Rank[i] == p1Rank[j] && p1Rank[i] != Pair1Rank1)
			{
    		Pair1Rank2 = p1Rank[i];
			}
		}
	}

Pair = 0; //reinitialize Pair

	for(int i = 0; i < 6; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(p2Rank[i] == p2Rank[j] && Pair == 0)
				{
        		Pair = 1; //set Pair to 1. No more rank will be stored into the temporary variable Pair1Rank1
				Pair2Rank1 = p2Rank[i];
				}

	  	}
	}

	
	for(int i = 0; i < 6; i++)
  	{  
    for(int j = i+1; j < 7; j++){
      	if(p2Rank[i] == p2Rank[j] && p2Rank[i] != Pair1Rank1)
			{
    		Pair2Rank2 = p2Rank[i];
			}
		}
	}
	int big1;
	big1 = max(Pair1Rank1, Pair1Rank2);
	int big2;
	big2 = max(Pair2Rank1, Pair2Rank2);
	int small1, small2;
	small1 = min(Pair1Rank1, Pair1Rank2);
	small2 = min(Pair2Rank1, Pair2Rank2);

	int returnvar = 0;
	returnvar = (big1 > big2 ? 1 : 2);
	if (returnvar == 0)
	{
		returnvar = (small1 > small2 ? 1 : 2);
	}

	return returnvar;
}

int CheckStraight(PLAYER *player1)
{
	int hand1[7];
	
	int straight1[5];//array of 5 storing the ranks of cards that form "Straight"
	//stright1[0] should have the largest rank

	
	for (int i = 0; i < 7; i++)
	{
		hand1[i] = player1->hand[i]->rank;
	}
	qsort(hand1, 7, sizeof(int), cmpfunc); //sort
	int x = 0;
	for(int i = 6; i < 0; i--)
  	{  
    	for(int j = 7; j < i+1; j--){
      		if(player1->hand[i]->rank + 1 == player1->hand[j]->rank && x >= 4) 
			//if the last element is greater than the second-to-last element by 1
			// AND if the index of the array storing the ranks is within the range of 0,1,2,3,4
				{
        		straight1[x] = player1->hand[j]->rank;
				straight1[x + 1] = player1->hand[i]->rank;
				}

	  	}
		x += 1;//increment the index;

	}

	int returnVar = 0;//variable for the function return. Initially set to zero

	if(straight1[0] != 0)
	{
	returnVar = straight1[0];
	}

	return returnVar;
}

int CompStraight(PLAYER *player1, PLAYER *player2)
{
	int p1 = CheckStraight(player1);
	int p2 = CheckStraight(player2);
	if (p1 > p2)
	{
		return 1;
	}
	return 2;
}
int CheckThreeCards(PLAYER *player)
{
	int Triple = 0;

	// determine whether there is a triple or not

  
  	for(int i = 0; i < 5; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank && player->hand[j]->rank == player->hand[j + 1]->rank)
				{
        		Triple += 1;
				}

	  	}
	}
	int x = (Triple >= 1 ? 1 : 0 ); //is there at least one triple or not. If yes, return 1
	return Triple;
}

// compare the rank of the card that forms "three of a kind"
int CompareThree1Kind(PLAYER *player1, PLAYER *player2)
{
	int rank1 = 0;
	int rank2 = 0;
	for(int i = 0; i < 5; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player1->hand[i]->rank == player1->hand[j]->rank && player1->hand[j]->rank == player1->hand[j + 1]->rank)
				{
        		rank1 = player1->hand[i]->rank;
				}

	  	}
	}

	for(int i = 0; i < 5; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player2->hand[i]->rank == player2->hand[j]->rank && player2->hand[j]->rank == player2->hand[j + 1]->rank)
				{
        		rank2 = player2->hand[i]->rank;
				}

	  	}
	}

	if(rank1 > rank2)
	{
		return 1;
	}
	else if(rank2 > rank1)
	{
	return 2;
	}
	return 0; //they have the same rank. They should split the pot
}

int CheckFourCards(PLAYER *player)
{
	int FourSame = 0;

	// determine whether four cards have the same ranks or not

  
  	for(int i = 0; i < 4; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank && player->hand[j]->rank == player->hand[j + 1]->rank)
				{
        		FourSame += 1;
				}

	  	}
	}
	
	int x = (FourSame >= 1 ? 1 : 0);
	return FourSame;
}

int CompareFour1Kind(PLAYER *player1, PLAYER *player2)
{
	int rank1 = 0;
	int rank2 = 0;
	for(int i = 0; i < 4; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player1->hand[i]->rank == player1->hand[j]->rank && player1->hand[j]->rank == player1->hand[j + 1]->rank)
				{
        		rank1 = player1->hand[i]->rank;
				}

	  	}
	}

	for(int i = 0; i < 3; i++)
  	{  
    	for(int j = i+1; j < 6; j++){
      		if(player2->hand[i]->rank == player2->hand[j]->rank && player2->hand[j]->rank == player2->hand[j + 1]->rank)
				{
        		rank2 = player2->hand[i]->rank;
				}

	  	}
	}

	int returnvar = 0;
	if(rank1 > rank2)
	{
		returnvar = 1;
	}
	else if(rank2 > rank1)
	{
	returnvar = 2;
	}
	return returnvar;
}



int CheckFullHouse(PLAYER *player)
{
	int Triple = 0; //triple = 1 means there is a three-of-a-kind
	int TmpRank = 0;
	int Pair = 0; //Pair = 1 means there is also a pair on top of a three of a kind

	// determine whether there is a triple or not

  
  	for(int i = 0; i < 5; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank && player->hand[j]->rank == player->hand[j + 1]->rank)
				{
        		Triple = 1;
				TmpRank = player->hand[i]->rank;
				}

	  	}
	}
if(Triple == 1)
{
	for(int i = 0; i < 6; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player->hand[i]->rank == player->hand[j]->rank && player->hand[i]->rank != TmpRank)
				{
        		Pair = 1; //pair set to 1 if there also exists a pair
				}

	  	}
	}
}
	return Pair; //if Pair = 1, the hand contains a full house
}

int CompFullHouse(PLAYER *player1, PLAYER *player2)
{
	//rank of the triple
	int TmpRank1 = 0;
	int TmpRank2 = 0;
	//Rank of the pairs
	int tmpPairRank1 = 0; 
	int tmpPairRank2 = 0;
	int PairRank1 = 0;
	int PairRank2 = 0;

	// determine whether there is a triple or not

  
  	for(int i = 0; i < 5; i++)
  	{  
    	for(int j = i+1; j < 7; j++){
      		if(player1->hand[i]->rank == player1->hand[j]->rank && player1->hand[j]->rank == player1->hand[j + 1]->rank)
				{
        		
				TmpRank1 = player1->hand[i]->rank;
				}

	  	}
	}

	for(int i = 0; i < 5; i++)
  	{  
		  
    	for(int j = i+1; j < 7; j++){
      		if(player2->hand[i]->rank == player2->hand[j]->rank && player2->hand[j]->rank == player2->hand[j + 1]->rank)
				{
        		
				TmpRank2 = player2->hand[i]->rank;
				}

	  	}
	}

	if (TmpRank1 > TmpRank2)
	{
		/* if the three cards of player 1 outranks those of player 2*/
		return 1;
	}
	else if(TmpRank2 > TmpRank1)
	{
		return 2;
	}
	else
	{
		for(int i = 0; i < 6; i++)
  		{  
    	for(int j = i+1; j < 7; j++){
      		if(player1->hand[i]->rank == player1->hand[j]->rank && player1->hand[i]->rank != TmpRank1 && tmpPairRank1 == 0)
			{
        		tmpPairRank1 = player1->hand[i]->rank;
			}

			else if(player1->hand[i]->rank == player1->hand[j]->rank && player1->hand[i]->rank != TmpRank1 && tmpPairRank1 != 0)
			{
				tmpPairRank2 = player1->hand[i]->rank;
			}

			

	  	}
		}

	PairRank1 = max(tmpPairRank1, tmpPairRank2);

	//reset temporary rank of pairs to zero
	tmpPairRank1 = 0;
	tmpPairRank2 = 0;

		for(int i = 0; i < 6; i++)
  		{  
    	for(int j = i+1; j < 7; j++){
      		if(player2->hand[i]->rank == player2->hand[j]->rank && player2->hand[i]->rank != TmpRank2 && tmpPairRank1 == 0)
			{
        		tmpPairRank1 = player2->hand[i]->rank;
			}
			else if (player2->hand[i]->rank == player2->hand[j]->rank && player2->hand[i]->rank != TmpRank2 && tmpPairRank1 != 0)
			{
				tmpPairRank2 = player2->hand[i]->rank;
			}

	  	}
		}
	//the pair with higher rank would be chosen
	PairRank2 = max(tmpPairRank1, tmpPairRank2);


		if(PairRank1 > PairRank2)
		{
			return 1;
		}
		else if(PairRank2 > PairRank1)
		{
			return 2;
		}
		else 
		{
			return 0; //the two have equal ranks for their pairs. They split the pot
		}


	}
	
}

int CheckFlush(PLAYER *player)
{
	int p1Suit[7];
	
	for(int i = 0; i < 7; i ++)
	{
		p1Suit[i] = player->hand[i]->suit;
	}

	qsort(p1Suit, 7, sizeof(int), cmpfunc); //sort the player's rank array. The card with the highest rank should be the 7th element of the array

	int FlushCnt = 0; //index for p1Flush

	int Compared = 1;
	for (Compared = 0; Compared < 4; Compared++) //compare the player's rank against each of the rank
	{
		for(int i = 7; i < 0; i--)
		{
			if(Compared == p1Suit[i] && FlushCnt <= 4)
			{
				
				FlushCnt++; //to the next element of the array
			}
		}

		if (FlushCnt >= 5)
		{
			break;
		}
		else
		{
			FlushCnt = 0; //set Flush to 0;
		}
			

	}

	int x = (FlushCnt >= 5 ? 1 : 0); //if Flush counter is greater or equal to 5, return 1. Else return 0. 
	return x;
}

int EvaluateFlush(PLAYER *player)
{
	int p1Suit[7];
	int p1Flush[5];

	for(int i = 0; i < 7; i ++)
	{
		p1Suit[i] = player->hand[i]->suit;
	}

	qsort(p1Suit, 7, sizeof(int), cmpfunc); //sort the player's suit array. The card with the highest rank should be the 7th element of the array

	int FlushIndex = 0; //index for p1Flush
	int Compared = 0;
	for (Compared = 0; Compared < 4; Compared++)
	{
		for(int i = 7; i < 0; i--)
		{
			if(Compared == p1Suit[i] && FlushIndex <= 4)
			{
				p1Flush[FlushIndex] = p1Suit[i];
				FlushIndex ++; //to the next element of the array
			}
		}
		
	}

	int x = p1Flush[0]; //return the greatest rank of the flush

	return x;
	
}