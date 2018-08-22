# COMP1521 18s2 Week 04 Lab
# Compute factorials, recursive function


### Global data

   .data
val: 
	 .word 1
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
   #  set up stack frame
   sw    $fp, -4($sp)       # push $fp onto stack
   la    $fp, -4($sp)       # set up $fp for this function
   sw    $ra, -4($fp)       # save return address
   sw    $s0, -8($fp)       # save $s0 to use as ... int n;
   addi  $sp, $sp, -12      # reset $sp to last pushed item

   #  code for main()
   li    $s0, 0             # n = 0;
   
   la    $a0, msg1
   li    $v0, 4
   syscall                  # printf("n  = ");

   li    $v0, 5
   syscall
   move  $s0, $v0           # scanf("%d", &n);
   
   move  $a0, $s0           
   jal   fac
   nop
   move  $s0, $v0           # tmp = fac(n);
                            # recycle $s0, don't need n now
   la    $a0, msg2
   li    $v0, 4
   syscall                  # printf("n! = ");

   move  $a0, $s0
   li    $v0, 1
   syscall                  # printf("%d\n",tmp);

   la    $a0, eol
   li    $v0, 4
   syscall                  # printf("\n");

   # clean up stack frame
   lw    $s0, -8($fp)       # restore $s0 value
   lw    $ra, -4($fp)       # restore $ra for return
   la    $sp, 4($fp)        # restore $sp (remove stack frame)
   lw    $fp, ($fp)          # restore $fp (remove stack frame)

   li    $v0, 0
   jr    $ra                # return 0

# fac() function

fac:
   # set up stack frame
	 # ... TODO ...
	 sw 	 $fp, -4($sp)	
	 la 	 $fp, -4($sp)
	 sw 	 $ra, -4($fp)
	 sw 	 $s0, -8($fp)
	 addi  $sp, $sp, -12
	
   # code for recursive fac()
	 # ... TODO ...

	 # base case (if n <= 1) 
	 li 	 $v0, 1
	 beq 	 $a0, $v0, end_fac

   # else, store current $a0 into $s0
	 # 	   , subtract 1 from $a0
	 # 		 , call jal fac with new $a0
	 move  $s0, $a0
	 sub 	 $a0, $a0, 1
	 jal fac

	 # multiply return value from nested jal fac with $s0
	 mul   $v0, $s0, $v0

	 # once this finishes, it proceeds to end_fac

   # clean up stack frame
	 # ... TODO ...   
	 end_fac:
	 		lw 		$s0, -8($fp)
			lw 		$ra, -4($fp)
			la 		$sp, 4($fp)
			lw 		$fp, ($fp)

			jr    $ra                # return tmp;
