/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <assert.h>
#include <limits.h>
#include <arpa/inet.h>

#include "debug.h"
#include "memory.h"
#include "args.h"

int bind_udp_socket(unsigned short port);

int main(int argc, char *argv[]){
    /* Receber porto a registar */
    struct gengetopt_args_info args;
    cmdline_parser(argc, argv, &args);

    int port = args.port_arg;
    if(port < 1 || port > USHRT_MAX){
    	fprintf(stderr, "Invalid port value: %d, accepted values [1-%hu].\n", port, USHRT_MAX);
    	exit(5);
    }

    int udp_server = bind_udp_socket(port);
    printf("Listening on port %d.\n", udp_server);

	close(udp_server);

    /* Liberta memória */
    cmdline_parser_free(&args);

    return 0;
}

int bind_udp_socket(unsigned short port) {
	int udp_client_socket;
	if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(21, "Can't create udp_client_socket (IPv4)");

	struct sockaddr_in udp_server_endpoint;
	memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
	udp_server_endpoint.sin_family = AF_INET;
	udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
	udp_server_endpoint.sin_port = htons(port);	// Server port
	if (bind(udp_client_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1) {
		close(udp_client_socket);
		ERROR(32, "Can't bind @udp_server_endpoint info");
	}

	return udp_client_socket;
}

/*
 * Perguntas:
 *  a)  O que sucede quando é indicado um porto entre 1 e 1023 (inclusive)?
 *  b)  O que sucede quando se tenta registar um porto que já está registado?
 *      Nota: é possível obter uma lista dos portos UDP registados no sistema através do utilitário netstat, executado da seguinte forma: netstat -u -l
*/

