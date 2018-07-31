// BigNum.h ... LARGE positive integer values

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "BigNum.h"

#define TRUE 1
#define FALSE 0

// BigNum *n => n holds type int defining array length
// 								holds type Byte which is an array of bytes


// Initialise a BigNum to N bytes, all zero
void initBigNum(BigNum *n, int Nbytes)
{
  // Init
	n->nbytes = Nbytes;
	n->bytes = malloc(Nbytes);
	
	// All zero
	int i = 0;
	while (i < Nbytes) {
		n->bytes[i] = '0';
		i++;
	}
	assert(n->bytes != NULL);
}

// Add two BigNums and store result in a third BigNum
void addBigNums(BigNum n, BigNum m, BigNum *res)
{
  // TODO
	int i = 0;
	int j = 0;
	int k = 0;
	
	int carry_over = 0;
	
	while (i < n.nbytes || j < m.nbytes) {
		// Adding two numbers together
		int temp_n = n.bytes[i] - 48;
		int temp_m = m.bytes[j] - 48;
		int total = temp_n + temp_m + carry_over;
	
		// Debugging
		// if (temp_n != 0 && temp_m != 0) {
		//   printf ("n.bytes[%d] = %d\n", i, temp_n);
		//   printf ("m.bytes[%d] = %d\n", j, temp_m);
		//   printf ("carry over = %d\n", carry_over);
		//   printf ("total = %d\n", total);
		// }
							
		// If the total of two numbers is greater than 10
		if (total >= 10) {
			res->bytes[k] = total - 10;
			carry_over = 1;
		} else {
			res->bytes[k] = total;
			carry_over = 0;
		}
		
		// Debugging
		// if (temp_n != 0 && temp_m != 0) {
		//   printf ("res->bytes[%d] = %d\n", k, res->bytes[k]);
		//   printf ("---\n");
		// }
		
		i++;
		j++;
		k++;
	}

	// Converting res->bytes back into ASCII
	for (k = 0; k < res->nbytes; k++) {
		res->bytes[k] += 48;
	}
  return;
}

// Set the value of a BigNum from a string of digits
// Returns 1 if it *was* a string of digits, 0 otherwise
int scanBigNum(char *s, BigNum *n)
{
  // TODO
	
	int i = 0;
	int start = 0;
	int end = 0;
	int start_found = FALSE;
	int end_found = TRUE; 
	
	// Scans through s to find start and end positions for number
	while (s[i] != '\0') {
		// printf ("%c\n", s[i]);

		// Start position
		if (start_found == FALSE && s[i] >= '0' && s[i] <= '9') {
			start = i;
			start_found = TRUE;
			end_found = FALSE;
		}
	
		if (end_found == FALSE) {
			// End position if string ends in non-digits
			if (s[i] < '0' || s[i] > '9') {
				end = i - 1;
				end_found = TRUE;
			}
			// End position if string ends in digits	
			if (s[i+1] == '\0') {
				end = i;
				end_found = TRUE;
			}
		}
		i++;
	}

	// If start isnt found, end is not found either -> returning 0
	if (start_found == FALSE) {
		return 0;
	}
	
	int j = 0;
	int k = 0;
	
	// Starting from back of s and mapping starting from n->bytes[0]
	// End is the first non-number char
	for (j = end; j >= start; j--) {
		n->bytes[k] = s[j];	
		k++;
	}
	
  return 1;
}

// Display a BigNum in decimal format
void showBigNum(BigNum n)
{
  // TODO
	// Starts from nbytes - 1 and only prints out after non-zero
	int i;
	int flag = FALSE;
	
	for (i = n.nbytes-1; i >= 0; i--) {
		if (n.bytes[i] > 0 && flag == FALSE) {
			flag = TRUE;
		}

		if (flag == TRUE) {
			printf ("%c", n.bytes[i]);
		}
	}
	
	printf ("\n");
  return;
}
