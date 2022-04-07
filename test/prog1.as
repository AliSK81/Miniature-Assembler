	lw	1,0,six
	addi	1,1,2
	lw	2,1,1
	beq	1,2,sum	
exit	halt
sum	add	1,1,2
	sw	1,0,1234
	j	exit
six	.fill	6
eight	.fill	8
temp	.fill	sum
