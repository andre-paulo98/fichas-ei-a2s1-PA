#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "comum.h"
#include "cliente_opt.h"
#include "my_functions.h"

#define INPUT_ERROR_DESPESA_INVALID 61
#define INPUT_ERROR_PAGAMENTO_INVALID 62

int main(int argc, char *argv[])
{
    int socket_client;
    struct sockaddr_in ser_addr;

    /* Processa os parâmetros da linha de comando */
    struct gengetopt_args_info args;
    if (cmdline_parser(argc, argv, &args) != 0){
	    fprintf(stderr,"ERROR: cmdline_parser\n");
	    exit(CMDLINE_PARSER_ERROR);
    }

    if(args.porto_arg <= 0 || args.porto_arg > 65535) {
        fprintf(stderr, "ERROR: port %d out of range [1,65536[\n", args.porto_arg);
        exit(INPUT_ERROR_DESPESA_INVALID);
    } else if(args.despesa_arg <= 0) {
        fprintf(stderr, "[INPUT ERROR] 'despesa' invalid value '%d'. Must be > 0\n", args.despesa_arg);
        exit(INPUT_ERROR_DESPESA_INVALID);
    } else if (args.pagamento_arg < args.despesa_arg) {
        fprintf(stderr, "[INPUT ERROR] 'pagamento' (%d) invalid, must be greater than 'despesa' (%d)\n", args.pagamento_arg, args.despesa_arg);
        exit(INPUT_ERROR_PAGAMENTO_INVALID);
    }

    unsigned long despesa = args.despesa_arg;
    unsigned long pagamento = args.pagamento_arg;

	/* preenche estrutura: ip/porto do servidor */
    ser_addr = my_set_server_info(args.ip_arg, args.porto_arg);
		
    /* cria um socket */
    socket_client = my_tcp_socket();

    /* faz ligação ao servidor */
    my_connect(socket_client, &ser_addr);

    printf("[CLIENT] debt=%ld, payment=%ld\n", despesa, pagamento);

    despesa = htonl(despesa);
    pagamento = htonl(pagamento);

    my_send(socket_client, &despesa, sizeof(unsigned long), 0);
    my_send(socket_client, &pagamento, sizeof(unsigned long), 0);


    char reply[100];
    ssize_t bytes = my_receive(socket_client, &reply, sizeof(char[100]), 0);
    reply[bytes] = '\0';
    printf("[CLIENT]server=>'%s'\n", reply);

    cmdline_parser_free(&args);

	/* liberta recursos utilizados */
	close(socket_client);
    
    return (0);
}
