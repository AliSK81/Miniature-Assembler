	add	1,0,0
start	addi	2,0,5
	sub	3,2,1
	beq	3,0,exit
	j	begin		# begin label is undefined
exit	halt
