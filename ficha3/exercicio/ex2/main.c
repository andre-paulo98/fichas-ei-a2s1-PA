#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#include "args.h"
#include "debug.h"

#define ERROR_PTHREAD_CREATE 1
#define ERROR_PTHREAD_JOIN 2

void *task(void *arg);

typedef struct {
	int n;
}thread_params_t;

int sum = 0;

int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	//(void)argc;
	//(void)argv;
    struct gengetopt_args_info args;
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	//

    int num_threads = args.threads_arg;
    int n = args.num_arg;

    thread_params_t thread_params;
    thread_params.n = n;

    pthread_t tids[num_threads];

    for (int i = 0; i < num_threads; i++){
        if ((errno = pthread_create(&tids[i], NULL, task, &thread_params) != 0))
		    ERROR(ERROR_PTHREAD_CREATE, "Erro no pthread_create()!");
    }


    for (int i = 0; i < num_threads; i++){
        if ((errno = pthread_join(tids[i], NULL)) != 0)
		    ERROR(ERROR_PTHREAD_JOIN, "Erro no pthread_join()!\n");
    }
    
    printf("Sum = %d\n", sum);

	cmdline_parser_free(&args);	
	return 0;
}

void *task(void *arg) {

    thread_params_t *args = (thread_params_t *) arg;

    for (int i = 0; i < args->n; i++){
        int aux = sum;
        aux++;
        sched_yield();
        sum = aux;
        //sum = sum + 1;
    }
    

    return NULL;
}