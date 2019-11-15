#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "args.h"
#include "debug.h"

int bit_n_to_zero(int input, size_t bin_t);

int main (int argc, char *argv[]) {

    if(argc != 3) {
        fprintf(stderr, "Error: expecting two arguments (hex number, bit n)\n");
        exit(1);
    }

    char *hex_string = argv[1];
    char *end_ptr;
    long value = strtol(hex_string, &end_ptr, 16);

    if(*end_ptr != '\0') {
        fprintf(stderr, "Error: invalid hexadecimal string '%s'\n", hex_string);
        exit(2);
    }

    if(value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "Error: value doesn't fit an int: %ld\n", value);
        exit(3);
    }

    int value_int = (int) value;

    size_t bit = atoi(argv[2]);

    int output = bit_n_to_zero(value_int, bit);

    printf("Result is now: 0x%x\n", output);

	return 0;
}

int bit_n_to_zero(int input, size_t bin_t) {
    if(bin_t > 64) {
        return 0;
    }
    return input & ~(1 << bin_t);
}
