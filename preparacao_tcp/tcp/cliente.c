#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "debug.h"
#include "comum.h"
#include "cliente_opt.h"
#include "my_functions.h"

int main(int argc, char *argv[])
{
    int socket_client;
    struct sockaddr_in ser_addr;

    /* Processa os parâmetros da linha de comando */
    struct gengetopt_args_info args;
    if (cmdline_parser(argc, argv, &args) != 0){
	    fprintf(stderr,"ERROR: cmdline_parser\n");
	    return 0;
	    //exit(C_ERRO_CMDLINE);
    }

	/* preenche estrutura: ip/porto do servidor */
    ser_addr = my_set_server_info(args.ip_arg, args.porto_arg);
		
    /* cria um socket */
    socket_client = my_tcp_socket();

    /* faz ligação ao servidor */
    my_connect(socket_client, &ser_addr);

    char question[] = "ping";
    my_send(socket_client, question, strlen(question), 0);
    printf("Sent %s to server.\n", question);

    char reply[10];
    ssize_t bytes = my_receive(socket_client, &reply, sizeof(char[10]), 0);
    reply[bytes] = '\0';
    printf("Recieved '%s' from server.\n", reply);

	/* liberta recursos utilizados */
	close(socket_client);

    cmdline_parser_free(&args);
    
    return (0);
}
