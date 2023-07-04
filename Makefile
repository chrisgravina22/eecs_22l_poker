####################### Default Target ######################
all:
	cd src/ && make

####################### Target to Clean #####################
clean:
	rm -f ./bin/Client
	rm -f ./bin/Server
	rm -f ./src/*o

####################### Archive Target #####################
tar:
	gtar cvzf Poker_V1.0_src.tar.gz README COPYRIGHT INSTALL Makefile bin/ doc/ src/
