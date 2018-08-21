# COMP1521 18s2 Week 04 Lab
# Compute factorials, no functions (except main)

### Global data

   .data
msg1:
   .asciiz "n  = "
msg2:
   .asciiz "n! = "
eol:
   .asciiz "\n"

### main() function
   .text
   .globl main

main:
   # set up stack frame
   sw    $fp, -4($sp)       # push $fp onto stack
   la    $fp, -4($sp)       # set up $fp for this function
   sw    $ra, -4($fp)       # save return address
   addi  $sp, $sp, -20      # move $sp to top of stack

	 # q2 given
	 # add   $t1, $0, $0

	 # 0x4321 shifted 16 bits and stored, lower 16 bits are 0
	 # lui 	 $t1, 0x4321

	 # performs bitwise or on $t1 which has lower 16 bits 0
	 # ori 	 $t1, $t1, 0x8765 

	 # therefore, the final value LOADED INTO $t1 is equal to
	 # 0x43218765

	 # q2 answer
	 li 	 $t1, 0x43218765
	
# ... output

   move  $a0, $t1 					# $t1
   li    $v0, 1
   syscall                  # printf("%d",fac);

   la    $a0, eol
   li    $v0, 4
   syscall                  # printf("\n");

# clean up stack frame
   lw    $ra, -4($fp)       # restore $ra for return
   la    $sp, 4($fp)        # restore $sp (remove stack frame)
   lw    $fp, ($fp)         # restore $fp (remove stack frame)

   li    $v0, 0
   jr    $ra                # return 0

