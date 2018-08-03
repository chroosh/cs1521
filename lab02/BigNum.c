// BigNum.h ... LARGE positive integer values
// Christopher Shi <z5165244>

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
	n->bytes = calloc(Nbytes, sizeof(Byte));
	
	// All zer0
	int i;
	for (i = 0; i < Nbytes; i++) {
		n->bytes[i] = '0';
	}
	assert(n->bytes != NULL);
}

// Add two BigNums and store result in a third BigNum
void addBigNums(BigNum n, BigNum m, BigNum *res)
{
	// Identifies the longest array
	int max = n.nbytes;
	if (m.nbytes > n.nbytes)
		max = m.nbytes;
	
	max++;

	// Reallocates res to max + 1
	if (max > res->nbytes) {
		res->bytes = realloc(res->bytes, max);
		res->nbytes = max;
	}
	
	// Resets all non digit values in n and m to 0
	int z;
	for (z = 0; z < max; z++) {
		if (n.bytes[z] < '0' || n.bytes[z] > '9')
			n.bytes[z] = '0';
		if (m.bytes[z] < '0' || m.bytes[z] > '9')
			m.bytes[z] = '0';
	}
	
	int i;
	int carry_over = 0;
	for (i = 0; i < max; i++) {
		// Adding two numbers together
		
		int total = (n.bytes[i] - 48) + (m.bytes[i] - 48) + carry_over;
	
		// Debugging
		// int temp_n = n.bytes[i] - '0';
		// int temp_m = m.bytes[i] - '0';
		// if (temp_n != 0 && temp_m != 0) {
		//   printf ("n.bytes[%d] = %d\n", i, temp_n);
		//   printf ("m.bytes[%d] = %d\n", i, temp_m);
		//   printf ("carry over = %d\n", carry_over);
		//   printf ("total = %d\n", total);
		// }
							
		// If the total of two numbers is greater than 10
		if (total >= 10) {
			res->bytes[i] = total - 10;
			carry_over = 1;
		} else {
			res->bytes[i] = total;
			carry_over = 0;
		}
		
		// Converting back to ASCII
		res->bytes[i] += '0';
		
		// Debugging
		// if (temp_n != 0 && temp_m != 0) {
		//   printf ("res->bytes[%d] = %d\n", i, res->bytes[i]);
		//   printf ("---\n");
		// }
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
		// Start position
		if (start_found == FALSE && s[i] >= '0' && s[i] <= '9') {
			start = i;
			start_found = TRUE;
			end_found = FALSE;
		}
		
		// End position if string ends in non-digits
		if (end_found == FALSE && (s[i] < '0' || s[i] > '9')) {
			end = i - 1;
			end_found = TRUE;
		}

		// End position if string ends in digits
		if (end_found == FALSE && s[i+1] == '\0') {
			end = i;
			end_found = TRUE;
		}
		i++;
	}
	
	// If start isnt found, end is not found either -> returning 0
	if (start_found == FALSE)
		return 0;

	// Reallocating memory to the size of the string
	int string_len = end - start + 1;
	if (string_len > n->nbytes) {
		n->bytes = realloc(n->bytes, string_len);
		n->nbytes = string_len;
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
	// Starts from nbytes - 1 and only prints out after non-zero
	int i;
	int flag = FALSE;
	for (i = n.nbytes-1; i >= 0; i--) {
		if (n.bytes[i] > '0' && flag == FALSE) {
			flag = TRUE;
		}

		if (flag == TRUE) {
			printf ("%c", n.bytes[i]);
		}
	}

  return;
}
