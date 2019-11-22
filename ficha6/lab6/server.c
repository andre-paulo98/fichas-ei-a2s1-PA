/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <math.h>


#include "debug.h"
#include "common.h"
#include "server_opt.h"

int bind_udp_socket(unsigned short port);
void handle_request(unsigned short udp_server);

int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args) != 0){
        exit(ERR_ARGS);
    }

	int port = args.port_arg;
	if(port < 1 || port > USHRT_MAX){
		fprintf(stderr, "Invalid port value: %d, accepted values [1-%hu].\n", port, USHRT_MAX);
		exit(ERR_ARGS);
	}

	int udp_server = bind_udp_socket(port);
	printf("Listening on port %d.\n", port);

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
	uint16_t number;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(struct sockaddr_in);
	if ((read_bytes = recvfrom(socket, &number, sizeof(number), 0, (struct sockaddr *) &client_addr, &client_addr_len)) == -1)
		ERROR(34, "Can't recvfrom client");

	number = ntohs(number);
	char ip[MAX_MSG_SIZE];
	// mostrar IP do cliente e porta
	DEBUG("REQUEST FROM ADDR: %s:%hu\n", inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)), ntohs(client_addr.sin_port));

	printf("Recieved: %hu\n", number);

	double value = sqrt(number);

	char response[MAX_MSG_SIZE + 1];
	sprintf(response, "%.2lf", value);

	if ((sendto(socket, response, strlen(response), 0, (struct sockaddr *) &client_addr, client_addr_len)) == -1)
		ERROR(EXIT_SYSCALL_FAILURE, "Can't sendto client");
}