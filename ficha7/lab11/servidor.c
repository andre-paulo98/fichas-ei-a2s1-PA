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

void processaCliente(int fd);

int main(int argc, char *argv[])
{
    int ser_fd, cli_fd;
	socklen_t cli_len;
    struct sockaddr_in cli_addr;

    /* Processa os parâmetros da linha de comando */
    struct gengetopt_args_info args_info;
    if (cmdline_parser(argc, argv, &args_info) != 0){
	    fprintf(stderr, "ERROR: cmdline_parser");
	    exit(C_ERRO_CMDLINE);
    }

    // criação do socket para servidor
    ser_fd = my_tcp_socket();

    /* preenche estrutura: ip/porto do servidor */
    my_bind(ser_fd, INADDR_ANY, args_info.porto_arg);

    my_tcp_listen(ser_fd, 5);

    printf("Servidor %s no porto %d\n", argv[0], args_info.porto_arg);

    /* ciclo infinito para atender todos os clientes */
    while (1) {
        cli_fd = my_tcp_accept(ser_fd, (struct sockaddr *) &cli_addr);

        /* mostra informação sobre o cliente e proceasdasdasdsasa pedido */
        char ip[20];
        DEBUG("cliente [%s@%d]", inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof(ip)), ntohs(cli_addr.sin_port));
        processaCliente(cli_fd);

        /* liberta recursos utilizados com este cliente*/
        my_close(cli_fd);
    }

    return (0);
}


void processaCliente(int fd) 
{
    uint16_t  n_cli, n_serv, res;

    srand(time(NULL));
    n_serv = 1 + (uint16_t) (100.0 * rand() / (RAND_MAX + 1.0));
    DEBUG("número escolhido: %d\n", n_serv);

    do {
        /* recebe dados do cliente - chamada bloqueante */
        my_receive(fd, &n_cli, sizeof(uint16_t), 0);

        n_cli = ntohs(n_cli);

        if (n_cli == n_serv)
            res = IGUAL;
        else if (n_cli < n_serv)
            res = MENOR;
        else
            res = MAIOR;

        res = htons(res);

        /* envia resposta ao cliente */
        my_send(fd, &res, sizeof(uint16_t), 0);
            
    } while (n_cli != n_serv);
}
