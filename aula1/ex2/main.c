#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "debug.h"



int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	(void)argc;
	(void)argv;


	int fatherPID = getpid();
	int r = fork();
	if(r == 0) {
		printf("Eu sou o Filho 1\n");
		int r3 = fork();
		if (r3 == 0){
			printf("Eu sou o neto\n");
		}
	} else if(getpid() == fatherPID){
		int r1 = fork();
		if(r1 == 0) {
			printf("Eu sou o Filho 2\n");
		} else {
			printf("Eu sou o pai\n");
		}

	}


	return 0;
}
