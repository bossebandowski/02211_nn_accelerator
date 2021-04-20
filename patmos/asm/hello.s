#
# The embedded version of Hello World: a blinking LED
#
# Expected Result: LED blinks
#

	.word   56

        add     r7  = r0, 0xF0090000
	addi	r8 = r0, 1

loop:	xor	r9 = r9, r8  # toggle value
	swl	[r7+0] = r9  # set the LED

	addi	r1 = r0, 1024		
	sli	r1 = r1, 10

wloop:	subi	r1 = r1, 1
	cmpneq	p1 = r1, r0
(p1)	br	wloop
        addi    r0  = r0 , 0
        addi    r0  = r0 , 0
	br	loop
