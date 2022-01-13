/* Pong functions source code file:
 * Original code given by the professors and adapted by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 */

#include <stdlib.h>
#include <ncurses.h>
#include "pong.h"
#include "single-pong.h"

/* Function that creates a new paddle
 * Guarantees that the created paddle does not overlap with existing ones
 * The new paddle is initialized at a random "x", and a "y" such that 
 * no other paddle has the same "y" value
 */
void new_paddle(paddle_position_t * paddle, int legth, paddle_position_t *Player_paddles, int numPlayers, ball_position_t ball){
    int y = 1;
    for (int i = 0; i < numPlayers; i++){
        if (Player_paddles[i].y != y) // Verify "y" position of all paddles
            break;
        y++;
    }
    paddle->x = (rand() % WINDOW_SIZE/2) + 3;
    paddle->y = y;
    paddle->length = legth;
}


/* Function that draws paddles on the screen
 * Draws character "c" in the positions given by"paddle"
 * This adaptation is useful when drawing paddles of different clients with "_" and "="
 */
void draw_paddle(WINDOW *win, paddle_position_t * paddle, int delete, char c){
    int ch;
    if(delete){
        ch = c;
    }else{
        ch = ' ';
    }
    int start_x = paddle->x - paddle->length;
    int end_x = paddle->x + paddle->length;
    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle->y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}

/* Function that moves a paddle, only if there is no other paddle in its way
 * This function has logic that prevents a paddle from moving if it were to move
 * to a position already occupied by another paddle or if it were to move out of bounds.
 * It also has logic to force the ball to move if a paddle moves towards it.
 */ 
void moove_paddle (paddle_position_t * paddle, paddle_position_t * paddles, int direction, int numPlayers, int index, ball_position_t *ball){
    int paddle_aux;
    int flag=0; // Auxiliary flag for logic

    // Move up logic
    if (direction == KEY_UP){
        paddle_aux=paddle->y-1;
        for (int i = 0; i < numPlayers; i++)// Check all paddles
            if (paddle_aux == paddles[i].y && (paddles[i].x-paddles[index].x < 5 && paddles[i].x-paddles[index].x > -5))
                flag=1;

        if (paddle->y  != 1 && flag!=1){
            if (ball->y == paddle->y - 1 && ball->y != 1 && (ball->x - paddle->x < 3 && ball->x - paddle->x > -3)){
                ball->y--;
                ball->up_hor_down = -1;
                paddle->y--;
            }
            else if (paddle->y - 1 != ball->y || ball->x - paddle->x >= 3 || ball->x - paddle->x <= -3)
                paddle->y--;
        }
    }

    // Move down logic
    if (direction == KEY_DOWN){
        paddle_aux=paddle->y+1;
        for (int i = 0; i < numPlayers; i++) // Check all paddles
            if (paddle_aux == paddles[i].y && (paddles[i].x-paddles[index].x < 5 && paddles[i].x-paddles[index].x > -5))
                flag=1;

        if (paddle->y  != WINDOW_SIZE - 2 && flag!=1){
            if (ball->y == paddle->y + 1 && ball->y != WINDOW_SIZE - 2 && (ball->x - paddle->x < 3 && ball->x - paddle->x > -3)){
                ball->y++;
                ball->up_hor_down = 1;
                paddle->y++;
            }
            else if (paddle->y + 1 != ball->y || ball->x - paddle->x >= 3 || ball->x - paddle->x <= -3)
                paddle->y++;
        }
        
    }
    
    // Move left logic
    if (direction == KEY_LEFT){
        paddle_aux=paddle->x-1;
        for (int i = 0; i < numPlayers; i++)// Check all paddles
            if (paddle->y == paddles[i].y && (paddles[i].x-paddle_aux < 5 && paddles[i].x-paddle_aux > -5)){
                if(i != index)
                    flag=1;
            }

        if (paddle->x  != 3 && flag!=1){
            if (ball->y == paddle->y && ball->x != 1 &&  ball->x - paddle->x == -3){
                ball->x--;
                ball->left_ver_right = -1;
                paddle->x--;
            }
            else if (paddle->y != ball->y || ball->x - paddle->x <= -3)
                paddle->x--;
        }  
    }
    
    // Move right logic
    if (direction == KEY_RIGHT){
        paddle_aux=paddle->x+1;
        for (int i = 0; i < numPlayers; i++) // Check all paddles
            if (paddle->y == paddles[i].y && (paddles[i].x-paddle_aux < 5 && paddles[i].x-paddle_aux > -5)){
                if(i != index)
                    flag=1;
            }
            
        if (paddle->x  != WINDOW_SIZE-4 && flag!=1){
            if (ball->y == paddle->y && ball->x != WINDOW_SIZE-2 && ball->x - paddle->x == 3){
                ball->x++;
                ball->left_ver_right = 1;
                paddle->x++;
            }
            else if (paddle->y != ball->y || ball->x - paddle->x >= 3)
                paddle->x++;
        }
    }
}

/* Function that places the ball randomly. No adaptations were made. */
void place_ball_random(ball_position_t * ball){
    ball->x = rand() % WINDOW_SIZE ;
    ball->y = rand() % WINDOW_SIZE ;
    ball->c = 'o';
    ball->up_hor_down = rand() % 3 -1; //  -1 up, 1 - down
    ball->left_ver_right = rand() % 3 -1 ; // 0 vertical, -1 left, 1 right
}

/* Function that moves the ball if it does not hit a paddle or move out of bounds
 * 
 */
void moove_ball(ball_position_t * ball, paddle_position_t *paddles, int numPlayers, int *scores){
    
    int next_x = ball->x + ball->left_ver_right;
    int next_y = ball->y + ball->up_hor_down;

    // Auxiliary flags for logic
    int flag3=0;
    int flag=0;
    int flag2=0;
    int flag4=0;

    for (int i = 0; i < numPlayers; i++){ // Check if ball collides with paddles
        if ((paddles[i].x + 2 >= next_x && paddles[i].x - 2 <= next_x) && paddles[i].y == next_y){
            if(ball->up_hor_down==0){
                flag2=1;
            }else{
                flag = 1; 
            }     
            scores[i]++;
        }
    }

    for (int i = 0; i < numPlayers; i++){ 
        if (paddles[i].y==ball->y && (paddles[i].x + 2 >= next_x && paddles[i].x - 2 <= next_x)){
            flag3=1;
        }
        if(paddles[i].y==next_y && (paddles[i].x + 2 >= ball->x && paddles[i].x - 2 <= ball->x))
            flag4=1;
    }

    // Move ball according to the conditions established by the flags and next ball position
    if( (next_y == 0 || next_y == WINDOW_SIZE-1 || flag == 1) && (next_x == 0 || next_x == WINDOW_SIZE-1 || flag2 == 1)){
        ball->up_hor_down *= -1;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
        return;
    }

    // Check if ball move in "x" direction is valid
    if( next_x == 0 || next_x == WINDOW_SIZE-1 || flag2 == 1 || flag3==1){
        ball->up_hor_down = rand() % 3 -1 ;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"left right win");
        wrefresh(message_win);
     }else{
        ball->x = next_x;
    }
    
    // Check if ball move in "y" direction is valid
    if( next_y == 0 || next_y == WINDOW_SIZE-1 || flag == 1 || flag4==1){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
    }else{
        ball -> y = next_y;
    }
}

/* Function that draws the ball on the screen */
void draw_ball(WINDOW *win, ball_position_t * ball, int draw){
    int ch;
    if(draw){
        ch = ball->c;
    }else{
        ch = ' ';
    }
    wmove(win, ball->y, ball->x);
    waddch(win,ch);
    wrefresh(win);
}

/* Function that updates the whole board.
 * This includes the ball and all the paddles  
 */
void update_board(WINDOW* my_win, message_server m, ball_position_t prev_ball, paddle_position_t *prev_paddles){
    
    // Erase and draw the ball
    draw_ball(my_win, &prev_ball, false);
    draw_ball(my_win, &m.ball, true);

    // Erase and draw the paddle of the client
    draw_paddle(my_win, &prev_paddles[0], false, '=');
    draw_paddle(my_win, &m.paddles[0], true, '=');

    // Erase and draw the paddle of other clients
    int i = 1;
    while (prev_paddles[i].length > 0 && i < MAX_NUMBER_OF_PLAYERS) {
        draw_paddle(my_win, &prev_paddles[i], false, '_');
        i++;
    }
    i = 1;
    while (m.paddles[i].length > 0 && i < MAX_NUMBER_OF_PLAYERS) {
        draw_paddle(my_win, &m.paddles[i], true, '_');
        i++;
    }
}