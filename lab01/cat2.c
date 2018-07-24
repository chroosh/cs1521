// Copy input to output
// COMP1521 18s1

#include <stdlib.h>
#include <stdio.h>

void copy(FILE *, FILE *);

int main(int argc, char *argv[])
{
	copy(stdin,stdout);
	return EXIT_SUCCESS;
}

// Copy contents of input to output, char-by-char
// Assumes both files open in appropriate mode

// Using getchar() and putchar()

void copy(FILE *input, FILE *output)
{
    int ch = 0;
    while((ch = fgetc(input)) != EOF) {
        fputc(ch, output);
    }
}
