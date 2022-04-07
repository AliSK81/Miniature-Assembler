	lui     1,15
	lui	2,31
	lw	3,0,origin
	nand	3,2,3
	ori	4,0,16383
	nand	4,1,4
	or	1,3,4
	sw	1,0,1000
	halt
origin	.fill	8064