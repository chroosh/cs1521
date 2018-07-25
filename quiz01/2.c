#include <stdio.h>

int main (void) {
    int i;
    char *str = "Hello";

    for (i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}
