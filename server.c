#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Serverfunc.h"
#include "sock_dg_inet.h"


int main(){
    message_server m;
    message_client m_client;
    struct sockaddr_in client_addr;
    struct sockaddr_in *Players= malloc(MAX_NUMBER_OF_PLAYERS* sizeof(client_addr));
    struct paddle_position_t *Players_paddle= malloc(MAX_NUMBER_OF_PLAYERS *sizeof(struct paddle_position_t));
    int *Players_score= malloc(MAX_NUMBER_OF_PLAYERS * sizeof(int));
    int Num_players=0;
    int Counter=0;
    int sock_fd=Socket_creation();
    Socket_identification(sock_fd);
    place_ball_random(&m.ball);

    //Listening to messages
    while (1){
        Receive_message_server(sock_fd, &m_client, &client_addr);
        printf("received message\n");

        switch (m_client.type)
        {
        case 1:
            /* add client to list */
            Players[Num_players]=client_addr;
            new_paddle(&Players_paddle[Num_players], PADLE_SIZE, Players_paddle, Num_players);
            Players_score[Num_players]=0;

            if(Num_players+1<MAX_NUMBER_OF_PLAYERS)
                m.paddles[Num_players+1].length=-1;

            Num_players++;
            m.paddles[0]=Players_paddle[Num_players-1];
            for(int i=0; i<Num_players-1; i++){
                m.paddles[i+1]=Players_paddle[i];
            }
            Send_Reply_server(sock_fd, &m, &Players[Num_players-1]);
            printf("sent message\n");
            break;
        case 2:
            /* paddle move - update ball on screen */
            Counter++;
            if (Counter >= Num_players) {
                moove_ball(&m.ball, Players_paddle, Num_players, Players_score);
                Counter=0;
            }
            int i = 0;
            for (i = 0; i < Num_players; i++){
                if (client_addr.sin_addr.s_addr == Players[i].sin_addr.s_addr && client_addr.sin_port == Players[i].sin_port ){
                    moove_paddle(&Players_paddle[i], Players_paddle, m_client.key, Num_players, i);
                    m.paddles[0] = Players_paddle[i];
                    break;
                }
            }
            for (int j = 0; j < Num_players; j++){
                if (j < i)
                    m.paddles[j+1] = Players_paddle[j];
                else if (j > i)
                    m.paddles[j] = Players_paddle[j];
                else
                    continue;
            }
            m.score = Players_score[i];
            Send_Reply_server(sock_fd, &m, &client_addr);
            printf("sent message\n");
            break;
        case 4:
            /* disconnect - remove user from list */
            //Players[Curr_Player]=Players[Num_players-1];
            for (i = 0; i < Num_players; i++){
                if (client_addr.sin_addr.s_addr == Players[i].sin_addr.s_addr && client_addr.sin_port == Players[i].sin_port ){
                    break;
                }
            }
            Players[i] = Players[Num_players - 1];
            Players_paddle[i] = Players_paddle[Num_players - 1];
            Players_score[i] = Players_score[Num_players - 1];
            Players_paddle[Num_players].length = -1;
            Num_players--;
            break;
        default:
            perror("invalid message type");
            exit(-1);
            break;
        }
    }
    free(Players);
}