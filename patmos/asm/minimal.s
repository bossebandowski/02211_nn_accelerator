#
# A short as possible assembler example
#

	.word   36
	addi	r1 = r0, 255  # first instruction maybe not executed
	addi	r2 = r0, 1
	addi	r3 = r0, 2
	add	r4 = r2, r3
	halt
	nop
	nop
	nop
