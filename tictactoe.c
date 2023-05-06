#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_PENDING 20

/**
 * represents the tictactoe grid
*/
char grid[10];

/**
 * will contain indicies that the respective players took on the grid
*/
int moves_1[9];
int moves_2[9];

struct sockaddr_in sin;
/**
 * builds socket connection for the server
 * Args
 * s: socket number
*/
void connecting(int* s){
    char* host_addr = "127.0.0.1";
    int port = 8080;

    /*set up passive open*/
    if((*s = socket(PF_INET, SOCK_STREAM, 0)) <0) {
    perror("simplex-talk: socket");
    exit(1);
    }

    /* Config the server address */
    // struct sockaddr_in sin; //might have to be global
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = inet_addr(host_addr);
    sin.sin_port = htons(port);
    // Set all bits of the padding field to 0
    memset(sin.sin_zero, '\0', sizeof(sin.sin_zero));

    /* Bind the socket to the address */
    if((bind(*s, (struct sockaddr*)&sin, sizeof(sin)))<0) {
        perror("simplex-talk: bind");
        exit(1);
    }

    // connections can be pending if many concurrent client requests
    listen(*s, MAX_PENDING);

}

/**
 * decides who goes first
*/
int coin_flip(){
    return 2;
}

/**
 * will check to see if player given has 3 spaces in a row
*/
int check_grid(int moves[]){
    //winning combinations
    //0
    int combo_012 = 0;
    int combo_036 = 0;
    int combo_048 = 0;

    //1
    int combo_147 = 0;

    //2
    int combo_246 = 0;
    int combo_258 = 0;

    //3
    int combo_345 = 0;

    //6
    int combo_678 = 0;

    //loop through move set and incrememt combo in which number in array appears
    for(int i = 0; i < 9; i++){
        int index = moves[i];

        if(index == -1){
            continue;
        }else if(index == 0){
            combo_012++;
            combo_036++;
            combo_048++;
        }else if(index == 1){
            combo_012++;
            combo_147++;
        }else if(index == 2){
            combo_012++;
            combo_246++;
            combo_258++;
        }else if(index == 3){
            combo_345++;
            combo_036++;
        }else if(index == 4){
            combo_246++;
            combo_345++;
            combo_048++;
            combo_147++;
            combo_048++;
        }else if(index == 5){
            combo_258++;
            combo_345++;
        }else if(index == 6){
            combo_678++;
            combo_036++;
            combo_246++;
        }else if(index == 7){
            combo_678++;
            combo_147++;
        }else if(index == 8){
            combo_258++;
            combo_678++;
            combo_048++;
        }
    }

    //if any of the combos is 3, means that player got 3 in a row
    if(combo_012 == 3 || combo_036 == 3 || combo_048 == 3 || combo_147 == 3 || combo_246 == 3 || combo_258 == 3 ||
     combo_345 == 3 || combo_678 == 3){
        return 0;
    }
    return -1;   
}

/**
 * game loop
*/
void game_loop(player1,player2){

    
    //send turn order to players
    char* first = "You are going first. You are X.";
    char* second = "Your are going second. You are O.";
    char move[2] = '/0';
    char end[MAX_PENDING];
    memset(end,'\0',MAX_PENDING);

    int f_len = strlen(first) + 1;
    int s_len = strlen(second) + 1;
    int grid_len = strlen(grid) + 1;

    send(player1,first,f_len,0);
    send(player2,second,s_len,0);
    
    int winner = -1;
    int check = -1;
    int place = -1;
    int ind = 0; //counter for number of total moves aka grid spaces that can be marked
    //while loop controlled by check

    while(ind < 9){
        //send grid to player one
        send(player1,grid,grid_len,0);
        //recv p1's move
        int len = recv(player1,move,sizeof(move),0);
        //update grid
        place = atoi(move);
        moves_1[ind] = place;
        grid[place] = "X";

        memset(move,'/0',sizeof(move));
        //check if game over
        check = check_grid(moves_1);
        if(check != -1){
            winner = 1;
            break;
        }
        


        //send grid to p2
        send(player2,grid,grid_len,0);
        //recv p2's move
        len = recv(player2,move,sizeof(move),0);
        //TODO: call accept??

        //update grid
        place = atoi(move);
        moves_2[ind] = place;
        grid[place] = "O";
        memset(move,'/0',sizeof(move));

        //check if game over
        check = check_grid(moves_2);
        if(check != -1){
            winner = 2;
        }
    }

    int len_win = 0;
    if(winner == -1){
        strcpy(end,"Tie, no one wins");
        len_win = strlen(end)+1;
        send(player1,end,len_win,0);
        send(player2,end,len_win,0);
    }else if(winner == 1){
        strcpy(end,"You win");
        len_win = strlen(end)+1;
        send(player1,end,len_win,0);
        memset(end,'\0',MAX_PENDING);
        strcpy(end,"You lose");
        send(player2,end,len_win,0);
    }else{
        strcpy(end,"You win");
        len_win = strlen(end)+1;
        send(player2,end,len_win,0);
        memset(end,'\0',MAX_PENDING);
        strcpy(end,"You lose");
        send(player1,end,len_win,0);
    }
    
}


int main(int argc, char *argv[]){
    strcpy(grid,"SSSSSSSSS"); //initialize grid with filler

    //initialize move arrays
    for(int i = 0; i < 9; i++){
        moves_1[i] = -1;
        moves_2[i] = -1;
    }

    int s = 0; //socket

    connecting(&s); //build socket connection
    socklen_t len = sizeof(sin);

    char buf[MAX_PENDING];
    memset(buf,'\0',MAX_PENDING);

    int playerA;
    if((playerA = accept(s, (struct sockaddr *)&sin, &len)) <0){
      perror("simplex-talk: accept");
      exit(1);
    }

    //TODO: create names array and store name?
    int readyA = recv(playerA, buf, sizeof(buf), 0); //on client side, send a name
    strcat(buf, " is ready.");
    fputs(buf,stdout);
    fflush(stdout);
    memset(buf, '\0', MAX_PENDING);

    int playerB;
    if((playerB = accept(s, (struct sockaddr *)&sin, &len)) <0){
      perror("simplex-talk: accept");
      exit(1);
    }
    int readyB = recv(playerB, buf, sizeof(buf), 0); //on client side, send a name
    strcat(buf, " is ready.");
    fputs(buf,stdout);
    fflush(stdout);

    

    if(coin_flip == 1){
        game_loop(playerA,playerB);
    }else{
        game_loop(playerB,playerA);;
    }

    close(playerA);
    close(playerB);
}