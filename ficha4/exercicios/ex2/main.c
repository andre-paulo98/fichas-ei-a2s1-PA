#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include "args.h"
#include "debug.h"

#define NUM_INTERACTIONS 10
#define PING 1
#define PONG 2

typedef struct
{
	// como recebem a mesma estrutura não pode ser ponteiros
	pthread_mutex_t ptr_mutex;
	pthread_cond_t ptr_cond;
	int next;

}thread_params_t;

void *ping(void *arg);
void *pong(void *arg);

int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	(void)argc;
	(void)argv;



	thread_params_t thread_params;

	if ((errno = pthread_mutex_init(&thread_params.ptr_mutex, NULL)) != 0)
		ERROR(12, "pthread_mutex_init() failed");

	if ((errno = pthread_cond_init(&thread_params.ptr_cond, NULL)) != 0)
		ERROR(14, "pthread_cond_init() failed!");

	//
	pthread_t tid1;
	pthread_t tid2;
	thread_params.next = PING;

	if ((errno = pthread_create(&tid1, NULL, ping, &thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno = pthread_create(&tid2, NULL, pong, &thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");



	if ((errno = pthread_join(tid1, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");
	if ((errno = pthread_join(tid2, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");



	if ((errno = pthread_mutex_destroy(&thread_params.ptr_mutex)) != 0)
		ERROR(13, "pthread_mutex_destroy() failed");

	if ((errno = pthread_cond_destroy(&thread_params.ptr_cond)) != 0)
		ERROR(15,"pthread_cond_destroy failed!");




	return 0;
}

void *ping(void *arg)
{
	thread_params_t *params = (thread_params_t *) arg;

	for (int i = 0; i < NUM_INTERACTIONS; ++i) {
		if ((errno = pthread_mutex_lock(&params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->next != PING){
			if ((errno = pthread_cond_wait(&params->ptr_cond, &params->ptr_mutex)!=0)){
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("PING\n");
		params->next = PONG;

		if ((errno = pthread_cond_signal(&params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		if ((errno = pthread_mutex_unlock(&params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}

	return NULL;
}
void *pong(void *arg)
{
	thread_params_t *params = (thread_params_t *) arg;

	for (int i = 0; i < NUM_INTERACTIONS; ++i) {

		if ((errno = pthread_mutex_lock(&params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while (params->next != PONG){
			if ((errno = pthread_cond_wait(&params->ptr_cond, &params->ptr_mutex)!=0)){
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("PONG\n");
		params->next = PING;

		if ((errno = pthread_cond_signal(&params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		if ((errno = pthread_mutex_unlock(&params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

	}

	return NULL;
}
