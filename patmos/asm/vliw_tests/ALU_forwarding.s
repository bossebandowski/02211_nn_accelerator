# Test case for forwarding of ALU instructions
	.word	136
	addi	r4 = r0, 0
	addi	r4 = r0, 0
	addi	r1 = r0, 2		||	addi	r1 = r0, 5
	add 	r2 = r1, r1		||	add 	r3 = r1, r1
	add 	r4 = r3, r2 	||	add 	r5 = r4, r2
	add 	r6 = r3, r5 	||	add 	r7 = r4, r5
	sub		r1 = r2, r3 	||	sub		r1 = r1, r4
	subi	r8 = r0, 8		||	subi	r9 = r0, 5
	sub		r10 = r8, r9	||	sub 	r11 = r8, r9
	add		r10 = r8, r9	||	add 	r11 = r8, r9
	and		r10 = r8, r9	||	and 	r11 = r8, r9
	addi	r12 = r0, 70
#	addi	r12 = r0, 5000
	subi	r1 = r0, 3		||	or		r13 = r12, r11
	sli		r15 = r12, 5	||	sri 	r16 = r12, 5
	sl		r16 = r10, r1	||	sr 		r17 = r10, r1
	sra		r19 = r12, r1	||	srai 	r18 = r12, 5
	halt
	nop
	nop
	nop
