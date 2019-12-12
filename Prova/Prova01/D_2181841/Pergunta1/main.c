#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "args.h"
#include "debug.h"

#define MIN_PROCS 4
#define MAX_PROCS 16

#define ERROR_GENGETOPT_PARSER 1
#define ERROR_CREATE_FORK 2

int main (int argc, char *argv[]) {

    struct gengetopt_args_info args;
	if(cmdline_parser(argc, argv, &args))
		ERROR(ERROR_GENGETOPT_PARSER, "Erro: execução de cmdline_parser\n");


	if(args.procs_arg < MIN_PROCS || args.procs_arg > MAX_PROCS) {
		printf("ERR: num_procs must be within [%d,%d]:%d given\n", MIN_PROCS, MAX_PROCS, args.procs_arg);
		exit(0);
	}


	pid_t pid;

	for(int i = 0; i < args.procs_arg; i++){
		pid = fork();
		if (pid == 0) {
			printf("[PAI:%d] Filho %d tem PID = %d\n", getppid(), i, getpid());
			exit(0);
		} else if (pid < 0)	{
			ERROR(ERROR_CREATE_FORK, "Erro: execução do fork()");
		}
	}

	for(int i = 0; i < args.procs_arg; i++){
		wait(NULL);
	}
	


	cmdline_parser_free(&args);	
	return 0;
}
