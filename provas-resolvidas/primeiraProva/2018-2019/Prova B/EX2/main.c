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

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "my_functions.h"

#define MAX_WEIGHT 4

typedef struct 
{
	int id;
	int weight;
	int passagens;
	int *peso_ponte;
	int *vehicles_left;

	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
} thread_params_t;

void *vehicles(void *arg);

int main(int argc, char *argv[]){
	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_t tids[args.vehicles_arg];

	int peso_ponte = 0;
	int vehicles_left = args.vehicles_arg;

	thread_params_t thread_params[args.vehicles_arg];

	for(int i = 0; i < args.vehicles_arg; i++){
		thread_params[i].id = i;
		thread_params[i].weight = (rand()%(MAX_WEIGHT-1))+1;
		thread_params[i].passagens = args.passagens_arg;
		thread_params[i].peso_ponte = &peso_ponte;
		thread_params[i].vehicles_left = &vehicles_left;
		thread_params[i].mutex = &mutex;
		thread_params[i].cond = &cond;
		printf("Weight V%d: %d\n",i, thread_params[i].weight);
	}
	printf("-----------------\n");

	mutex_init(&mutex);
	cond_init(&cond);

	for(int i = 0; i < args.vehicles_arg; i++){
		thread_create(&tids[i],vehicles, &thread_params[i]);
	}

	for(int i = 0; i < args.vehicles_arg; i++){
		thread_join(tids[i]);
	}

	mutex_destroy(&mutex);
	cond_destroy(&cond);


	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	return 0;
}

void *vehicles(void *arg) 
{
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
	
	while(params->passagens > 0){

		mutex_lock(params->mutex);
		while(*params->peso_ponte > (MAX_WEIGHT-params->weight)){
			cond_wait(params->cond, params->mutex);
		}
		(*params->peso_ponte) += params->weight;
		printf("V%d entered the bridge (total weight: %d)\n",params->id,*params->peso_ponte);
		mutex_unlock(params->mutex);
		
		sleep(3);

		mutex_lock(params->mutex);
		params->passagens--;
		(*params->peso_ponte) -= params->weight;
		printf("V%d left the bridge (total weight: %d)\n",params->id,*params->peso_ponte);
		cond_signal(params->cond);
		mutex_unlock(params->mutex);
	}
	
	
	return NULL;
}

