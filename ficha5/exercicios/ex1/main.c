#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include "args.h"
#include "debug.h"

int binToDecimal(const char *bin_s_ptr);

int main (int argc, char *argv[]) {

    struct gengetopt_args_info args;
	if(cmdline_parser(argc, argv, &args))
		ERROR(1, "Erro: execução de cmdline_parser\n");

	int decimal = binToDecimal(args.binary_arg);
    if(decimal != -1) {
        printf("O resultado é: %d\n", decimal);
    } else {
        printf("Ocorreu um erro a calcular o valor decimal de %s\n", args.binary_arg);
    }

	cmdline_parser_free(&args);	
	return 0;
}


int binToDecimal(const char *bin_s_ptr) {

    int decimal = 0;
    int multiplyFactor = 1;

    size_t size = strlen(bin_s_ptr);

    if(size > sizeof(int) * 8 -1) {
        return -1;
    }

    for(size_t i = 0; i < size; i++) {
        if(bin_s_ptr[i] == '1') {
            decimal += pow(2, size - i - 1);
        } else if(bin_s_ptr[i] != '0'){
            return -1;
        }

        multiplyFactor *= 2;
        // multiplyFactor << 1;

    }

    /*for(char const *aux_ptr = bin_s_ptr + size - 1; aux_ptr + 1 != bin_s_ptr; --aux_ptr){
        if(*aux_ptr == '1') {
            //
        }
    }*/


    return decimal;

} 