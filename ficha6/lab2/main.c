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

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

    unsigned short test = 0x1122;

    if(htons(test) == test){
        printf("Big Endian\n");
    } else {
        printf("Little Endian\n");
    }

    /* Verificar tamanho do tipo short */


    /* Determinar endianess da máquina local */

	return 0;
}

