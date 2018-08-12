// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by ...

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
   unsigned int a : 4,
                b : 8,
                c : 20;
};

int main(void)
{
  struct _bit_fields x = {10, 50, 100};
	unsigned int *ptrX = (unsigned int *) &x;

  // printf("%lu\n",sizeof(x));
	
	// unsigned int val = 10;
	// unsigned int *valPtr = &val;
	unsigned int mask = 1;
	unsigned int countA = 0, countB = 0, countC = 0;
	

	int i = 0;
  while (i < 32) {
		printf ("%d: ", i);
		if ((*ptrX & mask) > 0) {
			// bit at i is equal to 1;
			printf ("1\n");

			if (i < 4) {
				countA += (*ptrX & mask);
			} else if (i < 12) {
				countB += ((*ptrX & mask) >> 4);
			} else {
				countC += ((*ptrX & mask) >> 12);
			}
		} else {
			// bit at i is equal to 0;
			printf ("0\n");
		}
        
   	i++;
  	mask = mask << 1;
  }

	printf ("count a = %d\n", countA);
	printf ("count b = %d\n", countB);
	printf ("count c = %d\n", countC);


	return 0;
}
