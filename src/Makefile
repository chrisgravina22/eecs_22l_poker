# Variables
CC = gcc -g
DEBUG = -DDEBUG=1 -g
CFLAGS = -Wall -std=c11 -c
LFLAGS = -Wall
GTKINC	= `pkg-config --cflags gtk+-2.0`
GTKLIBS	= `pkg-config --libs gtk+-2.0`

####################### Default Target ##############################
all: Poker_Server Poker_Client

####################### Generate Object Files #######################
Poker_Server.o: Poker_Server.c Deck.c Deck.h Constants.h
	$(CC) $(CFLAGS) Poker_Server.c -o Poker_Server.o

Deck.o: Deck.c Deck.h players.h
	$(CC) $(CFLAGS) Deck.c -o Deck.o

players.o: players.c players.h
	$(CC) $(CFLAGS) players.c -o players.o

Poker_Client.o: Poker_Client.c
	$(CC) Poker_Client.c -Wall -std=c11 -O2 $(GTKINC) -c -o Poker_Client.o

####################### Generate the Executable #####################
Poker_Server: Poker_Server.o Deck.o players.o
	$(CC) Poker_Server.o Deck.o players.o -o Server
	mv Server ../bin

Poker_Client: Poker_Client.o Deck.o players.o
	$(CC) Poker_Client.o Deck.o players.o -Wall -std=c11 -O2 $(GTKLIBS) -lm -o Client
	mv Client ../bin

####################### Test ########################################
GUI_Test: Poker_Client.o
	$(CC) Poker_Client.o -Wall -std=c11 -O2 $(GTKLIBS) -lm -o GUI_Test

####################### Others ######################################
# Target for clean-up
clean:
	rm -f *.o
	rm -f Client
	rm -f Server

# Target to enable -std=c11
enableCC:
	scl enable devtoolset-7 tcsh
