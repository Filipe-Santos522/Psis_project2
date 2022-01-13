/* Client source code file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210

 * Some code provided by the professors was also used
 */

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

    // Check terminal arguments
    if(argc!=2){
            printf("Error in arguments\n");
            exit(1);
    }

    // Useful variables initialization
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int sock_fd=Socket_creation(); // Open socket for communication
    message_client m;
    message_server m_s;
    m.type = 1; /* Set message type to "connect"*/
    m_s.score = 0; /* Set initial score */
    Send_Reply_client(sock_fd, &m, &server_addr); /* Send connect message to server*/

    initscr();		    	/* Start curses mode 		*/
	cbreak();				
    keypad(stdscr, TRUE);   
	noecho();			    

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
    // Array of paddles of all players
    paddle_position_t *prev_paddles = malloc(MAX_NUMBER_OF_PLAYERS * sizeof(paddle_position_t));
    while (1) {
        prev_ball = m_s.ball; // Store previous ball position
        for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
            prev_paddles[i] = m_s.paddles[i]; // Store the previou position of all paddles
        Receive_message_client(sock_fd, &m_s, &server_addr); // Receive message from server
        update_board(my_win, m_s, prev_ball, prev_paddles); // Update ball and paddle on the screen
        wmove(my_win, 0, 0); // Move cursor to (0,0) (purely aesthetic)
        wrefresh(my_win);
        key = wgetch(my_win); // Get key from user
        mvwprintw(message_win, 1,1,"%c key pressed", key); // Print pressed key
        wrefresh(message_win);
        mvwprintw(message_win, 2,1,"Player score: %d", m_s.score); // Print player score
        wrefresh(message_win);
        
        // Check which key was pressed (113 is "q" for quit)
        if (key != 113 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) {
            m.key = key;  // Change key in message
            m.type = 2; // Change message type
            Send_Reply_client(sock_fd, &m, &server_addr); // Send message to server
        }
        else { // If "q" was pressed
            m.type = 4; // Change message type to disconnect
            Send_Reply_client(sock_fd, &m, &server_addr); // Send message to server
            break;
        }
    }
    endwin(); // Close curses
    free(prev_paddles); // Free array of paddles
    close(sock_fd); // Close socket
    exit(0); // Exit program
}