// Copy input to output
// COMP1521 18s1

#include <stdlib.h>
#include <stdio.h>

void copy(FILE *, FILE *);

int main(int argc, char *argv[])
{
    int i;
    FILE *fp;
    
    if (argc == 1) {
        copy(stdin, stdout);
    } else {
        for (i = 1;i < argc;i++) {
            fp = fopen(argv[i], "r");
            if (fp == NULL) {
                printf ("Can't read %s\n", argv[i]);
            } else {
                copy(stdin, stdout);
                fclose(fp);
            }
        }   
    }
    
    return EXIT_SUCCESS;
}

// Copy contents of input to output, char-by-char
// Assumes both files open in appropriate mode

void copy(FILE *input, FILE *output)
{
    char line[BUFSIZ] = {0};

    while(fgets(line, BUFSIZ, input) != NULL) {
        fputs(line, output);
    }
    
}
