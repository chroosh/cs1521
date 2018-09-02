############################################################ -*- asm -*-
# COMP1521 18s2 -- Assignment 1 -- Scrolling Text!
# Scroll letters from a message in argv[1]
#
# Base code by Jashank Jeremy
# Tweaked by John Shepherd
# $Revision: 1.5 $
#
# Edit me with 8-column tabs!

# Requires:
#  - `all_chars', defined in chars.s

# Provides:
	.globl	main # :: int, [char *], [char *] -> int
	.globl	setUpDisplay # :: int, int -> void
	.globl	showDisplay # :: void -> void
	.globl	delay # :: int -> vovid
	.globl	isUpper # :: char -> int
	.globl	isLower # :: char -> int

	.globl	CHRSIZE
	.globl	NROWS
	.globl	NDCOLS
	.globl	MAXCHARS
	.globl	NSCOLS
	.globl	CLEAR


########################################################################
	.data

	# /!\ NOTE /!\
	# In C, the values of the symbols `CHRSIZE', `NROWS', `NDCOLS',
	# `NSCOLS', `MAXCHARS', and `CLEAR' would be substituted during
	# preprocessing.  SPIM does not have preprocessing facilities,
	# so instead we provide these values in the `.data' segment.

	# # of rows and columns in each big char
CHRSIZE:	.word	9
	# number of rows in all matrices
NROWS:		.word	9
	# number of columns in display matrix
NDCOLS:		.word	80
	# max length of input string
MAXCHARS:	.word	100
	# number of columns in bigString matrix
	# max length of buffer to hold big version
	# the +1 allows for one blank column between letters
NSCOLS:		.word	9000	# (NROWS * MAXCHARS * (CHRSIZE + 1))
        # ANSI escape sequence for 'clear-screen'
CLEAR:	.asciiz "\033[H\033[2J"
# CLEAR:	.asciiz "__showpage__\n" # for debugging

main__0:	.asciiz	"Usage: ./scroll String\n"
main__1:	.asciiz	"Only letters and spaces are allowed in the string!\n"
main__2:	.asciiz "String mush be < "
main__3:	.asciiz " chars\n"
main__4:	.asciiz "Please enter a string with at least one character!\n"
eol: 			.asciiz "\n"
space: 		.asciiz " "

	.align	4
theString:	.space	101	# MAXCHARS + 1
	.align	4
display:	.space	720	# NROWS * NDCOLS
	.align	4
bigString:	.space	81000	# NROWS * NSCOLS


########################################################################
# .TEXT <main>
	.text
main:

# Frame:	$fp, $ra, ...
# Uses:		$a0, $a1, $t0, $t1, $t2, $s0, $s1
# Clobbers:	...

# Locals:
#	- `theLength' in $s0
#	- `bigLength' in $s1
#	- `ch' in $s2
#	- `str' in $t2
#	- `i' in $s3
#	- `j' in $s4
#	- `row' in $s5
#	- `col' in $s6
#	- `iterations' in $s7
#	- `startingCol' in $s4

# Structure:
#	main
#	-> [prologue]
#	-> main_argc_gt_two
#	-> main_PTRs_init
#	  -> main_PTRs_cond
#	    -> main_ch_notspace
#	    -> main_ch_isLower
#	    -> main_ch_isSpace
#	  -> main_PTRs_step
#	-> main_PTRs_f
#	[theLength cond]
#	  | main_theLength_ge_MAXCHARS
#	  | main_theLength_lt_MAXCHARS
#	  | main_theLength_lt_1
#	  | main_theLength_ge_1
# 
#	-> initialise display to all spaces
# -> create bigchars
# -> final (function calls)
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)  						# note: switch to $fp-relative
	sw	$s0, -8($fp) 							# theLength
	sw	$s1, -12($fp) 						# bigLength
	sw	$s2, -16($fp) 						# ch
	sw  $s3, -20($fp) 						# i
	sw 	$s4, -24($fp) 						# j/starting_col
	sw 	$s5, -28($fp) 						# row
	sw 	$s6, -32($fp) 						# col
	sw 	$s7, -36($fp) 						# iterations
	addi	$sp, $sp, -40

	# if (argc < 2)
	li	$t0, 2
	bge	$a0, $t0, main_argc_gt_two
	nop	# in delay slot
	# printf(...)
	la	$a0, main__0
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_argc_gt_two:

	move	$s0, $zero
main_PTRs_init:
	# s = argv[1]
	lw	$t2, 4($a1)
main_PTRs_cond:
	# optimisation: `ch = *s' now
	# (ch = )*s
	lb	$s2, ($t2)
	# *s != '\0'  =>  ch != 0
	beqz	$s2, main_PTRs_f
	nop	# in delay slot

	# if (!isUpper(ch))
main_ch_upper:
	move	$a0, $s2
	jal	isUpper
	nop	# in delay slot
	beqz	$v0, main_ch_lower
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (!isLower(ch))
main_ch_lower:
	move	$a0, $s2
	jal	isLower
	nop	# in delay slot
	beqz	$v0, main_ch_space
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (ch != ' ')
main_ch_space:
	li	$t0, ' '
	bne	$s2, $t0, main_ch_fail
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot

main_ch_fail:
	# printf(...)
	la	$a0, main__1
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot

main_ch_ok:
	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	# break  =>  jump out of for(*s...)
	bge	$s0, $t0, main_PTRs_f

	# theString[theLength]
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = ch
	sb	$s2, ($t0)

	# theLength++
	addi	$s0, $s0, 1

main_PTRs_step:
	# s++  =>  s = s + 1
	addiu	$t2, $t2, 1	# ADDIU because address
	j	main_PTRs_cond
	nop
main_PTRs_f:

	# theString[theLength] = ...
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = '\0'
	sb	$zero, ($t0)

	# CHRSIZE + 1
	la	$t0, CHRSIZE
	lw	$t0, ($t0)
	addi	$t0, $t0, 1
	# bigLength = theLength * (CHRSIZE + 1)
	mul	$s1, $t0, $s0

	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	blt	$s0, $t0, main_theLength_lt_MAXCHARS
	nop	# in delay slot
main_theLength_ge_MAXCHARS:
	# printf(..., ..., ...)
	la	$a0, main__2
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	move	$a0, $t0
	li	$v0, 1 # PRINT_INT_SYSCALL
	syscall
	la	$a0, main__3
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_lt_MAXCHARS:

	# if (theLength < 1)
	li	$t0, 1
	bge	$s0, $t0, main_theLength_ge_1
	nop	# in delay slot
main_theLength_lt_1:
	# printf(...)
	la	$a0, main__4
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_ge_1:

	# ... TODO ... starting from line 73 in scroll.c
	# items marked with a [x] indicate that they have been tested using syscalls

	# $t0 = NROWS
	# $t1 = NDCOLS
	lw 	 $t0, NROWS
	lw 	 $t1, NDCOLS

	# [x] initialise display to all spaces
	li   $s3, 0 														 	# i = $s3
	init_spaces_row:
		bge   $s3, $t0, end_init_spaces_row 		# if (i >= NROWS)

	li 	 $s4, 0 														 	# j = $s4
	init_spaces_col:
		bge  	$s4, $t1, end_init_spaces_col 		# if (j >= NDCOLS)

		# display[i][j] = 0;
		move 	$t3, $s3 													# move row into $t3
		mul 	$t3, $t3, $t1 										# $t3 = i * NDCOLS
		add 	$t3, $t3, $s4 										# $t3 = (i * NDCOLS) + j

		li 		$t2, ' ' 													# $t2 holding ' '
		sb 		$t2, display($t3) 								# storing ' ' into display($t3)

	spaces_increment:
		addi 	$s4, $s4, 1
		j 		init_spaces_col

	end_init_spaces_col:
		addi 	$s3, $s3, 1
		j 		init_spaces_row

	end_init_spaces_row:


	# [x] create bigchars array
	lw 	 $t0, CHRSIZE 												# $t0 = CHRSIZE
	lw 	 $t3, NSCOLS 													# $t0 = NSCOLS
	li 	 $s3, 0																# $s3 = i

	bigchars_i_loop:
		bge   $s3, $s0, end_bigchars_i_loop 		# if (i >= theLength)

		lb 		$s2, theString($s3) 							# ch = theString[i]
		li 		$t1, ' '
		bne 	$s2, $t1, ch_ne_space 						# if (ch == ' ')
		
		# double for loop 
		li 		$s5, 0 														# $s5 = row
		bigchars_row_loop:
			bge  	$s5, $t0, end_bigchars_row_loop 

		li 		$s6, 0 														# $s6 = col
		bigchars_col_loop:
			bge 	$s6, $t0, end_bigchars_col_loop

			# prologue
			addi 	$t4, $t0, 1  										# (CHRSIZE + 1)
			mul 	$t4, $t4, $s3 									# (CHRSIZE + 1) * i
			add 	$t4, $t4, $s6 									# (CHRSIZE + 1) * i + col

			# body	
			move 	$t2, $s5 												# move row into $t2
			mul 	$t2, $t2, $t3 									# $t2 = row * NSCOLS
			add 	$t2, $t2, $t4 									# $t2 = (row * NSCOLS) + $t4

			li 		$t4, ' ' 												# $t4 holding ' '
			sb 		$t4, bigString($t2) 						# storing ' ' into bigString($t2)

		bigchars_inner_increment:
			addi  $s6, $s6, 1
			j 		bigchars_col_loop

		end_bigchars_col_loop:
			addi 	$s5, $s5, 1
			j 		bigchars_row_loop
		
		end_bigchars_row_loop:
			j 		bigchars_part2

		# else
		ch_ne_space: 														# if (ch != ' ')
			
			li 		$t5, 0 													# $t5 = which
			li 		$t6, 1 													# $t6 = 1
			
			# if (isUpper(ch))
			bigchars_isUpper:
				move 	$a0, $s2
				jal 	isUpper
				bne 	$v0, $t6, bigchars_isLower
			
				move  $t5, $s2 											# move ch into which
				sub 	$t5, $t5, 65 									# $t5 = ch - 65
				j 		bigchars_else_loop

			# if (isLower(ch))
			bigchars_isLower:
				move 	$a0, $s2
				jal 	isLower
				bne 	$v0, $t6, bigchars_else_loop
			
				move 	$t5, $s2 											# move ch into which
				sub 	$t5, $t5, 97 									# $t5 = ch - 97
				addi 	$t5, $t5, 26 									# $t5
				j 		bigchars_else_loop

			bigchars_else_loop:	
				li 		$s5, 0 												# $s5 = row
				bigchars_row_loop2: 								# if (row > CHRSIZE)
					bge 	$s5, $t0, end_bigchars_row_loop2 	

				li 		$s6, 0 												# s6 = col
				bigchars_col_loop2: 								# if (col > CHRSIZE)
					bge  	$s6, $t0, end_bigchars_col_loop2
					
					lw 		$t0, CHRSIZE
					lw 		$t3, NSCOLS

					# allchars 
					move  $t8, $t5
					mul 	$t8, $t8, $t0
					mul 	$t8, $t8, $t0 							# $t8 = which * CHRSIZE^2
					
					move 	$t9, $s5
					mul 	$t9, $t9, $t0 							# $t9 = row * CHRSIZE

					add 	$t8, $t8, $t9 							# $t8 = $t8 + $t9
					add 	$t8, $t8, $s6 							# $t8 + col

					lb 		$t7, all_chars($t8)

					# bigString
					addi  $t4, $t0, 1 								# (CHRSIZE + 1)
					mul 	$t4, $t4, $s3 							# (CHRSIZE + 1) * i
					add 	$t4, $t4, $s6 							# (CHRSIZE + 1) * i + col
					
					move 	$t2, $s5
					mul 	$t2, $t2, $t3 							# row * NSCOLS
					add 	$t2, $t2, $t4 							# (row * NSCOLS) + $t4

					sb 		$t7, bigString($t2)

				bigchars_inner_increment2:
					addi 	$s6, $s6, 1
					j 		bigchars_col_loop2

				end_bigchars_col_loop2:
					addi 	$s5, $s5, 1
					j 		bigchars_row_loop2

				end_bigchars_row_loop2:
					j 		bigchars_part2

		bigchars_part2:
			move  $s6, $t0 												# move CHRSIZE into col
			add 	$s6, $s6, 1 										# (CHRSIZE + 1)
			mul 	$s6, $s6, $s3  									# (CHRSIZE + 1) * i
			add 	$s6, $s6, $t0 									# (CHRSIZE + 1) * i + CHRSIZE

			li 		$s5, 0
			bigchars_part2_loop:
				bge 	$s5, $t0, end_bigchars_part2_loop 

				# body
				move 	$t2, $s5 											# move row into $t2
				mul 	$t2, $t2, $t3 								# row * NSCOLS
				add 	$t2, $t2, $s6  								# (row * NSCOLs) + col

				li 		$t9, ' '
				sb 		$t9, bigString($t2)
				
			bigchars_part2_loop_incremement:
				addi 	$s5, $s5, 1
				j 		bigchars_part2_loop

			end_bigchars_part2_loop:
	
	bigchars_increment:
		addi  $s3, $s3, 1 	
		j 		bigchars_i_loop
	
	end_bigchars_i_loop:
		
	# [x] enough to scroll it completely off the left
	lw 		$t0, NDCOLS
	add 	$s7, $t0, $s1 				 							# iterations = NDCOLS + bigLength
	sub 	$s4, $t0, 1 												# starting_col = NDCOLS - 1

	
	# [x] final loop
	li 		$s3, 0 															# i = $s3
	final_loop:
		bge 	$s3, $s7, end_final_loop

		# setUpDisplay(starting_col, bigLength)
		move 	$a0, $s4 													# move starting_col into $a0
		move 	$a1, $s1 													# move bigLength into $a1
		jal 	setUpDisplay
		nop

		# showDisplay()
		jal  	showDisplay
		nop

		# starting_col--;
		sub 	$s4, $s4, 1

		# delay (1)
		li 		$a0, 1
		jal 	delay
		nop

	final_loop_increment:
		addi 	$s3, $s3, 1	
		j 		final_loop

	end_final_loop:

	# return 0
	move	$v0, $zero
main__post:
	# tear down stack frame
	lw 	$s7, -36($fp)
	lw 	$s6, -32($fp)
	lw 	$s5, -28($fp)
	lw 	$s4, -24($fp)
	lw 	$s3, -20($fp)
	lw	$s2, -16($fp)
	lw	$s1, -12($fp)
	lw	$s0, -8($fp)
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <setUpDisplay>
	.text
setUpDisplay:

# Frame:	$fp, $ra, ...
# Uses:		$a0, $a1, ...
# Clobbers:	...

# Locals:
#	- `row' in $s0
#	- `out_col' in $s1
#	- `in_col' in $s2
#	- `first_col' in $s3
#	- ...

# Structure:
#	setUpDisplay
#	-> [prologue]
#	-> setup
# -> starting_lt_0
# -> starting_ge_0
# 	-> double for loop
# -> part2
# 	-> double for loop
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	sw  $s0, -8($fp) 		 	 										# $s0 = row
	sw 	$s1, -12($fp) 												# $s1 = out_col
	sw  $s2, -16($fp) 												# $s2 = in_col
	sw  $s3, -20($fp) 												# $s3 = first_col
	la	$sp, -24($fp)

	# setup
	lw  $t0, NROWS 														# $t0 = NROWS
	lw 	$t3, NDCOLS 													# $t3 = NDCOLS
	lw  $t6, NSCOLS 													# $t6 = NSCOLS

	move  $t8, $a0 														# moves $a0 (starting) into $t8
	bge $t8, 0, starting_ge_0 								# if (starting < 0)

	starting_lt_0:
		li  $s1, 0 															# out_col = 0
		mul $s3, $t8, -1 												# first_col = -starting
		j 	setup_part2
	
	# else
	starting_ge_0: 														# if (starting > 0)

		li  $s1, 0
		out_col_loop:
			bge 	$s1, $t8, end_out_col_loop 			# if (out_col >= starting)

		li 	$s0, 0
		row_loop1:
			bge 	$s0, $t0, end_row_loop1 				# if (row >= NROWS)

			# display[row][out_col] = ' ';
			move 	$t2, $s0 												# move row in $t2
			mul 	$t2, $t2, $t3 									# row * NDCOLS
			add 	$t2, $t2, $s1 									# (row * NDCOLS) + out_col
			
			li 		$t7, ' '
			sb 		$t7, display($t2)
			
		setup_increment1:
			addi  $s0, $s0, 1
			j 		row_loop1

		end_row_loop1:
			addi 	$s1, $s1, 1
			j 		out_col_loop

		end_out_col_loop:
			li 		$s3, 0
			j 		setup_part2
			
	
	setup_part2:
		
		move  $s2, $s3 													# move first col into in_col 
		in_col_loop:
			bge  $s2, $a1, end_in_col_loop 				# if (first_col >= length)

		bge $s1, $t3, epilogue_setup_display 		# if (out_col >= NDCOLS)

		li 	$s0, 0
		row_loop2:
			bge  $s0, $t0, end_row_loop2

			# display[row][out_col] = bigString[row][in_col]
			move $t5, $s0 												# move row into $t5
			mul  $t5, $t5, $t6 										# row * NDCOLS
			add  $t5, $t5, $s2 										# (row * NDCOLS) + in_col 
			
			lb 	 $t7, bigString($t5)							# bigString[row][in_col]

			###
		
			move $t5, $s0 												# move row into $t5
			mul  $t5, $t5, $t3 										# row * NSCOLS
			add  $t5, $t5, $s1 										# (row * NSCOLS) + out_col

			sb 	 $t7, display($t5) 								# display[row][out_col]
			
		setup_increment2:
			addi  $s0, $s0, 1
			j 		row_loop2

		end_row_loop2:
			addi 	$s2, $s2, 1
			addi 	$s1, $s1, 1
			
			j 		in_col_loop

		end_in_col_loop:
			j 		epilogue_setup_display

	# tear down stack frame
	epilogue_setup_display:
		lw 	$s3, -20($fp)
		lw 	$s2, -16($fp)
		lw 	$s1, -12($fp)
		lw 	$s0, -8($fp)
		lw	$ra, -4($fp)
		la	$sp, 4($fp)
		lw	$fp, ($fp)
		jr	$ra
		nop	# in delay slot

########################################################################
# .TEXT <showDisplay>
	.text
showDisplay:

# Frame:	$fp, $ra, ...
# Uses:		...
# Clobbers:	...

# Locals:
#	- `i' in $s0
#	- `j' in $s1

# Structure:
#	showDisplay
#	-> [prologue]
#	-> setup
# -> printf(CLEAR)
# -> show_row_loop
# -> show_col_loop
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	sw  $s0, -8($fp) 				 								  # s0 = i (< NROWS)
	sw 	$s1, -12($fp) 			 									# s1 = j (< NDCOLS)
	addi 	$sp, $sp, -16
	
	# setup
	lw 	$t0, NROWS
	lw 	$t1, NDCOLS

	# printf(CLEAR)
	la 	$a0, CLEAR
	li 	$v0, 4
	syscall

	li 	$s0, 0
	show_row_loop:
		bge  $s0, $t0, end_show_row_loop
	
	li  $s1, 0
	show_col_loop:
		bge  $s1, $t1, end_show_col_loop

		la   $t2, display      									# display matrix stored in $t2
		move $t3, $s0 				 									# i stored in $t3

		mul  $t3, $t3, $t1 	 	 									# i * NDCOLS
		add  $t3, $t3, $s1 		 									# (i * NDCOLS) + j

		add  $t2, $t2, $t3 		 									# display + (i * NDCOLS) + j

		lb 	 $t3, ($t2) 			 									# load ($t2) into $t3
		move $a0, $t3 				 									# putchar(display[i][j])
		li 	 $v0, 11
		syscall

	show_increment:
		addi 	$s1, $s1, 1
		j 		show_col_loop

	end_show_col_loop:
		la 		$a0, eol 				 									# printf ("\n");
		li 		$v0, 4
		syscall

		addi  $s0, $s0, 1
		j 		show_row_loop

	end_show_row_loop:
		j 		epilogue_show_display

	# tear down stack frame
	epilogue_show_display:
		lw  $s1, -12($fp)
		lw 	$s0, -8($fp)
		lw	$ra, -4($fp)
		la	$sp, 4($fp)
		lw	$fp, ($fp)
		jr	$ra
		nop	# in delay slot

########################################################################
# .TEXT <delay>
	.text
delay:

# Frame:	$fp, $ra
# Uses:		$a0, $t0, $t1, $t2, $t3, $t4, $t5
# Clobbers:	$t0, $t1, $t2, $t3, $t4, $t5

# Locals:
#	- `n' in $a0
#	- `x' in $t0
#	- `i' in $t1
#	- `j' in $t2
#	- `k' in $t3

# Structure:
#	delay
#	-> [prologue]
#	-> delay_i_init
#	-> delay_i_cond
#	   -> delay_j_init
#	   -> delay_j_cond
#	      -> delay_k_init
#	      -> delay_k_cond
#	         -> delay_k_step
#	      -> delay_k_f
#	      -> delay_j_step
#	   -> delay_j_f
#	   -> delay_i_step
#	-> delay_i_f
#	-> [epilogue]

# Code:
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# x <- 0
	move	$t0, $zero
	# These values control the busy-wait. (values changed)
	li	$t4, 100
	li	$t5, 30

delay_i_init:
	# i = 0;
	move	$t1, $zero
delay_i_cond:
	# i < n;
	bge	$t1, $a0, delay_i_f
	nop	# in delay slot

delay_j_init:
	# j = 0;
	move	$t2, $zero
delay_j_cond:
	# j < DELAY_J;
	bge	$t2, $t4, delay_j_f
	nop	# in delay slot

delay_k_init:
	# k = 0;
	move	$t3, $zero
delay_k_cond:
	# k < DELAY_K;
	bge	$t3, $t5, delay_k_f
	nop	# in delay slot

	# x = x + 1
	addi	$t0, $t0, 1

delay_k_step:
	# k = k + 1
	addi	$t3, $t3, 1
	j	delay_k_cond
	nop	# in delay slot
delay_k_f:

delay_j_step:
	# j = j + 1
	addi	$t2, $t2, 1
	j	delay_j_cond
	nop	# in delay slot
delay_j_f:

delay_i_step:
	# i = i + 1
	addi	$t1, $t1, 1
	j	delay_i_cond
	nop	# in delay slot
delay_i_f:

delay__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isUpper>
	.text
isUpper: 		# behaves similarly to isLower

# Frame:	$fp, $ra, ...
# Uses:		$a0, ...
# Clobbers:	$v0, ...

# Locals: 	# same as isLower
#	- `ch' in $a0
# - ... $v0 used as temporary register

# Structure:
#	isUpper
#	-> [prologue]
# [ch cond]
# 	| isUpper_ch_ge_A
# 	| isUpper_ch_le_Z
# 	| isUpper_ch_lt_A
# 	| isUpper_ch_gt_Z
# -> isUpper_ch_phi
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# if (ch >= 'A')
	li  $v0, 'A'
	blt $a0, $v0, isUpper_ch_lt_A
	nop
isUpper_ch_ge_A:
	# if (ch <= 'Z')
	li 	$v0 , 'Z'
	bgt $a0, $v0, isUpper_ch_gt_Z
	nop
isUpper_ch_le_Z:
	addi 	$v0, $zero, 1
	j isUpper_ch_phi
	nop

	# ... else
isUpper_ch_lt_A:
isUpper_ch_gt_Z:
	move 	$v0, $zero
	# fallthrough
isUpper_ch_phi:

isUpper__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isLower>
	.text
isLower:

# Frame:	$fp, $ra
# Uses:		$a0
# Clobbers:	$v0

# Locals:
#	- `ch' in $a0
#	- ... $v0 used as temporary register

# Structure:
#	isLower
#	-> [prologue]
#	[ch cond]
#	   | isLower_ch_ge_a
#	   | isLower_ch_le_z
#	   | isLower_ch_lt_a
#	   | isLower_ch_gt_z
#	-> isLower_ch_phi
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# if (ch >= 'a')
	li	$v0, 'a'
	blt	$a0, $v0, isLower_ch_lt_a
	nop	# in delay slot
isLower_ch_ge_a:
	# if (ch <= 'z')
	li	$v0, 'z'
	bgt	$a0, $v0, isLower_ch_gt_z
	nop	# in delay slot
isLower_ch_le_z:
	addi	$v0, $zero, 1
	j	isLower_ch_phi
	nop	# in delay slot

	# ... else
isLower_ch_lt_a:
isLower_ch_gt_z:
	move	$v0, $zero
	# fallthrough
isLower_ch_phi:

isLower__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

#################################################################### EOF
