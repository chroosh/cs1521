#include <stdio.h>

int main (void) {
    printf ("hello world\n");

    return 0;
}

int subfunction(void) {
    printf ("test\n");
    
    int i = 0;
    while (i < 10) {
        printf ("%d\n", i);
        i++;
    }

    return 0;
}
