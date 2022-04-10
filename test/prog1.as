	lw	1,0,nine	# load reg1 with 9
	lw	2,1,2		# reg2 will be 7 (addr=9+2=11)
	addi	2,2,1		# increament reg2
	addi	1,1,-1		# decreament reg1
	beq	1,2,sum	
exit	halt
sum	add	1,1,2		# sum reg1 & reg2 and put in reg1
	sw	1,0,1234	# save reg1 in address 1234
	j	exit
nine	.fill	9
temp	.fill	sum		# keeps the address of sum label (6)
seven	.fill	7
