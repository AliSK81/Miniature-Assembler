hello	addi	10,0,-100
	addi	11,0,100
	beq	10,11,main
hello	add	12,10,11	# redefinition of label hello
bye	halt