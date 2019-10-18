#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include "args.h"
#include "debug.h"

#define ERROR_PTHREAD_CREATE    1
#define ERROR_PTHREAD_JOIN      2
#define ERROR_FORK              3
#define NUMBER_OF_CHILDS        3
#define C_ERRO_MUTEX_INIT       4
#define C_ERRO_MUTEX_DESTROY    5

// -------  PROTÓTIPOS -----------------
void *sums(void *arg);

int sum = 0;

struct ThreadParams{
	int n;
	pthread_mutex_t mutex;
};

// ----------  MAIN  -------------------
int main(int argc, char *argv[])
{

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	int num_threads = args.threads_arg;
	struct ThreadParams params;
	params.n = args.num_arg;

	pthread_t tids[num_threads];

	if ((errno = pthread_mutex_init(&params.mutex, NULL)) != 0)
		ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

	for (int i = 0; i < num_threads; i++){
		if (pthread_create(&tids[i], NULL, sums, &params) != 0) {
			printf("pthread_create() failed!\n");
			exit(ERROR_PTHREAD_CREATE);
		}
	}

	for (int i = 0; i < num_threads; i++){
		if (pthread_join(tids[i], NULL) != 0) {
			printf("pthread_join() failed!\n");
			exit(ERROR_PTHREAD_JOIN);
		}
	}

	printf("sum = %d\n", sum);

	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	/* Destroi o mutex */
	if ((errno = pthread_mutex_destroy(&params.mutex)) != 0)
		ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");

	exit(0);
}


// ----------     FUNÇÕES    -------------------
void *sums(void *arg)
{
	struct ThreadParams *thParams = (struct ThreadParams *) arg;

	if ((errno = pthread_mutex_lock(&thParams->mutex)) != 0){
		WARNING("pthread_mutex_lock() failed");
		return NULL;
	}

	for (int i = 0; i < thParams->n; i++){
		int aux = sum;
		aux = aux + 1;
		sched_yield();
		sum = aux;
		//sum = sum + 1;
	}

	if ((errno = pthread_mutex_unlock(&thParams->mutex)) != 0){
		WARNING("pthread_mutex_unlock() failed");
		return NULL;
	}
	return NULL;
}