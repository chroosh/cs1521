# COMP1521 17s2 Final Exam
# void colSum(m, N, a)

   .text
   .globl colSum

# params: m=$a0, N=$a1, a=$a2
colSum:
# prologue
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   addi $sp, $sp, -4
   sw   $ra, ($sp)
   addi $sp, $sp, -4
   sw   $s0, ($sp)
   addi $sp, $sp, -4
   sw   $s1, ($sp)
   addi $sp, $sp, -4
   sw   $s2, ($sp)
   addi $sp, $sp, -4
   sw   $s3, ($sp)
   addi $sp, $sp, -4
   sw   $s4, ($sp)
   addi $sp, $sp, -4
   sw   $s5, ($sp)
   # if you need to save more than six $s? registers
   # add extra code here to save them on the stack

# suggestion for local variables (based on C code):
# m=#s0, N=$s1, a=$s2, row=$s3, col=$s4, sum=$s5

	# moving params into registers
	move $s0, $a0
	move $s1, $a1
	move $s2, $a2

   # add code for your colSum function here
	li   $s4, 0
	outer_for:
		bge  $s4, $s1, end_outer_for
		li   $s5, 0
		li   $s3, 0
		inner_for:
			bge   $s3, $s1, end_inner_for

			# $t0 = m[row][col]
			mul   $t0, $s3, $s1
			add 	$t0, $t0, $s4
			li    $t1, 4 							# ((row * N) + col) * 4 - because int types are 4
			mul 	$t0, $t0, $t1
			add 	$t0, $t0, $s0

			lb 	$t0, ($t0) 						# load byte at address into register

			# sum += m[row][col]
			add 	$s5, $s5, $t0


			#printing m[row][col]
			#move 	$a0, $s5
			#li 	$v0, 1
			#syscall 
				
			#la 	$a0, eol 
		 	#li 	$v0, 4
			#syscall

			# row++;
			addi 	$s3, $s3, 1
			j inner_for

		end_inner_for:

		li 	$t1, 4 								# same as above
		mul 	$t0, $s4, $t1
		add 	$t0, $t0, $s2

		sb    $s5, ($t0) 							# storing byte into address 
	
		# col++
		addi 	$s4, $s4, 1
		j outer_for

	end_outer_for:

# epilogue
   # if you saved more than six $s? registers
   # add extra code here to restore them
   lw   $s5, ($sp)
   addi $sp, $sp, 4
   lw   $s4, ($sp)
   addi $sp, $sp, 4
   lw   $s3, ($sp)
   addi $sp, $sp, 4
   lw   $s2, ($sp)
   addi $sp, $sp, 4
   lw   $s1, ($sp)
   addi $sp, $sp, 4
   lw   $s0, ($sp)
   addi $sp, $sp, 4
   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   j    $ra

