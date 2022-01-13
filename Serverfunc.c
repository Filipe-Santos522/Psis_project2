/* Communication functions source code file by:
 * Filipe Santos - 90068
 * Alexandre Fonseca - 90210
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Serverfunc.h"
#include "sock_dg_inet.h"


//Function that creates a socket with error verification and returns its descriptor
int Socket_creation(){
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1){
        perror("socket: ");
        exit(-1);
    }
    return sock_fd;
}

// Function that binds a socket to an address and port with error verification
void Socket_identification(int sock_fd){
    
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port =htons(SOCK_PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }
}

// Function that sends a message from the server to the client with error verification
void Send_Reply_server(int sock_fd, message_server *reply_message, struct sockaddr_in* client_addr){
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int nbytes = sendto(sock_fd, reply_message, sizeof(message_server), 0,  (struct sockaddr *) client_addr, client_addr_size);
    if (nbytes == -1){
        perror("sendto");
        exit(-1);
    }
}

// Function that sends a message from the client to the server with error verification
void Send_Reply_client(int sock_fd, message_client *reply_message, struct sockaddr_in* client_addr){
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int nbytes = sendto(sock_fd, reply_message, sizeof(message_client), 0,  (struct sockaddr *) client_addr, client_addr_size);
    if (nbytes == -1){
        perror("sendto");
        exit(-1);
    }
}

// Function that receives a message sent from the client with error verification
void Receive_message_server(int sock_fd, message_client *ball, struct sockaddr_in* client_addr){
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int nbytes = recvfrom(sock_fd, ball, sizeof(*ball), 0, (struct sockaddr *) client_addr, &client_addr_size);
    if (nbytes == -1){
        perror("recvfrom");
        exit(-1);
    }
}

// Function that receives a message sent from the server with error verification
void Receive_message_client(int sock_fd, message_server *ball, struct sockaddr_in* client_addr){
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int nbytes = recvfrom(sock_fd, ball, sizeof(*ball), 0, (struct sockaddr *) client_addr, &client_addr_size);
    if (nbytes == -1){
        perror("recvfrom");
        exit(-1);
    }
}