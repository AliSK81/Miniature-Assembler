main	ori	1,0,-450
	lw	2,0,addr
	jalr	3,2
	sw	5,0,1000
	halt
isNeg	add	5,0,0
	slt	4,1,0
if	beq	4,0,return
	addi	5,0,1
return	jalr	0,3
addr	.fill	isNeg
