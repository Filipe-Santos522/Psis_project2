#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Serverfunc.h"
#include "sock_dg_inet.h"

int main(int argc, char* argv[]){

    if(argc!=2){
            printf("Error in arguments\n");
            exit(1);
    }

    int nbytes;
    struct sockaddr_in client_addr;
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int sock_fd=Socket_creation();
    ball_position_t x;
    message_client m;
    message_server m_s;
    m.type = 1; /* Set message type to "connect"*/
    Send_Reply_client(sock_fd, &m, &server_addr); /* Send connect message */

    paddle_position_t paddle;
    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    message_win = newwin(5, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);


    int key;
    ball_position_t prev_ball;
    paddle_position_t *prev_paddles = malloc(MAX_NUMBER_OF_PLAYERS * sizeof(paddle_position_t));
    while (1) {
        prev_ball = m_s.ball;
        for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
            prev_paddles[i] = m_s.paddles[i];
        Receive_message_client(sock_fd, &m_s, &server_addr);
        update_board(my_win, m_s, prev_ball, prev_paddles);
        key = wgetch(my_win);
        mvwprintw(message_win, 1,1,"%c key pressed", key);
        wrefresh(message_win);
        if (key != 113 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) {
            m.key = key;
            m.type = 2;
            Send_Reply_client(sock_fd, &m, &server_addr);
        }
        else {
            m.type = 4;
            Send_Reply_client(sock_fd, &m, &server_addr);
            break;
        }
    }

    close(sock_fd);
    exit(0);
}