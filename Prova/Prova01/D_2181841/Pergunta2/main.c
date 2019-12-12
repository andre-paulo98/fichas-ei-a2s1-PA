#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "args.h"
#include "debug.h"

#define MIN_THREADS 3
#define MAX_THREADS 20

#define MIN_SIZE 5

#define MIN_ROUNDS 3
#define MAX_ROUNDS 10

#define NANOSECONDS_SLEEP 10000

#define RANDOM_MAX 100
#define RANDOM_MIN 1

#define ERROR_GENGETOPT_PARSER 1
#define ERROR_CREATE_FORK 2
#define PTHREAD_MUTEX_INIT 3
#define PTHREAD_COND_INIT 4 
#define PTHREAD_CREATE 5
#define PTHREAD_JOIN 6
#define PTHREAD_MUTEX_DESTROY 7



void *workers(void *arg);
void *cleaner(void *arg);

typedef struct{
	int n;
	int buffer[50];
	int index_escrita;
	int max_escrita;
	int rounds;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} thread_params_t;

int needsData = 1;

int main (int argc, char *argv[]) {

    struct gengetopt_args_info args;
	if(cmdline_parser(argc, argv, &args))
		ERROR(ERROR_GENGETOPT_PARSER, "Erro: execução de cmdline_parser\n");

	// VERIFICACAO DE ARGUMENTOS
	if(args.threads_arg < MIN_THREADS || args.threads_arg > MAX_THREADS){
		printf("ERR: threads_arg must be within [%d,%d] (%d given)\n", MIN_THREADS, MAX_THREADS, args.threads_arg);
		exit(0);
	}

	if(args.size_arg < MIN_SIZE){
		printf("ERR: size_arg must be atleast %d (%d given)\n", MIN_SIZE, args.size_arg);
		exit(0);
	}

	if(args.rounds_arg < MIN_ROUNDS || args.rounds_arg > MAX_ROUNDS) {
		printf("ERR: rounds_arg must be within [%d,%d] (%d given)\n", MIN_ROUNDS, MAX_ROUNDS, args.rounds_arg);
		exit(0);
	}

	int nThreads = args.threads_arg;
	int size = args.size_arg;
	int rounds = args.rounds_arg;



	thread_params_t params;
	memset(params.buffer, 0, sizeof(params.buffer));
	params.index_escrita = 0;
	params.max_escrita = size;
	params.rounds = rounds;


	pthread_t tids[nThreads];

	if ((errno = pthread_mutex_init(&params.mutex, NULL)) != 0)
		ERROR(PTHREAD_MUTEX_INIT, "pthread_mutex_init() failed!");

	if ((errno = pthread_cond_init(&params.cond, NULL)) != 0)
		ERROR(PTHREAD_COND_INIT, "pthread_cond_init() failed!");

	for (int i = 0; i < nThreads; i++){
		params.n = i;
		if (pthread_create(&tids[i], NULL, workers, &params) != 0) {
			ERROR(PTHREAD_CREATE, "pthread_create() failed!\n");
		}
	}

	pthread_t tid_cleaner;
	if ((errno = pthread_create(&tid_cleaner, NULL, cleaner, &params) != 0))
		ERROR(PTHREAD_CREATE, "Erro no pthread_create()!");

	for (int i = 0; i < nThreads; i++){
		if (pthread_join(tids[i], NULL) != 0) {
			ERROR(PTHREAD_JOIN, "pthread_join() failed!\n");
		}
	}
	// espera que a thread para limpar termine	
	if ((errno = pthread_join(tid_cleaner, NULL)) != 0)
		ERROR(PTHREAD_JOIN, "Erro no pthread_join()!\n");
	

	

	if ((errno = pthread_mutex_destroy(&params.mutex)) != 0)
		ERROR(PTHREAD_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");
	
	/* Destroi a condicao */
	if ((errno = pthread_cond_destroy(&params.cond)) != 0)
		ERROR(PTHREAD_MUTEX_DESTROY, "pthread_cond_destroy() failed!");




	cmdline_parser_free(&args);	
	return 0;
}


void *cleaner(void *arg) {
	thread_params_t *params = (thread_params_t *) arg;
	for (int i = 0; i < params->rounds; i++) {
			
		printf("[CLEANER]: about to start cleaning...\n");
		for (int i = 0; i < params->max_escrita; i++)
		{
			printf("%d|", params->buffer[i]);
			params->buffer[i] = 0;
		}
		printf("[CLEANER]: cleaning done\n");
		params->index_escrita = 0;
		
		sleep(1);
		/*struct timespec delay_ms = {0};
		delay_ms.tv_sec = 1;
		delay_ms.tv_nsec = 1000000 * 1;	*/
		//nanosleep(delay_ms, NULL);

	}
	
	needsData = 0;
	exit(1);
	return 0;
}

void *workers(void *arg) {
	unsigned int seedp = pthread_self();
	thread_params_t *params = (thread_params_t *) arg;


	while (needsData == 1) {
	
		if ((errno = pthread_mutex_lock(&(params->mutex))) != 0) {
			WARNING("pthread_mutex_lock() failed\n");
			return NULL;
		}

		while (params->index_escrita == params->max_escrita) {
			if ((errno = pthread_cond_wait(&(params->cond), &(params->mutex))) != 0) {
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		
		int num = (rand_r(&seedp) % RANDOM_MAX) + RANDOM_MIN;
		printf("[WORKER:%d] writing '%d' at position %d\n", params->n, num, params->index_escrita);
		params->buffer[params->index_escrita] = num;

		params->index_escrita++;

		if ((errno = pthread_mutex_unlock(&(params->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}
	
	printf("[WORKER:%d] All rounds done\n", params->n);


	return 0;
}