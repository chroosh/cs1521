# COMP1521 18s2 Week 04 Quiz
# Loops, no functions (except main)

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

	 # q1.s init
	 li 	 $t1, 0
	 li 	 $t2, 1
	 li 	 $t3, 10

loop:
	 # if i ($s1) > n ($s0)  ==> break
	 bgt 	 $t2, $t3, end_loop
	 mul 	 $t1, $t1, $t2
	 addi  $t2, $t2, 1
	 j loop

end_loop:
	 # sw 	 $t1, result 			# value in $t1 is loaded into result

# ... output

   move  $a0, $t1 					# calling $t1 directly
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

