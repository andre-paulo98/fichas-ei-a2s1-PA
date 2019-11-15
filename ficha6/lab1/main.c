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

#include "debug.h"
#include "memory.h"
#include "args.h"

void memory_dump(void *addr, size_t size);

int main(int argc, char *argv[]){
    /* Receber endereço IPv4 em formato texto */
    struct gengetopt_args_info args;
    cmdline_parser(argc, argv, &args);

    /* Converter e mostrar endereço em formato: byte.byte.byte.byte */
    struct in_addr ipv4_addr;
    switch (inet_pton(AF_INET, args.address_arg, &ipv4_addr))
    {
        case -1:
            ERROR(1, "inetpton() failed");
        case 0: 
            fprintf(stderr, "%s is not a valid IPv4 dotted-decimal address", args.address_arg);
            exit(2);
        default:
            printf("IPv4 network address: ");
            memory_dump(&ipv4_addr, sizeof(ipv4_addr));
            break;
    }

    /* Libertar memória alocada pelo gengetopt */
    cmdline_parser_free(&args);
	return 0;
}

void memory_dump(void *addr, size_t size) {
    unsigned char *ptr = addr;
    printf("0x");
    for(int i = 0; i < size; ++i) {
        printf("%02x ", ptr[i]);
    }
    printf("\n");
}

