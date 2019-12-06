#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../debug/debug.h"
#include "../cmdline/client_cmdline.h"
#include "common.h"


int main(int argc, char *argv[]) 
{
	struct gengetopt_args_info args;

	// cmdline_parser: deve ser a primeira linha de código no main
	if( cmdline_parser(argc, argv, &args) )
		ERROR(99, "Erro: execução de cmdline_parser\n");

	int tcp_client_socket;
	if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ERROR(41, "Can't create tcp_client_socket (IPv4)");

	// -----------

	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	switch (inet_pton(AF_INET, args.ip_arg, &tcp_server_endpoint.sin_addr)) {
		case 0:
			fprintf(stderr, "[%s@%d] ERROR - Cannot convert IP address (IPv4): Invalid Network Address\n",
			        __FILE__, __LINE__);
		exit(22);
		case -1:
			ERROR(22, "Cannot convert IP address (IPv4)");
	}
	tcp_server_endpoint.sin_port = htons(args.porto_arg);

	// -----------

	printf("a ligar ao servidor... "); fflush(stdout);
	if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
		ERROR(43, "Can't connect @tcp_server_endpoint");
	printf("ok. \n");

	// -----------
	uint8_t request = DATE_TIME;

	if(send(tcp_client_socket, &request, sizeof(uint8_t), 0) == -1) {
		ERROR(1, "send() failed");
	}

	char response[PROTOCOL_MAX_BUFFER];

	size_t bytes = recv(tcp_client_socket, response, PROTOCOL_MAX_BUFFER, 0);
	if(bytes == -1) {
		ERROR(1, "recv() failed");
	}
	response[bytes] = '\0';
	printf("Recieved: %s\n", response);





	if (close(tcp_client_socket) == -1)
		ERROR(45, "Can't close tcp_client_socket (IPv4)");
	printf("ligação fechada. ok. \n");
	cmdline_parser_free(&args);
	exit(0);
}	
