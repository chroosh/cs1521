// COMP1521 18s1 Week 02 Lab (warm-up)

#include <stdio.h>

int main()
{
	// Code to generate and display the largest "int" value

	int x = 2147483647;
	printf("int %x, %d\n", x, x);

	// Code to generate and display the largest "unsigned int" value

	unsigned int y = 4294967295;
	printf("unsigned int %x, %u\n", y, y);

	// Code to generate and display the largest "long int" value

	long int xx = 2147483647;;
	printf("long int %lx, %ld\n", xx, xx);

	// Code to generate and display the largest "unsigned long int" value

	unsigned long int xy = 4294967295;;
	printf("unsigned long int %lx, %lu\n", xy, xy);

	// Code to generate and display the largest "long long int" value

	long long int xxx = 9223372036854775807;
	printf("long long int %llx, %lld\n", xxx, xxx);

	// Code to generate and display the largest "unsigned long long int" value

	unsigned long long int xxy = 1844674407370955161;
	printf("unsigned long long int %llx, %llu\n", xxy, xxy);

	return 0;
}

