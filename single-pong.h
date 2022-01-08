#include <stdlib.h>
#include <ncurses.h>

#include "pong.h"
#define MAX_NUMBER_OF_PLAYERS 10

WINDOW * message_win;

typedef struct ball_position_t{
    int x, y;
    int up_hor_down; //  -1 up, 0 horizontal, 1 down
    int left_ver_right; //  -1 left, 0 vertical,1 right
    char c;
} ball_position_t;

typedef struct paddle_position_t{
    int x, y;
    int length;
} paddle_position_t;

typedef struct message_server{
    ball_position_t ball;
    paddle_position_t paddles[MAX_NUMBER_OF_PLAYERS];
    int score;
} message_server;

typedef struct message_client {
    //Type: 1 - connect, 2 - paddle_move, 4 - disconnect
    int type;
    int key;
} message_client;


void new_paddle (paddle_position_t * paddle, int legth, paddle_position_t *Player_paddles, int numPlayers);
void draw_paddle(WINDOW *win, paddle_position_t * paddle, int delete, char c);
void moove_paddle (paddle_position_t * paddle, paddle_position_t * paddles, int direction, int numPlayers, int index);
void place_ball_random(ball_position_t * ball);
void moove_ball(ball_position_t * ball, paddle_position_t *paddles, int numPlayers, int *scores);
void draw_ball(WINDOW *win, ball_position_t * ball, int draw);
void update_board(WINDOW* my_win, message_server m, ball_position_t prev_ball, paddle_position_t *prev_paddles);
void update_ball_on_screen(WINDOW * my_win, ball_position_t * ball);