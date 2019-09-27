#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "args.h"
#include "debug.h"

struct gengetopt_args_info args;


int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	(void)argc;
	(void)argv;

	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");


	for (int i = 0; i < args.num_procs_arg; ++i) {
		int r = fork();
		if(r == 0) {
			printf("Processo #%d (PID = %d)\n", i, getpid());
			exit(0);
		}

	}

	cmdline_parser_free(&args);

	return 0;
}
