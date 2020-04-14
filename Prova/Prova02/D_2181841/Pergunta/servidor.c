#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "debug.h"
#include "comum.h"
#include "servidor_opt.h"
#include "my_functions.h"

#define QUANTITY_TYPES 12


int bind_udp_socket(unsigned short port);
void handle_request(unsigned short udp_server);

char letras[QUANTITY_TYPES]     = {'L', 'M', 'N', 'Q', 'R', 'S', 'T', 'U', 'H', 'V', 'W', 'Y'};
int16_t veloMax[QUANTITY_TYPES] = {120, 130, 140, 160, 170, 180, 190, 200, 210, 240, 270, 300};
int main(int argc, char *argv[])
{
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser(argc, argv, &args) != 0){
        exit(1);
    }

    int port = args.porto_arg;
    if(port < 1 || port > USHRT_MAX){
        fprintf(stderr, "ERROR: invalid port '%d' ([1,65535])\n", port);
        exit(INPUT_ERROR_PORTA_INVALID);
    }

    int udp_server = bind_udp_socket(port);
    printf("[SERVER] Waiting for clients UDP/%d\n", port);

    while(1) {
        handle_request(udp_server);
    }

    cmdline_parser_free(&args);
    close(udp_server);

    return 0;
}



int bind_udp_socket(unsigned short port) {
    int udp_client_socket;
    if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        ERROR(EXIT_SYSCALL_FAILURE, "Can't create udp_client_socket (IPv4)");

    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(port);	// Server port
    if (bind(udp_client_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1) {
        close(udp_client_socket);
        ERROR(EXIT_SYSCALL_FAILURE, "Can't bind @udp_server_endpoint info");
    }

    return udp_client_socket;
}

void handle_request(unsigned short socket) {
    ssize_t read_bytes;
    char letra;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    if ((read_bytes = recvfrom(socket, &letra, sizeof(letra), 0, (struct sockaddr *) &client_addr, &client_addr_len)) == -1)
        ERROR(EXIT_RECVFROM_FAILURE, "Can't recvfrom client");

    printf("[SERVER] %ld byte(s) recieved ('%c')\n", read_bytes, letra);

    int16_t speed = -1;
    for (int i = 0; i < QUANTITY_TYPES; i++) {
        if(letra == letras[i]) {
            speed = veloMax[i];
            break;
        }
    }

    if(speed != -1) {
        speed = htons(speed);
        printf("[SERVER] Sending answer to client\n");

        ssize_t bytes_sent = sendto(socket, &speed, sizeof(int16_t), 0, (struct sockaddr *) &client_addr,
                                    client_addr_len);
        if (bytes_sent == -1)
            ERROR(EXIT_SENDTO_FAILURE, "Can't sendto client");

        printf("[SERVER] Answer sent to client (%ld bytes(s))\n", bytes_sent);
    }
}