/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "debug.h"
#include "memory.h"

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

    /* a) Criar um socket UDP / IPv4 */
	int udp_client_socket_ipv4;
	if ((udp_client_socket_ipv4 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERROR(21, "Can't create udp_client_socket (IPv4)");

    /* b) Criar um socket UDP / IPv6 */

	int udp_client_socket_ipv6;
	if ((udp_client_socket_ipv6 = socket(AF_INET6, SOCK_DGRAM, 0)) == -1)
		ERROR(21, "Can't create udp_client_socket (IPv6)");

    /* c) Mostrar descritores dos sockets */
    printf("Socket IPv4: %d, Socket IPv6: %d\n", udp_client_socket_ipv4, udp_client_socket_ipv6);

    /* d) Fechar os descritores dos sockets */
    close(udp_client_socket_ipv4);
    close(udp_client_socket_ipv6);

	return 0;
}
