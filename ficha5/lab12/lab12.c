#include <stdio.h>

void printBinary(int a);

int main(int argc, char *argv[])
{
	 (void)argc; (void)argv;


	// a) 0...01
    unsigned int A = 1;
    printf("Mask a): %u\n", A);
	
	
	// b) 0...10
    unsigned int B = 1 << 1;
    printf("Mask b): %u\n", B);
	
	
	// c) 0...10101
    unsigned int C = 0;
    for (size_t i = 0; i < 7; i++){
        C = C << 2;
        C = C | 1;
        //printBinary(C);
        //printf("%u\n", C);
    }
    
    printf("Mask c): %u\n", C);
    printBinary(C);


	return 0;
}


void printBinary(int a) {
    int k, c;
    for (c = 31; c >= 0; c--) {
        k = a >> c;

        if (k & 1)
            printf("1");
        else
            printf("0");
    }

    printf("\n");
}