#include <stdio.h>

int f(int n) {
    if (n == 0) {
        return 0;
    } else {
        return n + f(n-1);;
    }
}

int main (void) {
   printf ("%d\n", f(4));
   
   return 0;
}
