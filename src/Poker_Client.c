#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <gtk/gtk.h>
#include <assert.h>
#include "Constants.h"
#include "Deck.h"
#define BUFFSIZE 256

GtkWidget *Window;
GtkWidget *VBox1, *VBox2, *VBox3, *HBox1, *HBox2, *HBox3, *VBox4;
GtkWidget *HBox4, *VBox5, *VBox6, *HBox5, *VBox7, *VBox8, *VBox9;
GtkWidget *BoardHBox, *PCardsHBox;
GtkWidget *VBoxP1, *VBoxP2, *VBoxP3, *VBoxP4, *HBoxP1, *HBoxP2, *HBoxP3, *HBoxP4;
GtkWidget *FrameN1, *FrameN2, *FrameN3, *FrameN4, *FrameBoard;
GtkWidget *FrameP1, *FrameP2, *FrameP3, *FrameP4, *FramePCards;
GtkWidget *FrameMinBet, *FramePot, *MinBetLabel, *PotLabel;
GtkWidget *PlayerName1, *PlayerName2, *PlayerName3, *PlayerName4;
GtkWidget *Points1, *Points2, *Points3, *Points4;
GtkWidget *ActButtons, *CallButton, *FoldButton, *RaiseButton, *CheckButton;
GtkWidget *HButtons, *QuitButton, *ShutdownButton;

GtkWidget *flop1, *flop2, *flop3, *turn, *river;
GtkWidget *p1card1, *p1card2, *p2card1, *p2card2, *p3card1, *p3card2, *p4card1, *p4card2;

GdkColor color;

GtkWidget *userEntry, *serverEntry, *portEntry, *seatEntry, *startEntry;
char userName[BUFFSIZE];
char serverName[BUFFSIZE];
char portNumber[BUFFSIZE];
char userSeat[BUFFSIZE];
char raiseAmount[BUFFSIZE];
char startResponse[BUFFSIZE];

int userRank1, userSuit1, userRank2, userSuit2;
int flopRank1, flopSuit1, flopRank2, flopSuit2, flopRank3, flopSuit3;
int turnRank, turnSuit, riverRank, riverSuit;

char userPath1[BUFFSIZE];
char userPath2[BUFFSIZE];
char flop1Path[BUFFSIZE];
char flop2Path[BUFFSIZE];
char flop3Path[BUFFSIZE];
char turnPath[BUFFSIZE];
char riverPath[BUFFSIZE];

int SocketFD,	/* socket file descriptor */
PortNo;		/* port number */
struct sockaddr_in
ServerAddress;	/* server address we connect with */
struct hostent
*Server;	/* server host */
int userRaise;
int seatNumber;
int minBet;
CARD *hand [2];
BOARD *board; 

void getCards(void);

void SetCardImage(int suit, int rank, char *cardPath);

gboolean update_gui(gpointer Data);

void FatalError(const char *ErrorMsg)
{
    perror(ErrorMsg);
    fputs(": Exiting!", stderr);
    exit(20);
} /* end of FatalError */
void ConnectServer(char* username, char* serverAddress, char* port)
{
    int l, n;
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

    Server = gethostbyname(serverAddress);
    if (Server == NULL)
    {   fprintf(stderr, " no such host named '%s'\n", serverAddress);
        exit(10);
    }
    PortNo = atoi(port);
    if (PortNo <= 2000)
    {   fprintf(stderr, "Invalid port number %d, should be greater 2000\n", PortNo);
        exit(10);
    }
    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {   
        FatalError("socket creation failed");
    }
    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    memcpy(&ServerAddress.sin_addr.s_addr,
		Server->h_addr_list[0], Server->h_length);
    printf("Connecting to the server...\n");

    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,	sizeof(ServerAddress)) < 0)
    {   
        FatalError("connecting to server failed");
    }
    l = strlen(userName);
    n = write(SocketFD, userName, l);
    if (n < 0)
    {   
        FatalError("writing to socket failed");
    }

}

// function to enter username and which server user is connecting to
void EnteringNameDialog(void)
{
    GtkWidget *contentArea;
    GtkWidget *serverFrame, *portFrame;
    const char *enteredName;
    userEntry = gtk_entry_new();
    serverEntry = gtk_entry_new();
    portEntry = gtk_entry_new();
    serverFrame = gtk_frame_new("Server Name: ");\
    portFrame = gtk_frame_new("Port Number: ");
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Please enter your name: ");
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(message));
    gtk_container_add(GTK_CONTAINER(contentArea), userEntry);    
    gtk_container_add(GTK_CONTAINER(contentArea), serverFrame);
    gtk_container_add(GTK_CONTAINER(serverFrame), serverEntry);
    gtk_container_add(GTK_CONTAINER(contentArea), portFrame);
    gtk_container_add(GTK_CONTAINER(portFrame), portEntry);

    gtk_widget_show_all(message);

    gtk_dialog_run(GTK_DIALOG(message));
    enteredName = gtk_entry_get_text(GTK_ENTRY(userEntry));
    strcpy(userName, enteredName);
    strcpy(serverName, gtk_entry_get_text(GTK_ENTRY(serverEntry)));
    strcpy(portNumber, gtk_entry_get_text(GTK_ENTRY(portEntry)));
    gtk_widget_destroy(message);
    ConnectServer(userName, serverName, portNumber);
}

// allows the user to pick their seat
void SeatDialog(void)
{
    char RecvBuf[BUFFSIZE];
    //while(RecvBuf != NULL)
    //{
        int i = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
        RecvBuf[i] = 0;
    //}
    //strcpy(RecvBuf, "SEAT-0-1-3");
    printf("%s\n", RecvBuf);

    char seatsMessage[BUFFSIZE];
    strcpy(seatsMessage, "Please enter which seat you want to take: ");

    char *initMessage = strtok(RecvBuf, "-");
    if(strcmp(initMessage, "SEAT") == 0)
    {
        while(initMessage != NULL) 
        {   
            initMessage = strtok(NULL, "-");
            if(initMessage == NULL)
            {
                break;
            }
            strcat(seatsMessage, initMessage);
            strcat(seatsMessage, " ");
        }
    }

    GtkWidget *contentArea;
    seatEntry = gtk_entry_new();
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, seatsMessage);
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(message));
    gtk_container_add(GTK_CONTAINER(contentArea), seatEntry);    

    gtk_widget_show_all(message);

    gtk_dialog_run(GTK_DIALOG(message));
    strcpy(userSeat, gtk_entry_get_text(GTK_ENTRY(seatEntry)));
    seatNumber = atoi(userSeat);

    gtk_widget_destroy(message);


    //printf("%s\n", userSeat);
    userSeat[strlen(userSeat)] = 0;
    int l = strlen(userSeat);
    //printf("%d\n", l);
    int n = write(SocketFD, userSeat, l);
    if (n < 0)
    {   
        FatalError("user seat info not sent");
    }

}

// function to start game
void StartDialog(void)
{
    GtkWidget *contentArea;
    startEntry = gtk_entry_new();
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Please enter if you want to start the game: (y/n) ");
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(message));
    gtk_container_add(GTK_CONTAINER(contentArea), startEntry);    

    gtk_widget_show_all(message);

    gtk_dialog_run(GTK_DIALOG(message));
    strcpy(startResponse, gtk_entry_get_text(GTK_ENTRY(startEntry)));

    gtk_widget_destroy(message);

    startResponse[strlen(startResponse)] = 0;
    int l = strlen(startResponse);
    int n = write(SocketFD, startResponse, l);
    if (n < 0)
    {   
        FatalError("game is not started");
    }
}

// function to take in user input for how much they want to raise
void RaiseDialog(void)
{
    GtkWidget *contentArea;
    userEntry = gtk_entry_new();
    GtkWidget *message = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Please enter the amount you want to raise: ");
    contentArea = gtk_dialog_get_content_area(GTK_DIALOG(message));
    gtk_container_add(GTK_CONTAINER(contentArea), userEntry);    

    gtk_widget_show_all(message);

    gtk_dialog_run(GTK_DIALOG(message));
    strcpy(raiseAmount, gtk_entry_get_text(GTK_ENTRY(userEntry)));
    userRaise = atoi(raiseAmount);

    gtk_widget_destroy(message);
}

void RaiseClicked(GtkWidget *Widget, gpointer Data)
{
    RaiseDialog();

    raiseAmount[strlen(raiseAmount)] = 0;
    int l = strlen(raiseAmount);
    int n = write(SocketFD, raiseAmount, l);
    if (n < 0)
    {   
        FatalError("call not received");
    }

    g_timeout_add_seconds(0.5, update_gui, NULL);
}

void CheckClicked(GtkWidget *Widget, gpointer Data)
{
    int l = strlen("0");
    int n = write(SocketFD, "0", l);
    if (n < 0)
    {   
        FatalError("check not received");
    }

    printf("check\n");

    g_timeout_add_seconds(0.5, update_gui, NULL);
}

void CallClicked(GtkWidget *Widget, gpointer Data)
{
    char callMessage[BUFFSIZE];

    sprintf(callMessage, "%d", minBet);

    callMessage[strlen(callMessage)] = 0;
    int l = strlen(callMessage);
    int n = write(SocketFD, callMessage, l);
    if (n < 0)
    {   
        FatalError("call not received");
    }

    printf("call\n");

    g_timeout_add_seconds(0.5, update_gui, NULL);
}

void FoldClicked(GtkWidget *Widget, gpointer Data)
{
    int l = strlen("-1");
    int n = write(SocketFD, "-1", l);
    if (n < 0)
    {   
        FatalError("fold not received");
    }
    
    printf("fold\n");

    g_timeout_add_seconds(0.5, update_gui, NULL);
}

gboolean update_gui(gpointer Data)
{
    char RecvBuf[BUFFSIZE];
    int i = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
    RecvBuf[i] = 0;
    printf("incoming message %s\n", RecvBuf);
    //strcpy(RecvBuf, "HAND-0:3-4:1");
    //strcpy(RecvBuf, "CONNECTION-Spiders-2");
    //strcpy(RecvBuf, "FLOP-12:0-0:1-3:2");
    //strcpy(RecvBuf, "TURNER-12:0-0:1-3:2-8:0");
    //strcpy(RecvBuf, "RIVER-12:0-0:1-3:2-8:0-9:0");
    //strcpy(RecvBuf, "GETBET-50");

    char *initMessage = strtok(RecvBuf, "-");

    if(strcmp(initMessage, "CONNECTION") == 0)
    {
        char *newPlayer = strtok(NULL, "-");
        char *newSeat = strtok(NULL, "");

        int newSeatNum = atoi(newSeat);

        if(newSeatNum == 0)
        {
            gtk_label_set_label(GTK_LABEL(PlayerName1), newPlayer);
            gtk_widget_show(FrameN1);
            gtk_widget_show(FrameP1);
        }
        else if(newSeatNum == 1)
        {
            gtk_label_set_label(GTK_LABEL(PlayerName2), newPlayer);
            gtk_widget_show(FrameN2);
            gtk_widget_show(FrameP2);
        }
        else if(newSeatNum == 2)
        {
            gtk_label_set_label(GTK_LABEL(PlayerName3), newPlayer);
            gtk_widget_show(FrameN3);
            gtk_widget_show(FrameP3);
        }
        else if(newSeatNum == 3)
        {
            gtk_label_set_label(GTK_LABEL(PlayerName4), newPlayer);
            gtk_widget_show(FrameN4);
            gtk_widget_show(FrameP4);
        }

        StartDialog();
    }
    else if(strcmp(initMessage, "HAND") == 0)
    {
        char *rank1 = strtok(NULL, ":");
        char *suit1 = strtok(NULL, "-");
        char *rank2 = strtok(NULL, ":");
        char *suit2 = strtok(NULL, "");

        userRank1 = atoi(rank1);
        userSuit1 = atoi(suit1);
        userRank2 = atoi(rank2);
        userSuit2 = atoi(suit2);

        SetCardImage(userSuit1, userRank1, userPath1);
        SetCardImage(userSuit2, userRank2, userPath2);

        if(seatNumber == 0)
        {

            gtk_image_set_from_file(p1card1, userPath1);
            gtk_image_set_from_file(p1card2, userPath2);
            gtk_widget_show(p1card1);
            gtk_widget_show(p1card2);
        }
        else if(seatNumber == 1)
        {
            gtk_image_set_from_file(p2card1, userPath1);
            gtk_image_set_from_file(p2card2, userPath2);
            gtk_widget_show(p2card1);
            gtk_widget_show(p2card2);
        }
        else if(seatNumber == 2)
        {
            gtk_image_set_from_file(p3card1, userPath1);
            gtk_image_set_from_file(p3card2, userPath2);
            gtk_widget_show(p3card1);
            gtk_widget_show(p3card2);
        }
        else if(seatNumber == 3)
        {
            gtk_image_set_from_file(p4card1, userPath1);
            gtk_image_set_from_file(p4card2, userPath2);
            gtk_widget_show(p4card1);
            gtk_widget_show(p4card2);
        }

        int l = strlen("ok");
        int n = write(SocketFD, "ok", l);
        if (n < 0)
        {   
            FatalError("hand not received");
        }
    }
    else if(strcmp(initMessage, "FLOP") == 0)
    {
        char *rank1 = strtok(NULL, ":");
        char *suit1 = strtok(NULL, "-");
        char *rank2 = strtok(NULL, ":");
        char *suit2 = strtok(NULL, "-");
        char *rank3 = strtok(NULL, ":");
        char *suit3 = strtok(NULL, "");

        flopRank1 = atoi(rank1);
        flopSuit1 = atoi(suit1);
        flopRank2 = atoi(rank2);
        flopSuit2 = atoi(suit2);
        flopRank3 = atoi(rank3);
        flopSuit3 = atoi(suit3);

        SetCardImage(flopSuit1, flopRank1, flop1Path);
        SetCardImage(flopSuit2, flopRank2, flop2Path);
        SetCardImage(flopSuit3, flopRank3, flop3Path);

        gtk_image_set_from_file(flop1, flop1Path);
        gtk_image_set_from_file(flop2, flop2Path);
        gtk_image_set_from_file(flop3, flop3Path);

        gtk_widget_show(flop1);
        gtk_widget_show(flop2);
        gtk_widget_show(flop3);

        int l = strlen("ok");
        int n = write(SocketFD, "ok", l);
        if (n < 0)
        {   
            FatalError("flop not received");
        }
    }
    else if(strcmp(initMessage, "TURNER") == 0)
    {
        char *rank1 = strtok(NULL, ":");
        char *suit1 = strtok(NULL, "-");
        char *rank2 = strtok(NULL, ":");
        char *suit2 = strtok(NULL, "-");
        char *rank3 = strtok(NULL, ":");
        char *suit3 = strtok(NULL, "-");
        char *rank4 = strtok(NULL, ":");
        char *suit4 = strtok(NULL, "");

        flopRank1 = atoi(rank1);
        flopSuit1 = atoi(suit1);
        flopRank2 = atoi(rank2);
        flopSuit2 = atoi(suit2);
        flopRank3 = atoi(rank3);
        flopSuit3 = atoi(suit3);
        turnRank = atoi(rank4);
        turnSuit = atoi(suit4);

        SetCardImage(flopSuit1, flopRank1, flop1Path);
        SetCardImage(flopSuit2, flopRank2, flop2Path);
        SetCardImage(flopSuit3, flopRank3, flop3Path);
        SetCardImage(turnSuit, turnRank, turnPath);

        gtk_image_set_from_file(flop1, flop1Path);
        gtk_image_set_from_file(flop2, flop2Path);
        gtk_image_set_from_file(flop3, flop3Path);
        gtk_image_set_from_file(turn, turnPath);

        gtk_widget_show(flop1);
        gtk_widget_show(flop2);
        gtk_widget_show(flop3);
        gtk_widget_show(turn);

        int l = strlen("ok");
        int n = write(SocketFD, "ok", l);
        if (n < 0)
        {   
            FatalError("turn not received");
        }
    }
    else if(strcmp(initMessage, "RIVER") == 0)
    {
        char *rank1 = strtok(NULL, ":");
        char *suit1 = strtok(NULL, "-");
        char *rank2 = strtok(NULL, ":");
        char *suit2 = strtok(NULL, "-");
        char *rank3 = strtok(NULL, ":");
        char *suit3 = strtok(NULL, "-");
        char *rank4 = strtok(NULL, ":");
        char *suit4 = strtok(NULL, "-");
        char *rank5 = strtok(NULL, ":");
        char *suit5 = strtok(NULL, "");

        flopRank1 = atoi(rank1);
        flopSuit1 = atoi(suit1);
        flopRank2 = atoi(rank2);
        flopSuit2 = atoi(suit2);
        flopRank3 = atoi(rank3);
        flopSuit3 = atoi(suit3);
        turnRank = atoi(rank4);
        turnSuit = atoi(suit4);
        riverRank = atoi(rank5);
        riverSuit = atoi(suit5);

        SetCardImage(flopSuit1, flopRank1, flop1Path);
        SetCardImage(flopSuit2, flopRank2, flop2Path);
        SetCardImage(flopSuit3, flopRank3, flop3Path);
        SetCardImage(turnSuit, turnRank, turnPath);
        SetCardImage(riverSuit, riverRank, riverPath);

        gtk_image_set_from_file(flop1, flop1Path);
        gtk_image_set_from_file(flop2, flop2Path);
        gtk_image_set_from_file(flop3, flop3Path);
        gtk_image_set_from_file(turn, turnPath);
        gtk_image_set_from_file(river, riverPath);

        gtk_widget_show(flop1);
        gtk_widget_show(flop2);
        gtk_widget_show(flop3);
        gtk_widget_show(turn);
        gtk_widget_show(river);

        int l = strlen("ok");
        int n = write(SocketFD, "ok", l);
        if (n < 0)
        {   
            FatalError("river not received");
        }
    }
    else if(strcmp(initMessage, "GETBET") == 0)
    {
        char *minimum = strtok(NULL, "");

        minBet = atoi(minimum);

        gtk_label_set_label(GTK_LABEL(MinBetLabel), minimum);

        gtk_widget_set_sensitive(CheckButton, TRUE);
        gtk_widget_set_sensitive(CallButton, TRUE);
        gtk_widget_set_sensitive(RaiseButton, TRUE);
        gtk_widget_set_sensitive(FoldButton, TRUE);

        return FALSE;
    }

    return G_SOURCE_CONTINUE;
}

void ShutdownServer(		/* ask server to shutdown */
	GtkWidget *Widget,
	gpointer Data)
{
#ifdef DEBUG
    printf("%s: ShutdownServer callback starting...\n", Program);
#endif

    int l = strlen("shutdown");
    int n = write(SocketFD, "shutdown", l);
    if (n < 0)
    {   
        FatalError("server not shutdown");
    }

	gtk_main_quit();

#ifdef DEBUG
    printf("%s: ShutdownServer callback done.\n", Program);
#endif
} /* end of ShutdownServer */

int main(int argc, char *argv[])
{
    //hand[0] = CreateCard(0,0);
    //hand[1] = CreateCard(0,0);
    //board = CreateBoard();

    color.red = 0x9292;
    color.green = 0xC7C7;
    color.blue = 0x6B6B;

    //SetCardImage(1,1,flop1Path);


    /* build the GUI */
    /* (adapted from https://en.wikipedia.org/wiki/GTK%2B#Example) */

    /* initialize the GTK libraries */
    gtk_init(&argc, &argv);

    /* create the main, top level window */
    Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* give it the title of this program */
    gtk_window_set_title(GTK_WINDOW(Window), "Family Friendly Poker");

    /* center the window */
    gtk_window_set_position(GTK_WINDOW(Window), GTK_WIN_POS_CENTER);

    /* set the window's default size */
    gtk_window_set_default_size(GTK_WINDOW(Window), 1024, 720);
    gtk_container_set_border_width (GTK_CONTAINER(Window), 10);

    /* set background color of window */
    gtk_widget_modify_bg(Window, GTK_STATE_NORMAL, &color);

    /* map the destroy signal of the window to gtk_main_quit;
     * when the window is about to be destroyed, we get a notification and
     * stop the main GTK+ loop by returning 0 */
    g_signal_connect(Window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* entering name */
    EnteringNameDialog();

    /* entering seat number */
    SeatDialog();

    /* overall vertical arrangement in the window */
    VBox4 = gtk_vbox_new(TRUE, 5);
    gtk_container_add(GTK_CONTAINER(Window), VBox4);
    HBox1 = gtk_hbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(VBox4), HBox1);
    HBox2 = gtk_hbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(VBox4), HBox2);
    HBox3 = gtk_hbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(VBox4), HBox3);
    VBox1 = gtk_vbox_new(TRUE, 10);
    VBox2 = gtk_vbox_new(TRUE, 10);
    VBox3 = gtk_vbox_new(TRUE, 10);
    HBox4 = gtk_hbox_new(TRUE, 10);
    VBox5 = gtk_vbox_new(TRUE, 10);
    VBox6 = gtk_vbox_new(TRUE, 10);
    HBox5 = gtk_hbox_new(TRUE, 10);
    VBox7 = gtk_vbox_new(TRUE, 10);
    VBox8 = gtk_vbox_new(TRUE, 10);
    VBox9 = gtk_vbox_new(TRUE, 10);
    VBoxP1 = gtk_vbox_new(TRUE, 10);
    VBoxP2 = gtk_vbox_new(TRUE, 10);
    VBoxP3 = gtk_vbox_new(TRUE, 10);
    VBoxP4 = gtk_vbox_new(TRUE, 10);
    HBoxP1 = gtk_hbox_new(TRUE, 10);
    HBoxP2 = gtk_hbox_new(TRUE, 10);
    HBoxP3 = gtk_hbox_new(TRUE, 10);
    HBoxP4 = gtk_hbox_new(TRUE, 10);

    /* on the top, put a button to request the current time from the server */
    //RequestButton = gtk_button_new_with_label("Request Time from Server");
    //gtk_container_add(GTK_CONTAINER(VBox), RequestButton);

    /* in the middle, a frame displaying player 1 */
    gtk_container_add(GTK_CONTAINER(HBox3), VBox1);
    FrameN1 = gtk_frame_new("Player 1:");
    gtk_container_add(GTK_CONTAINER(VBox1), FrameN1);
    PlayerName1 = gtk_label_new(userName);
    gtk_container_add(GTK_CONTAINER(FrameN1), PlayerName1);

    FrameP1 = gtk_frame_new("Points");
    gtk_container_add(GTK_CONTAINER(VBox1), FrameP1);
    Points1 = gtk_label_new("1000");
    gtk_container_add(GTK_CONTAINER(FrameP1), Points1);

    /* in the middle, a frame displaying player 2 */
    gtk_container_add(GTK_CONTAINER(HBox3), VBox2);
    FrameN2 = gtk_frame_new("Player 2:");
    gtk_container_add(GTK_CONTAINER(VBox2), FrameN2);
    PlayerName2 = gtk_label_new("Granny");
    gtk_container_add(GTK_CONTAINER(FrameN2), PlayerName2);

    FrameP2 = gtk_frame_new("Points");
    gtk_container_add(GTK_CONTAINER(VBox2), FrameP2);
    Points2 = gtk_label_new("1200");
    gtk_container_add(GTK_CONTAINER(FrameP2), Points2);

    /* in the middle, a frame displaying player 3 */
    gtk_container_add(GTK_CONTAINER(HBox3), VBox3);
    FrameN3 = gtk_frame_new("Player 3:");
    gtk_container_add(GTK_CONTAINER(VBox3), FrameN3);
    PlayerName3 = gtk_label_new("Bansworth");
    gtk_container_add(GTK_CONTAINER(FrameN3), PlayerName3);

    FrameP3 = gtk_frame_new("Points");
    gtk_container_add(GTK_CONTAINER(VBox3), FrameP3);
    Points3 = gtk_label_new("200");
    gtk_container_add(GTK_CONTAINER(FrameP3), Points3);

    /* in the middle, a frame displaying player 1 */
    gtk_container_add(GTK_CONTAINER(HBox3), VBox9);
    FrameN4 = gtk_frame_new("Player 4:");
    gtk_container_add(GTK_CONTAINER(VBox9), FrameN4);
    PlayerName4 = gtk_label_new("Graig");
    gtk_container_add(GTK_CONTAINER(FrameN4), PlayerName4);

    FrameP4 = gtk_frame_new("Points");
    gtk_container_add(GTK_CONTAINER(VBox9), FrameP4);
    Points4 = gtk_label_new("1000");
    gtk_container_add(GTK_CONTAINER(FrameP4), Points4);

    /* three action buttons: call, raise, fold */
    gtk_container_add(GTK_CONTAINER(VBox4), HBox5);
    ActButtons = gtk_hbutton_box_new();
    gtk_container_add(GTK_CONTAINER(HBox5), VBox7);
    gtk_container_add(GTK_CONTAINER(HBox5), ActButtons);
    gtk_container_add(GTK_CONTAINER(HBox5), VBox8);
    FrameMinBet = gtk_frame_new("Min Bet");
    gtk_container_add(GTK_CONTAINER(VBox7), FrameMinBet);
    MinBetLabel = gtk_label_new("1200");
    gtk_container_add(GTK_CONTAINER(FrameMinBet), MinBetLabel);
    FramePot = gtk_frame_new("Pot");
    gtk_container_add(GTK_CONTAINER(VBox8), FramePot);
    PotLabel = gtk_label_new("1200");
    gtk_container_add(GTK_CONTAINER(FramePot), PotLabel);
    CheckButton = gtk_button_new_with_label("Check");
    gtk_container_add(GTK_CONTAINER(ActButtons), CheckButton);
    CallButton = gtk_button_new_with_label("Call");
    gtk_container_add(GTK_CONTAINER(ActButtons), CallButton);
    RaiseButton = gtk_button_new_with_label("Raise");
    gtk_container_add(GTK_CONTAINER(ActButtons), RaiseButton);
    FoldButton = gtk_button_new_with_label("Fold");
    gtk_container_add(GTK_CONTAINER(ActButtons), FoldButton);

    /* frame for the board to display community cards */
    FrameBoard = gtk_frame_new("Board");
    gtk_container_add(GTK_CONTAINER(HBox1), FrameBoard);
    BoardHBox = gtk_hbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(FrameBoard), BoardHBox);
    flop1 = gtk_image_new_from_file(flop1Path);
    gtk_container_add(GTK_CONTAINER(BoardHBox), flop1);
    flop2 = gtk_image_new_from_file("./images/5spade.png");
    gtk_container_add(GTK_CONTAINER(BoardHBox), flop2);
    flop3 = gtk_image_new_from_file("./images/4club.png");
    gtk_container_add(GTK_CONTAINER(BoardHBox), flop3);
    turn = gtk_image_new_from_file("./images/Kspade.png");
    gtk_container_add(GTK_CONTAINER(BoardHBox), turn);
    river = gtk_image_new_from_file("./images/Kdiamond.png");
    gtk_container_add(GTK_CONTAINER(BoardHBox), river);

    /* frame for player's cards */
    FramePCards = gtk_frame_new("Player Cards");
    gtk_container_add(GTK_CONTAINER(HBox2), FramePCards);
    PCardsHBox = gtk_hbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(FramePCards), PCardsHBox);
    gtk_container_add(GTK_CONTAINER(PCardsHBox), VBoxP1);
    gtk_container_add(GTK_CONTAINER(PCardsHBox), VBoxP2);
    gtk_container_add(GTK_CONTAINER(PCardsHBox), VBoxP3);
    gtk_container_add(GTK_CONTAINER(PCardsHBox), VBoxP4);
    gtk_container_add(GTK_CONTAINER(VBoxP1), HBoxP1);
    gtk_container_add(GTK_CONTAINER(VBoxP2), HBoxP2);
    gtk_container_add(GTK_CONTAINER(VBoxP3), HBoxP3);
    gtk_container_add(GTK_CONTAINER(VBoxP4), HBoxP4);
    p1card1 = gtk_image_new_from_file("./images/2spade.png");
    gtk_container_add(GTK_CONTAINER(HBoxP1), p1card1);
    p1card2 = gtk_image_new_from_file("./images/Aclub.png");
    gtk_container_add(GTK_CONTAINER(HBoxP1), p1card2);
    p2card1 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP2), p2card1);
    p2card2 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP2), p2card2);
    p3card1 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP3), p3card1);
    p3card2 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP3), p3card2);
    p4card1 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP4), p4card1);
    p4card2 = gtk_image_new_from_file("./images/BackCard.png");
    gtk_container_add(GTK_CONTAINER(HBoxP4), p4card2);

    /* on the bottom, two buttons to quit client and shutdown server */
    gtk_container_add(GTK_CONTAINER(VBox4), HBox4);
    HButtons = gtk_hbutton_box_new();
    gtk_container_add(GTK_CONTAINER(HBox4), VBox5);
    gtk_container_add(GTK_CONTAINER(HBox4), HButtons);
    gtk_container_add(GTK_CONTAINER(HBox4), VBox6);
    QuitButton = gtk_button_new_with_label("Quit Client");
    gtk_container_add(GTK_CONTAINER(HButtons), QuitButton);
    ShutdownButton = gtk_button_new_with_label("Shutdown Server");
    gtk_container_add(GTK_CONTAINER(HButtons), ShutdownButton);

    /* timer to update gui */
    g_timeout_add_seconds(0.5, update_gui, NULL);

    /* make sure that everything becomes visible */
    gtk_widget_show_all(Window);

    gtk_widget_hide(flop1);
    gtk_widget_hide(flop2);
    gtk_widget_hide(flop3);
    gtk_widget_hide(turn);
    gtk_widget_hide(river);
    gtk_widget_hide(FrameN1);
    gtk_widget_hide(FrameP1);
    gtk_widget_hide(FrameN2);
    gtk_widget_hide(FrameP2);
    gtk_widget_hide(FrameN3);
    gtk_widget_hide(FrameP3);
    gtk_widget_hide(FrameN4);
    gtk_widget_hide(FrameP4);
    gtk_widget_hide(p1card1);
    gtk_widget_hide(p1card2);
    gtk_widget_hide(p2card1);
    gtk_widget_hide(p2card2);
    gtk_widget_hide(p3card1);
    gtk_widget_hide(p3card2);
    gtk_widget_hide(p4card1);
    gtk_widget_hide(p4card2);
    gtk_widget_set_sensitive(CheckButton, FALSE);
    gtk_widget_set_sensitive(CallButton, FALSE);
    gtk_widget_set_sensitive(RaiseButton, FALSE);
    gtk_widget_set_sensitive(FoldButton, FALSE);

    /* connect request button with function asking server for time */
    //g_signal_connect(RequestButton, "clicked",
	//		G_CALLBACK(GetTimeFromServer), Label);

    /* connect quit button with function terminating this client */
    /* (note the 'swapped' call; try without to see the effect) */
    g_signal_connect_swapped(QuitButton, "clicked",
			G_CALLBACK(gtk_widget_destroy), Window);

    /* connect shutdown button with function terminating server and client */
    g_signal_connect(ShutdownButton, "clicked",
			G_CALLBACK(ShutdownServer), NULL);

    /* connect raise button with function that raises bet */
    g_signal_connect(RaiseButton, "clicked",
            G_CALLBACK(RaiseClicked), NULL);

    /* connect call button with call function */
    g_signal_connect(CallButton, "clicked",
            G_CALLBACK(CallClicked), NULL);

    /* connect call button with call function */
    g_signal_connect(CheckButton, "clicked",
            G_CALLBACK(CheckClicked), NULL);

    /* connect fold button with fold function */
    g_signal_connect(FoldButton, "clicked",
            G_CALLBACK(FoldClicked), NULL);

    if(seatNumber == 0)
    {
        gtk_label_set_label(GTK_LABEL(PlayerName1), userName);
        gtk_widget_show(FrameN1);
        gtk_widget_show(FrameP1);
    }
    else if(seatNumber == 1)
    {
        gtk_label_set_label(GTK_LABEL(PlayerName2), userName);
        gtk_widget_show(FrameN2);
        gtk_widget_show(FrameP2);
    }
    else if(seatNumber == 2)
    {
        gtk_label_set_label(GTK_LABEL(PlayerName3), userName);
        gtk_widget_show(FrameN3);
        gtk_widget_show(FrameP3);
    }
    else if(seatNumber == 3)
    {
        gtk_label_set_label(GTK_LABEL(PlayerName4), userName);
        gtk_widget_show(FrameN4);
        gtk_widget_show(FrameP4);
    }

    /* start the main loop, handle all registered events */;
    gtk_main();

    return 0;
}

void getCards()
{
    char RecvBuf[256];	// message buffer for receiving a message 
    int n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
    RecvBuf[n] = 0;

    char *rank1 = strtok(RecvBuf, ":");
    char *suit1 = strtok(NULL, "-");
    char *rank2 = strtok(NULL, ":");
    char *suit2 = strtok(NULL, "");

    hand[0]->rank = atoi(rank1);
    hand[0]->suit = atoi(suit1);
    hand[1]->rank = atoi(rank2);
    hand[1]->suit = atoi(suit2);
    
}

void SetCardImage(int suit, int rank, char *cardPath)
{
    //char cardPath[] = "./images/";
    cardPath[0] = "\0";
    strcpy(cardPath, "./images/");

    if(rank == 0)
    {
        strcat(cardPath, "2");
    }
    else if(rank == 1)
    {
        strcat(cardPath, "3");
    }
    else if(rank == 2)
    {
        strcat(cardPath, "4");
    }
    else if(rank == 3)
    {
        strcat(cardPath, "5");
    }
    else if(rank == 4)
    {
        strcat(cardPath, "6");
    }
    else if(rank == 5)
    {
        strcat(cardPath, "7");
    }
    else if(rank == 6)
    {
        strcat(cardPath, "8");
    }
    else if(rank == 7)
    {
        strcat(cardPath, "9");
    }
    else if(rank == 8)
    {
        strcat(cardPath, "10");
    }
    else if(rank == 9)
    {
        strcat(cardPath, "J");
    }
    else if(rank == 10)
    {
        strcat(cardPath, "Q");
    }
    else if(rank == 11)
    {
        strcat(cardPath, "K");
    }
    else if(rank == 12)
    {
        strcat(cardPath, "A");
    }
    else if(rank == 13)
    {
        strcat(cardPath, "Back");
    }

    if(suit == 0)
    {
        strcat(cardPath, "club");
    }
    else if(suit == 1)
    {
        strcat(cardPath, "diamond");
    }
    else if(suit == 2)
    {
        strcat(cardPath, "heart");
    }
    else if(suit == 3)
    {
        strcat(cardPath, "spade");
    }
    else if(suit == 4)
    {
        strcat(cardPath, "Card");
    }

    strcat(cardPath, ".png");
}