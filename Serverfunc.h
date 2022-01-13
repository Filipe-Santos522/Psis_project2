/* Communication functions header file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "single-pong.h"

// Types defined in "single-pong.c" are used, namely "message_server" and "message_client" 
// Headers of functions defined in the respective source code file
int Socket_creation();
void Socket_identification(int);
void Send_Reply_server(int, struct message_server *, struct sockaddr_in*);
void Send_Reply_client(int, struct message_client *, struct sockaddr_in*);
void Receive_message_server(int, struct message_client *, struct sockaddr_in*);
void Receive_message_client(int, struct message_server *, struct sockaddr_in*);
