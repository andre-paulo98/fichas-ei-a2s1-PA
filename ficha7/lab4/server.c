#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"
#include "common.h"


int main(int argc, char *argv[]) 
{
	struct gengetopt_args_info args;

	// cmdline_parser: deve ser a primeira linha de código no main
	if( cmdline_parser(argc, argv, &args) )
		ERROR(99, "Erro: execução de cmdline_parser\n");

	int tcp_server_socket;
	if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ERROR(51, "Can't create tcp_server_socket (IPv4)");



	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); 		// Todas as interfaces de rede
	tcp_server_endpoint.sin_port = htons(args.porto_arg);		// Server port
	if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
		ERROR(52, "Can't bind @tcp_server_endpoint");



	int tcp_max_simultaneous_clients = 1;
	if (listen(tcp_server_socket, tcp_max_simultaneous_clients)  == -1)
		ERROR(53, "Can't listen for %d clients", tcp_max_simultaneous_clients);



	char tcp_client_string_ip[20];
	struct sockaddr_in tcp_client_endpoint;
	socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
	int tcp_client_socket;
	printf("à espera da ligação do cliente... "); fflush(stdout);
	if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1)
		ERROR(54, "Can't accept client");
	printf("ok. \n");

	sleep(2);

	printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));

	char num_str[PROTOCOL_MAX_BUFFER];
	ssize_t bytes = recv(tcp_client_socket, num_str, PROTOCOL_MAX_BUFFER, 0);
	if(bytes == -1) {
		ERROR(1, "recv() failed");
	}

	num_str[bytes] = 0; // igual a adicionar o \0 no final da string
	printf("recieved: %s\n", num_str);


	bytes = recv(tcp_client_socket, num_str, PROTOCOL_MAX_BUFFER, 0);
	if(bytes == -1) {
		ERROR(1, "recv() failed");
	}

	num_str[bytes] = 0; // igual a adicionar o \0 no final da string
	printf("recieved: %s\n", num_str);


	uint16_t number = strtol(num_str, NULL, 10);

	printf("Sent: %hu\n", number);
	number = htons(number);

	if(send(tcp_client_socket, &number, sizeof(uint16_t), 0) == -1) {
		ERROR(1, "sent() failed");
	}


	if (close(tcp_client_socket) == -1)
		ERROR(55, "Can't close tcp_client_socket (IPv4)");

	// TCP IPv4: fecha socket (server)
	if (close(tcp_server_socket) == -1)
		ERROR(56, "Can't close tcp_server_socket (IPv4)");
	printf("ligação fechada. ok. \n");

	// libertar recurso (cmdline_parser)
	cmdline_parser_free(&args);	

	exit(0);
}	
