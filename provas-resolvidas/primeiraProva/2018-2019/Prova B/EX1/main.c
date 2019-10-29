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
#include <ctype.h>

#include "debug.h"
#include "memory.h"
#include "args.h"
#include "my_functions.h"

#define NUM_PROCESSOS 26

int main(int argc, char *argv[]){
	
	struct gengetopt_args_info args;

	// gengetopt parser: deve ser a primeira linha de código no main
	if (cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");


	FILE *fp = c_fopen(args.file_arg, "r");

	printf("File: %s\n",args.file_arg);

	for(int i = 0; i < NUM_PROCESSOS; i++){
		int pid = fork();
		
		c_fseek(fp,0,SEEK_SET);
		
		if (pid == 0) {			// Processo filho 
			char letter_upper = 65+i;
			char letter_lower = tolower(letter_upper);
			int count = 0;
			char buffer;
			while((buffer = fgetc(fp)) != EOF){
				if(buffer == letter_upper || buffer == letter_lower){
					count++;
				}
			}
			
			printf("Process [%c]: found %d '%c' or '%c' occurrences!\n", letter_upper, count, letter_lower,letter_upper);
			
			exit(0);			// Termina processo filho (para este não criar novos processos)
		} else if (pid > 0) {	// Processo pai
			// usar preferencialmente a zona a seguir ao for
		} else					// < 0 - erro
			ERROR(2, "Erro na execucao do fork()");
	}

	for(int i = 0; i < NUM_PROCESSOS; i++){
		wait(NULL);
	}
	
	fclose(fp);

	cmdline_parser_free(&args);
	
	return 0;
}

