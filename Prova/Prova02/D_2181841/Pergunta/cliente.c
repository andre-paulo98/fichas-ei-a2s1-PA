#include <stdio.h>
#include <signal.h>
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

#define EXIT_SIGACTION_FAILURE 55
#define EXIT_TIMEOUT_EXPIRED 56

void trata_sinal(int signal);

int timeout = -1;
int main(int argc, char *argv[])
{
    int socket_client;
    struct sockaddr_in ser_addr;

    /* Processa os parâmetros da linha de comando */
    struct gengetopt_args_info args;
    if (cmdline_parser(argc, argv, &args) != 0){
	    fprintf(stderr,"ERROR: cmdline_parser\n");
	    exit(EXIT_SYSCALL_FAILURE);
    }

    if(args.porto_arg <= 0 || args.porto_arg > 65535) {
        fprintf(stderr, "ERROR: invalid port '%d' ([1,65535])\n", args.porto_arg);
        exit(INPUT_ERROR_PORTA_INVALID);
    } else if (args.timeout_given == 1 && (args.timeout_arg < 2 || args.timeout_arg > 32)) {
        fprintf(stderr, "ERROR: invalid value '%d' for timeout [2,32]\n", args.timeout_arg);
        exit(INPUT_ERROR_TIMEOUT_INVALID);
    }

    socket_client = my_udp_socket();

	/* preenche estrutura: ip/porto do servidor */
    ser_addr = my_set_server_info(args.ip_arg, args.porto_arg);

    printf("[CLIENT] Sending data to server.\n");
    char letra = args.letter_arg[0];
    ssize_t bytes_sent = sendto(socket_client, &letra, sizeof(char), 0, (struct sockaddr *) &ser_addr, sizeof(struct sockaddr_in));
    if (bytes_sent == -1)
        ERROR(EXIT_SENDTO_FAILURE, "Can't sendto server");

    printf("[CLIENT] Data sent (%ld byte(s) sent).\n", bytes_sent);
    printf("[CLIENT] Waiting for server's answer\n");

    if(args.timeout_given == 1) {
        alarm(args.timeout_arg);
        struct sigaction act;
        timeout = args.timeout_arg;
        act.sa_handler = trata_sinal;

        /* mascara sem sinais -- nao bloqueia os sinais */
        sigemptyset(&act.sa_mask);

        act.sa_flags = 0;           /*fidedigno*/
        act.sa_flags |= SA_RESTART; /*recupera chamadas bloqueantes*/

        /* Captura do sinal SIGUSR1 */
        if(sigaction(SIGALRM, &act, NULL) < 0){
            ERROR(EXIT_SIGACTION_FAILURE, "sigaction - SIGALRM");
        }
    }

    int16_t speed;
    socklen_t from_addr_len = sizeof(struct sockaddr_in);
    if ((recvfrom(socket_client, &speed, sizeof(int16_t), 0, (struct sockaddr *) &ser_addr, &from_addr_len)) == -1)
        ERROR(EXIT_RECVFROM_FAILURE, "Can't recvfrom server");

    speed = ntohs(speed);
    printf("[CLIENT] Top speed for letter '%c'=%d kph\n", letra, speed);

	close(socket_client);

    cmdline_parser_free(&args);
    
    return (0);
}

void trata_sinal(int signal){
    if(signal == SIGALRM) {
        printf("[CLIENT] Timeout (%d secs) expired for recvfrom: quiting\n", timeout);
        exit(EXIT_TIMEOUT_EXPIRED);
    }
}