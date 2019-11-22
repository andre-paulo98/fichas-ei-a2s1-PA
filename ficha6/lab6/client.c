/**
 * @file: client.c
 * @date: 2016-11-17
 * @author: autor
 */
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#include "debug.h"
#include "common.h"
#include "client_opt.h"

struct sockaddr_in get_server_addr(const char *ip, unsigned short port);
void do_request(int socket, const struct sockaddr_in *server_addr);

int main(int argc, char *argv[]){
	/* Estrutura gerada pelo utilitario gengetopt */
	struct gengetopt_args_info args;

	/* Processa os parametros da linha de comando */
	if (cmdline_parser (argc, argv, &args) != 0){
		exit(ERR_ARGS);
	}

	/*
	 * Inserir codigo do servidor
	 */
	int port = args.port_arg;
	if(port < 1 || port > USHRT_MAX){
		fprintf(stderr, "Invalid port value: %d, accepted values [1-%hu].\n", port, USHRT_MAX);
		exit(5);
	}

	int udp_client_socket;
	if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(EXIT_SYSCALL_FAILURE, "Can't create udp_client_socket (IPv4)");

	struct sockaddr_in server_addr = get_server_addr(args.ip_arg, args.port_arg);

	do_request(udp_client_socket, &server_addr);

	cmdline_parser_free(&args);
	close(udp_client_socket);

	return 0;
}

struct sockaddr_in get_server_addr(const char *ip, unsigned short port) {

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	switch (inet_pton(AF_INET, ip, &addr.sin_addr)) {
		case 0:
			fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
			        __FILE__, __LINE__);
			exit(EXIT_SYSCALL_FAILURE);
		case -1:
			ERROR(EXIT_SYSCALL_FAILURE, "Cannot convert IP address (IPv4)");
	}
	addr.sin_port = htons(port);

	return addr;
}

void do_request(int socket, const struct sockaddr_in *server_addr) {
	srandom(time(NULL));

	uint16_t number = random() % 100;

	printf("Sending %hu\n", number);
	number = htons(number);


	if (sendto(socket, &number, sizeof(uint16_t), 0, (struct sockaddr *) server_addr,
	                             sizeof(struct sockaddr_in)) == -1)
	ERROR(EXIT_SYSCALL_FAILURE, "Can't sendto server");

	struct sockaddr_in from_addr;
	socklen_t from_addr_len = sizeof(struct sockaddr_in);
	char buffer[MAX_MSG_SIZE + 1];
	ssize_t read_bytes;
	if ((read_bytes = recvfrom(socket, buffer, MAX_MSG_SIZE, 0, (struct sockaddr *) &from_addr, &from_addr_len)) == -1)
		ERROR(EXIT_SYSCALL_FAILURE, "Can't recvfrom server");

	buffer[read_bytes] = 0;
	printf("result: %s\n", buffer);

}