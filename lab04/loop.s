# COMP1521 18s2 Week 04 Lab
# Looping, no functions (except main)

	.data
eol:
	.asciiz "\n"

### main() function
	.text
	.globl main


# ... setting up global vars in "data" segment

main:
   # set up stack frame
	sw    $fp, -4($sp)       # push $fp onto stack
  la    $fp, -4($sp)       # set up $fp for this function
  sw    $ra, -4($fp)       # save return address
  sw    $s0, -8($fp)       # save $s0 to use as ... int n;
  sw    $s1, -12($fp)      # save $s1 to use as ... int i;
  sw    $s2, -16($fp)      # save $s2 to use as ... int num;
  addi  $sp, $sp, -20      # move $sp to top of stack

# ... scanning in values

  li    $v0, 5
  syscall                  # scanf("%d", into $v0)

# ... TODO: your code for the body of main() goes here ..

	move 	$s0, $v0 				 	 # loads $v0 into $s0
	li    $s1, 0 						 # loads 0 into $s1 (i)
	li 	  $s2, 0 						 # loads 0 into $s2 (num)


# ... where operations happen 
while:
	bge   $s1, $s0, end_while
	addi  $s2, $s2, 5 			 # num+=5
	addi  $s1, $s1, 1 			 # i++
	j while

end_while:

# ... printing result, restoring stack frame
	move  $a0, $s2
  li 	  $v0, 1
	syscall

	la 	  $a0, eol
	li 	  $v0, 4
	syscall

# ... clean stack frame
	lw 		$s2, -16($fp) 		# restore $s2 value
	lw 		$s1, -12($fp) 		# restore $s1 value 
	lw 		$s0, -8($fp) 			# restore $s0 value
	lw 		$ra, -4($fp) 			# restore $ra for return
	la 		$sp, 4($fp) 			# restore $sp (remove stack frame)
	lw 		$fp, ($fp) 				# restore $fp (remove stack frame)

	li $v0, 0
	jr $ra

	
