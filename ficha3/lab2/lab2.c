#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "debug.h"

int main (int argc, char *argv[]) {	
	
	/* Silencia os warnings */
	(void)argc;
	(void)argv;

 	printf("Nome do programa: %s\n", argv[0]);
 	printf("PID do processo atual: %d\n", getpid());
 	printf("Número de threads do processo atual: ");
 	fflush(stdout);
 	char cmd[100];

 	sprintf(cmd, "ps -eLf | grep %s | tr -s ' ' | cut -f6 -d' '", argv[0]);

 	int res = system(cmd);

 	if(res < 0){
 		ERROR(1, "ERROR ON SYSTEM FUNCTION");
 	}

	exit(0);
}
