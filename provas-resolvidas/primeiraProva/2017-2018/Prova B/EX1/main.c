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

int main(int argc, char *argv[]){

	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	printf("Selected signal: %d\nFirst: %d\nLast: %d\n", args.sinal_arg, args.first_arg, args.last_arg);

	int counter = 0;
	for(int i = args.first_arg; i < args.last_arg; i++){
		int index = args.last_arg - args.first_arg + 1;
		if(i == getpid()){
			printf("#%03d: Refusing to send signal '%d' to self\n",index, args.sinal_arg);
		}else if(i == getppid()){
			printf("#%03d: Refusing to send signal '%d' to own session shell\n",index, args.sinal_arg);
		}else{
			printf("#%03d: Sending signal '%d' to PID %d: ",index,args.sinal_arg,i);
			if(kill(i,args.sinal_arg) == 0){
				printf("sucess\n");
				counter++;
			}else{
				printf("erro\n");
			}
		}
	}

	printf("[INFO]: %d signal(s) sent\n",counter);



	// gengetopt: libertar recurso (assim que possível)
	cmdline_parser_free(&args);

	return 0;
}
