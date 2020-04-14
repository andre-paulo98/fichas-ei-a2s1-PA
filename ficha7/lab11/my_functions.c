/**
 * @file my_functions.c
 * @brief Library feita com base nos auxiliares fornecidos
 * @date 12/12/2019
 * @author Andre Carvalho - 2181840
 * @author Andre Paulo - 2181841
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "debug.h"
#include "my_functions.h"

//// ---------------------------------
////            UDP
//// ---------------------------------

int my_udp_socket(){
    int udp_client_socket;
    if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(UPD_OPEN_ERROR, "Can't create udp_client_socket (IPv4)");
    return udp_client_socket;
}


//// ---------------------------------
////            TCP
//// ---------------------------------


int my_tcp_socket(void) {
    int tcp_client_socket;
    if((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 1) {
        ERROR(TCP_OPEN_ERROR, "Error creating tcp_client_socket (IPv4)");
    }
    return tcp_client_socket;
}

struct sockaddr_in my_tcp_client_info(int socket){
    struct sockaddr_in client;
    socklen_t length = sizeof(struct sockaddr_in);
    if (getsockname(socket, (struct sockaddr *)&client, &length) == -1)
        ERROR(TCP_CLI_INFO_ERROR, "Can't connect @tcp_server_endpoint");
    return client;
}

int my_tcp_listen(int socket, int queue_max) {
    int r_listen = listen(socket, queue_max);
    if(r_listen == -1){
        ERROR(TCP_LISTEN_ERROR, "Error on Listen.");
    }
    return r_listen;
}

void my_connect(int socket, struct sockaddr_in * server){
    if (connect(socket, (struct sockaddr *) server, sizeof(struct sockaddr_in)) == -1)
        ERROR(TCP_CONNECT_ERROR, "Can't connect @tcp_server_endpoint");
}

int my_tcp_accept(int socket, struct sockaddr_in *client){
    socklen_t cli_len = sizeof(struct sockaddr);
    int client_socket = accept(socket, (struct sockaddr *) client, &cli_len);
    if(client_socket < 0) {
        ERROR(TCP_ACCEPT_ERROR, "Error on Accept.");
    }
    return client_socket;
}


//// ---------------------------------
////            COMMON
//// ---------------------------------


struct sockaddr_in my_set_server_info(const char * ip, short port ){

    struct sockaddr_in udp_endpoint;
    memset(&udp_endpoint, 0, sizeof(struct sockaddr_in));
    udp_endpoint.sin_family = AF_INET;
    switch (inet_pton(AF_INET, ip, &udp_endpoint.sin_addr.s_addr)){
        case 0:
            fprintf(stderr,"Cannot convert IP address (IPv4): Invalid Network Address\n");
            exit(IP_CONVERT_ERROR);
            break;
        case -1:
            fprintf(stderr,"Cannot convert IP address (IPv4)");
            exit(IP_CONVERT_ERROR);
    }
    udp_endpoint.sin_port = htons(port);

    return udp_endpoint;
}

void my_bind(int socket, in_addr_t interfaces, short port){
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(interfaces);
    udp_server_endpoint.sin_port = htons(port);
    if (bind(socket, (struct sockaddr *)&udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
        ERROR(BIND_ERROR, "Can't bind ip and port to the interface");
}

/// SEND
ssize_t my_send(int socket, const void *data, size_t length, int flags) {
    return my_send_to(socket, data, length, flags, NULL);
}

ssize_t my_send_to(int socket, const void *data, size_t length, int flags, struct sockaddr_in *dest_addr){
    ssize_t sent_bytes;

    if ((sent_bytes = sendto(socket, data, length, flags, (struct sockaddr *)dest_addr, sizeof(struct sockaddr_in))) == -1)
        ERROR(SEND_ERROR, "Can't send the data!");
    return sent_bytes;
}

/// RECEIVE
ssize_t my_receive(int socket, void *buffer, size_t length, int flags){
    return my_receive_from(socket, buffer, length, flags, NULL);
}

ssize_t my_receive_from(int socket, void *buffer, size_t length, int flags, struct sockaddr_in *orig_addr){
    ssize_t read_bytes;
    socklen_t addr_length = sizeof(struct sockaddr_in);

    if ((read_bytes = recvfrom(socket, buffer, length, flags, (struct sockaddr *)orig_addr, &addr_length)) == -1)
        ERROR(RECV_ERROR, "Can't recvfrom server");
    return read_bytes;
}

void my_close(int socket){
    if (close(socket) == -1)
        ERROR(CLOSE_SOCKET_ERROR, "Can't close the UDP socket!");
}