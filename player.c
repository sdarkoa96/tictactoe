#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

struct sockaddr_in sin;

void instructions(){
    printf("Welcome to tic-tac-toe. You will be playing against another opponent on a game hosted by the server.\n");
    printf("Your goal is to mark 3 consecutive boxes before your opponent.\n");
    printf("Please enter your name (10 characters or less): \n");
}

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
    if((connect(*s, (struct sockaddr*)&sin, sizeof(sin)))<0) {
        perror("simplex-talk: bind");
        exit(1);
    }
}

void print_grid(char *grid){
    int grid_len = strlen(grid);
    int row = 0;

    for(int i = 0; i < grid_len; i++){
        // check if row = 3 if so print newline; reset to 0
        if(row == 3){
            row = 0;
            printf("\n");
        }

        //check is spot empty
        if(grid[i] == 'S'){
            printf("   |");
        }
        //print spot X or O
        else{
            printf(" %c |",grid[i]);
        } 
        //incrememt row
        row++;
    }
}

int check_move(char *grid, int move){
    if(grid[move] != 'S'){
        printf("You cannot take this space.\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]){

    char name[11];
    memset(name,'\0',11);

    instructions();

    //get name from stdin
    int name_len = 0;
    while(name_len < 1){
        name_len = scanf("%10s",name);
        if(name_len < 1){
            printf("Please enter and valid name.\n");
        }
    }

    int socket = 0;
    connecting(&socket);

    //send name to server
    name_len = strlen(name) + 1;
    send(socket,name,name_len,0);

    char msg[30];
    memset(msg,'\0',30);

    //recv player order
    int recv_len = recv(socket,msg,sizeof(msg),0);
    fputs(msg,stdout);
    fflush(stdout);
    
    //loop
    // recv grid OR who won - if not grid exit loop
    //print grid
    //ask playe for move
    //check move
    //sending moves

}