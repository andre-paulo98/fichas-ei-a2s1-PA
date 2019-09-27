#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "args.h"
#include "debug.h"

int main (int argc, char *argv[]) {
	/* Silencia os warnings */
	//(void)argc;
	//(void)argv;

	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	//

	cmdline_parser_free(&args);	
	return 0;
}
