#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "debug.h"

void *showDetails(void *arg);


int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	(void)argc;
	(void)argv;
	//

	int j = 0;
	int isFather = 0;

	// CRIAÇÃO DOS PROCESSOS FILHO
	do {
		int res = fork();
		if(res == 0) {
			isFather = 0;
		} else {
			isFather = 1;
		}
		j++;

	} while (j < 3 && isFather == 1);
	// ----




	int errno;
	if(isFather == 0) {

		pthread_t tid1, tid2;
		if ((errno = pthread_create(&tid1, NULL, showDetails, NULL)) != 0) {
			ERROR(1, "pthread_create() failed!");
		}
		if ((errno = pthread_create(&tid2, NULL, showDetails, NULL)) != 0) {
			ERROR(1, "pthread_create() failed!");
		}

		// espera que a thread termine
		if ((errno = pthread_join(tid1, NULL)) != 0)
			ERROR(11, "Erro no pthread_join()!\n");
		if ((errno = pthread_join(tid2, NULL)) != 0)
			ERROR(11, "Erro no pthread_join()!\n");
	}


	return 0;
}


void *showDetails(void *arg) {
	(void)arg;
	printf("PID Pai: %d, PID Filho: %d, TID: %lu \n\n", getppid(), getpid(), pthread_self());
	return NULL;
}