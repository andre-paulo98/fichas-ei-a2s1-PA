#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "debug.h"
#include "comum.h"
#include "servidor_opt.h"
#include "my_functions.h"

int main(int argc, char *argv[])
{
    int socket_server, socket_client;
    struct sockaddr_in cli_addr;

    struct gengetopt_args_info args;
    if (cmdline_parser(argc, argv, &args) != 0){
	    fprintf(stderr, "ERROR: cmdline_parser");
	    exit(CMDLINE_PARSER_ERROR);
    }

    // Cria o socket para o servidor
    socket_server = my_tcp_socket();

    // Preenche a estrutura de dados
    my_bind(socket_server, INADDR_ANY, args.porto_arg);

    // listen da porta
    my_tcp_listen(socket_server, 5);

    printf("Servidor %s no porto %d\n", argv[0], args.porto_arg);

    /* ciclo infinito para atender todos os clientes */
    while (1) {
        socket_client = my_tcp_accept(socket_server, &cli_addr);

        /* mostra informação sobre o cliente e processa pedido */
        char ip[20];
        DEBUG("cliente [%s@%d]", inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof(ip)), ntohs(cli_addr.sin_port));

        char question[10];

        // espera até receber uma resposta
        ssize_t bytes = my_receive(socket_client, &question, sizeof(char[10]), 0);
        question[bytes] = '\0';
        printf("QUESTION FROM CLIENT: '%s'\n", question);

        char pong[] = "pong";

        my_send(socket_client, &pong, strlen(pong), 0);
        printf("REPLIED '%s'.\n", pong);

        /* liberta recursos utilizados com este cliente*/
        my_close(socket_client);
    }
    cmdline_parser_free(&args);
    return (0);
}
