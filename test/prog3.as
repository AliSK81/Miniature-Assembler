	j	start	
edit	addi	4,0,3000
	sw	4,0,N2
start	lw	1,0,N1
	lw	2,0,N2
	sub	3,1,2
	add	4,2,1
	slt	5,3,4
	beq	5,0,edit
	halt
N1	.fill	1000
N2	.fill	-2000