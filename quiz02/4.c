#include <stdio.h>

int main (void) {
	// The value of z is set to the address of x
	int x = 5, *z = &x;

	// This doesnt actually mean *z = &x, rather z = &x.
	// The '*' initialises z as a pointer to &x.
	
	printf ("x = %d\n", x);
	printf ("z = %p\n", z);
	printf ("*z = %d\n", *z);
	printf ("&x = %p\n", &x);
	printf ("---\n"); 
	
	// Increasing *z (= x) by 3.
	// Remember, z points to the address of x,
	// So the value at z is the value at x
	*z = *z + 3;
	
	// The memory address that z points to changes.
	// It is now no longer pointing to &x
	z++;
	// Increase x by 1
	x++;

	printf ("%d\n", x);
}
