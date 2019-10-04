#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "debug.h"

void trata_sinal(int signal);

int continua = 1;

int main(int argc, char* argv[]) {
    /* Silencia os warnings */
    //(void)argc;
    //(void)argv;

    int res = fork();

    if (res == 0) {  // é o filho

        while (1) {
            sleep(5);
            kill(getppid(), SIGUSR1);
        }

    } else if (res > 0) {  // é o pai

	    printf("PID do processo PAI: %d", getpid());

        struct sigaction act;
        act.sa_handler = trata_sinal; 	// Definir a rotina de resposta a sinais 
        sigemptyset(&act.sa_mask);  	// mascara sem sinais -- nao bloqueia os sinais 
        act.sa_flags = 0;           	
        //act.sa_flags |= SA_RESTART; 	// recupera chamadas bloqueantes

        // Captura do sinal ??? 
        if(sigaction(SIGUSR1, &act, NULL) < 0)
            ERROR(3, "sigaction (sa_handler) - SIGUSR1");

        
        if(sigaction(SIGINT, &act, NULL) < 0)
            ERROR(3, "sigaction (sa_handler) - SIGINT");


		struct gengetopt_args_info args;
        if (cmdline_parser(argc, argv, &args))
            ERROR(1, "Erro: execução de cmdline_parser\n");


		while (continua) {
            pause();
            if(continua) {
				FILE *fileptr = fopen(args.file_arg);
				if(fileptr == NULL) {
					ERROR(2, "ERROR on fopen()");
				}

				char *lineptr = NULL;
				size_t n;

				if(getline(&lineptr, &n, fileptr) != -1) {
					printf("%s", lineptr);
				} else {
					printf("Erro ao ler o ficheiro. Poderá estar vazio.\n");
				}


				fclose(fileptr);
            }
        }



		cmdline_parser_free(&args);
    } else {
        ERROR(1, "ERROR on fork()");
        printf("");
    }

    //

    
    return 0;
}

void trata_sinal(int signal) {
    int aux;
    aux = errno;  // Copia da variável global errno

    if(signal == SIGINT) {
        continua = 0;
    } 

    // código
    printf("Recebi o sinal (%d)\n", signal);

    errno = aux;  // Restaura valor da variável global errno
}