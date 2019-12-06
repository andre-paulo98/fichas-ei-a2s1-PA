#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"
#include "common.h"

char* handle_request(uint8_t request, char* response, size_t max_response_size);

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


	while(1) {

		char tcp_client_string_ip[20];
		struct sockaddr_in tcp_client_endpoint;
		socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
		int tcp_client_socket;
		printf("à espera da ligação do cliente... "); fflush(stdout);
		if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1)
			ERROR(54, "Can't accept client");
		printf("ok. \n");

		printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));

		uint8_t request;
		ssize_t bytes = recv(tcp_client_socket, &request, sizeof(uint8_t), 0);
		if(bytes == -1) {
			ERROR(1, "recv() failed");
		}
		char response[PROTOCOL_MAX_BUFFER];
		handle_request(request, response, PROTOCOL_MAX_BUFFER);

		if(send(tcp_client_socket, response, sizeof(response), 0) == -1) {
			ERROR(1, "sent() failed");
		}

		if (close(tcp_client_socket) == -1)
			ERROR(55, "Can't close tcp_client_socket (IPv4)");
	}

	// TCP IPv4: fecha socket (server)
	if (close(tcp_server_socket) == -1)
		ERROR(56, "Can't close tcp_server_socket (IPv4)");
	printf("ligação fechada. ok. \n");

	// libertar recurso (cmdline_parser)
	cmdline_parser_free(&args);	

	exit(0);
}


char* handle_request(uint8_t request, char* response, size_t max_response_size) {
	time_t t = time(NULL);
	struct tm *localtime_ptr;
	localtime_ptr = localtime(&t);

	if(localtime_ptr == NULL) {
		fprintf(stderr, "localtime() failed: %s\n", strerror(errno));
		response[0] = 0;
		return response;
	}

	switch(request){
		case DATE_TIME:
			printf("recieved DATETIME\n");
			strftime(response, max_response_size, "%d/%m/%Y_%H:%M:%S", localtime_ptr);
			break;
		case DATE:
			printf("recieved DATE\n");
			strftime(response, max_response_size, "%d/%m/%Y", localtime_ptr);
			break;
		case TIME:
			printf("recieved TIME\n");
			strftime(response, max_response_size, "%H:%M:%S", localtime_ptr);
			break;
	}
	return response;
}
