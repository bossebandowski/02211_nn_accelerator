#
# This is a simple output of a single character on the UART
#
# TODO: maybe this should just switch a LED to see the result.
#

# TODO: looks like the UART is in memory address 0....

	.word   40
	addi	r0 = r0, 0  # first instruction not executed
	add	r1 = r0, 0xf0080000
	addi	r2 = r0, 42 # '*'
	swl	[r1 + 1] = r2
	halt
	nop
	nop
	nop
