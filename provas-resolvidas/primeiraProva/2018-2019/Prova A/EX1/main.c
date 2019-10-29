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
#include <pthread.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "my_functions.h"

void *truck(void *arg);

typedef struct
{
	int id;
	int *ptr_container;
	int *ptr_index_container;
	int contaier_number;
	int cont_counter;
	int dist_counter;
	pthread_mutex_t *ptr_mutex;
} thread_params_t;


int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if (cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	if (args.truck_arg <= 0)
	{
		ERROR(1, "Erro: Invalid number of trucks\n");
	}

	if (args.containers_arg <= 0)
	{
		ERROR(1, "Erro: Invalid number of containers\n");
	}

	int container[args.containers_arg];
	srand(time(NULL));
	for (int i = 0; i < args.containers_arg; i++){
		container[i] = (rand() % (9)+1)*100;
	}
	
	pthread_t tids[args.truck_arg];
	thread_params_t thread_params[args.truck_arg];
	pthread_mutex_t mutex;

	int index_container = 0;

	// Inicialização das estruturas - para cada thread
	for (int i = 0; i < args.truck_arg; i++)
	{
		thread_params[i].id = i + 1;
		thread_params[i].ptr_container = container;
		thread_params[i].ptr_index_container = &index_container;
		thread_params[i].contaier_number = args.containers_arg;
		thread_params[i].ptr_mutex = &mutex;
		thread_params[i].cont_counter = 0;
		thread_params[i].dist_counter = 0;
	}

	// Mutex: inicializa o mutex antes de criar a(s) thread(s)
	mutex_init(&mutex);


	// Criação das threads + passagem de parâmetro
	for (int i = 0; i < args.truck_arg; i++)
	{
		thread_create(&tids[i], truck, &thread_params[i]);
		
	}

	// Espera que todas as threads terminem
	for (int i = 0; i < args.truck_arg; i++)
	{
		thread_join(tids[i]);
	}

	// Mutex: liberta recurso
	mutex_destroy(&mutex);


	int total_dist = 0;

	printf("\n\n-------------------------------------------\n");
	for(int i = 0; i<args.truck_arg; i++){
		total_dist += thread_params[i].dist_counter;
		printf("Truck %d: %d container(s); %d kms\n", thread_params[i].id, thread_params[i].cont_counter, thread_params[i].dist_counter);
		printf("-------------------------------------------\n");
	}

	float avg = total_dist / args.truck_arg;
	printf("\nTotal kms: %d\n", total_dist);
	printf("Avg kms per truck: %.1f\n",avg);

	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	return 0;
}

void *truck(void *arg){
	
	thread_params_t *params = (thread_params_t *)arg;
	printf("[INFO] TruckID '%d' starting service\n", params->id);

	// Mutex: entra na secção crítica
	int continua = 1;
	while (continua)
	{
		int distance = 0;

		if (!mutex_lock(params->ptr_mutex))return NULL;
		
		if (*params->ptr_index_container < params->contaier_number){
			distance = params->ptr_container[*params->ptr_index_container];
			int left = params->contaier_number - *params->ptr_index_container-1;
			printf("[ID:%d] Got container %03d ( %d left): %dkms\n", params->id, *params->ptr_index_container + 1, left, distance);

			(*params->ptr_index_container)++;
			params->cont_counter++;
			params->dist_counter += distance;

			// Mutex: sai da secção crítica
		}else{
			continua = 0;
		}
		if(!mutex_unlock(params->ptr_mutex)) return NULL;

		sleep(distance/100);
	}

	return NULL;
}