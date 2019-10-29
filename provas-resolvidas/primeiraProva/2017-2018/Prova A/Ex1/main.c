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

void trata_sinal(int signal);

int continua = 1;

int main(int argc, char *argv[]){
	/* Disable warnings */
	
	/*
		[PID:7823]: command to execute is ‘date’
		[INFO] kill -s SIGUSR1 7823 (to execute command)
		[INFO] kill -s SIGINT 7823 (to terminate)
		[INFO] SIGUSR1 received. Executing command ‘date’
	*/

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	printf("[PID:%d]: command to execute is ‘%s’\n",getpid(), args.execute_arg);
	printf("[INFO] kill -s SIGUSR1 %d (to execute command)\n",getpid());
	printf("[INFO] kill -s SIGINT %d (to terminate)\n",getpid());

	struct sigaction act;

	act.sa_handler = trata_sinal; 	// Definir a rotina de resposta a sinais 
	sigemptyset(&act.sa_mask);  	// mascara sem sinais -- nao bloqueia os sinais 
	act.sa_flags = 0;           	
	
	// Captura do sinalais
	signal_action(SIGUSR1, &act);
	signal_action(SIGINT, &act);

	while(continua){
		pause();
		if(continua){
			printf("[INFO] SIGUSR1 received. Executing command ‘%s’\n",args.execute_arg);
			int status = system(args.execute_arg);
			if(status != 0) {
				printf("[ERROR] Failed execution: exiting!\n");
				continua = 0;
			}
		}
	}


	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	return 0;
}

void trata_sinal(int signal)
{
	int aux;	
	aux = errno;   // Copia da variável global errno 
	if(signal == SIGINT){
		printf("[INFO] SIGINT received. Exiting!\n");
		continua = 0;
	}	
	
	errno = aux;   // Restaura valor da variável global errno
}