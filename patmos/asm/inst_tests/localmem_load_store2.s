# Test case for local memory load and store
	.word	252
	add 	r1 = r0, 0xFF0FF000
	add 	r1 = r0, 0xFF0FF000
	addi	r2 = r0, 4
	swl		[r2 + 0] = r1
	swl		[r2 + 0] = r1
	lbl		r3 = [r2 + 0]
	lbl		r4 = [r2 + 1]
	lbl		r5 = [r2 + 2]
	lbl		r6 = [r2 + 3]
	lhl		r7 = [r2 + 0]
	lhl		r8 = [r2 + 1]
	lwl		r9 = [r2 + 0]
	lbul	r10 = [r2 + 0]
	lbul	r11 = [r2 + 1]
	lbul	r12 = [r2 + 2]
	lbul	r13 = [r2 + 3]
	lhul	r14 = [r2 + 0]
	lhul	r15 = [r2 + 1]
	shl		[r2 + 0] = r1
	shl		[r2 + 1] = r1
	shl		[r2 + 0] = r1
	shl		[r2 + 1] = r1
	lbl		r3 = [r2 + 0]
	lbl		r4 = [r2 + 1]
	lbl		r5 = [r2 + 2]
	lbl		r6 = [r2 + 3]
	lhl		r7 = [r2 + 0]
	lhl		r8 = [r2 + 1]
	lwl		r9 = [r2 + 0]
	lbul	r10 = [r2 + 0]
	lbul	r11 = [r2 + 1]
	lbul	r12 = [r2 + 2]
	lbul	r13 = [r2 + 3]
	lhul	r14 = [r2 + 0]
	lhul	r15 = [r2 + 1]
	sbl		[r2 + 0] = r1
	sbl		[r2 + 1] = r1
	sbl		[r2 + 2] = r1
	sbl		[r2 + 3] = r1
	sbl		[r2 + 0] = r1
	sbl		[r2 + 1] = r1
	sbl		[r2 + 2] = r1
	sbl		[r2 + 3] = r1
	lbl		r3 = [r2 + 0]
	lbl		r4 = [r2 + 1]
	lbl		r5 = [r2 + 2]
	lbl		r6 = [r2 + 3]
	lhl		r7 = [r2 + 0]
	lhl		r8 = [r2 + 1]
	lwl		r9 = [r2 + 0]
	lbul	r10 = [r2 + 0]
	lbul	r11 = [r2 + 1]
	lbul	r12 = [r2 + 2]
	lbul	r13 = [r2 + 3]
	lhul	r14 = [r2 + 0]
	lhul	r15 = [r2 + 1]
	halt
	nop
	nop
	nop
