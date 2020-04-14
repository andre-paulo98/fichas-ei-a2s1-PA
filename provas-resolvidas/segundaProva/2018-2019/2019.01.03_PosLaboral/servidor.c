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



   //return 0;

    // Cria o socket para o servidor
    socket_server = my_tcp_socket();

    // Preenche a estrutura de dados
    my_bind(socket_server, INADDR_ANY, args.porto_arg);

    // listen da porta
    my_tcp_listen(socket_server, 5);

    //printf("Servidor %s no porto %d\n", argv[0], args.porto_arg);

    /* ciclo infinito para atender todos os clientes */
    while (1) {
        socket_client = my_tcp_accept(socket_server, &cli_addr);

        /* mostra informação sobre o cliente e processa pedido */
        char ip[20];
        printf("[SERVER-main]:connection request from '%s' (port:%d)\n", inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof(ip)), ntohs(cli_addr.sin_port));


        unsigned long despesa;
        my_receive(socket_client, &despesa, sizeof(unsigned long), 0);
        despesa = ntohl(despesa);
        printf("[SERVER]debt=%ld\n", despesa);

        unsigned long pagamento;
        my_receive(socket_client, &pagamento, sizeof(unsigned long), 0);
        pagamento = ntohl(pagamento);
        printf("[SERVER]payment=%ld\n", pagamento);


        // calculo
        int trocos[2][9] = {{500, 200, 100, 50, 20, 10, 5, 2, 1},
                            {  0,   0,   0,  0,  0,  0, 0, 0, 0}};

        int qtTrocos = sizeof(trocos[0])/ sizeof(int);

        int pag = pagamento - despesa;
        while(pag != 0) {
            for (int i = 0; i < qtTrocos; i++) {
                if(trocos[0][i] <= pag){
                    pag = pag - trocos[0][i];
                    trocos[1][i] += 1;
                    break;
                }
            }
        }

        char reply[100] = "Despesa:";
        sprintf(reply, "%s%ld;Pagamento:%ld;Troco:%ld", reply, despesa, pagamento, (pagamento-despesa));
        int runFirst = 0;
        for(int i = 0; i < qtTrocos; i++) {
            if(trocos[1][i] != 0) {
                sprintf(reply, "%s%c%d*%d", reply, runFirst == 0 ? '=' : '+', trocos[1][i], trocos[0][i]);
                runFirst = 1;
            }
        }

        my_send(socket_client, &reply, strlen(reply), 0);
        printf("[SERVER] Answer to client: '%s'.\n", reply);

        /* liberta recursos utilizados com este cliente*/
        my_close(socket_client);
    }

    cmdline_parser_free(&args);
    return (0);
}
