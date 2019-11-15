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

void memory_dump(void *addr, size_t size);

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

    /* Mostrar tamanho (em bytes) do tipo struct sockaddr_in */
    printf("sizeof(struct sockaddr_in) = %zu\n", sizeof(struct sockaddr_in));

    /* Colocar a "zero" uma variável (my_addr_IPv4) do tipo struct sockaddr_in */
    struct sockaddr_in my_addr_IPv4;
    memset(&my_addr_IPv4, 0, sizeof(struct sockaddr_in));

    /* Mostrar bytes da memória da variável my_addr_IPv4 (criar e chamar função)*/
    printf("Memory Dump: ");
    memory_dump(&my_addr_IPv4, sizeof(my_addr_IPv4));

    /* Colocar a "4" os primeiros 6 bytes da variável my_addr_IPv4 */
    memset(&my_addr_IPv4, 4, 6);

    /* Mostrar bytes da memória da variável my_addr_IPv4 (chamar função)*/
    printf("Memory Dump: ");
    memory_dump(&my_addr_IPv4, sizeof(my_addr_IPv4));

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