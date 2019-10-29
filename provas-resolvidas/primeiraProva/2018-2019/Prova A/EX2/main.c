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
#include <signal.h>

#include "debug.h"
#include "memory.h"
#include "args.h"

struct gengetopt_args_info args;

void trata_sinal_info(int signal, siginfo_t *siginfo, void *context);

int continua = 1;


int main(int argc, char *argv[]){

	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	struct sigaction act_info;

	act_info.sa_sigaction = trata_sinal_info;
	sigemptyset(&act_info.sa_mask);
	act_info.sa_flags = 0;           	//fidedigno
	act_info.sa_flags |= SA_SIGINFO; 	//info adicional sobre o sinal
	//act_info.sa_flags |= SA_RESTART; 	//recupera chamadas bloqueantes
	
	printf("[INFO] Processing signal from [MIN:%d, MAX:%d]\n", args.min_arg, args.max_arg);

	for(int i = args.min_arg; i<= args.max_arg; i++){
		if(sigaction(i, &act_info, NULL) < 0)
			printf("[WARNING] Can't install handler for signal '' (%d)\n",i);
	}
	printf("[INFO] Terminate: kill -s SIGKILL %d | kill -s SIGINT %d\n",getpid(),getpid());

	while(continua){
		pause();
	}

	cmdline_parser_free(&args);
	return 0;
}

void trata_sinal_info(int signal, siginfo_t *siginfo, void *context) 
{
	(void)context;
	/* Cópia da variável global errno */
	int aux = errno;

	printf("[PID:%d] Got signal '%s' (%d) from process '%ld'\n",getpid(),strsignal(signal),signal,(long)siginfo->si_pid);

	if(signal == SIGINT){
		printf("Got SIGINT -- terminating\n");
		continua = 0;
	}else{
		
		printf("Waiting for a signal\n");
	}


	/* Restaura valor da variável global errno */
	errno = aux;
}

