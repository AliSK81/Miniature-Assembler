	lw	1,0,num1
	lw	2,0,num2
	ori	3,0,1
	lui	3,2
	addi	3,3,-29000
	slti	4,3,25985
	beq	0,4,exit
	sw	4,0,1035
exit	halt
num1	.fill	1025
num2	.fill	7