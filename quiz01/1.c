#include <stdio.h>

int main (void) {
    int a[10];

    printf ("%zu\n", sizeof(int));
    printf ("%p\n", &a[0]);
    printf ("%p\n", &a[2]);

    // sizeof(int) is 4 -> each int takes up 4 bytes of memory
    // so 2 ints take up 8 bytes
    //
    // If &a[0] is 0x10000
    //    &a[2] is 0x10008
    //
    // I'm assuming that the value returned is the first byte
    // of memory allocated to the int
    //
    // So, &[0] is allocated 0x10000, 0x10001, 0x10002, 0x10003
    // and &[1] is allocated 0x10004, 0x10005, 0x10006, 0x10007
    //
    // -> Therefore &[2] starts on 0x10008.
}
