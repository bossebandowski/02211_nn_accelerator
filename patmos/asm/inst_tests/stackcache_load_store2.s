# Test case for stack cache load and store
	.word	264
	add 	r1 = r0, 0xFF0FF000
	add 	r1 = r0, 0xFF0FF000
	addi	r2 = r0, 4
	addi    r3 = r0, 0x100
	mts     s6 = r3
	sres	32
	sws		[r2 + 0] = r1
	sws		[r2 + 0] = r1
	lbs		r3 = [r2 + 0]
	lbs		r4 = [r2 + 1]
	lbs		r5 = [r2 + 2]
	lbs		r6 = [r2 + 3]
	lhs		r7 = [r2 + 0]
	lhs		r8 = [r2 + 1]
	lws		r9 = [r2 + 0]
	lbus	r10 = [r2 + 0]
	lbus	r11 = [r2 + 1]
	lbus	r12 = [r2 + 2]
	lbus	r13 = [r2 + 3]
	lhus	r14 = [r2 + 0]
	lhus	r15 = [r2 + 1]
	shs		[r2 + 0] = r1
	shs		[r2 + 1] = r1
	shs		[r2 + 0] = r1
	shs		[r2 + 1] = r1
	lbs		r3 = [r2 + 0]
	lbs		r4 = [r2 + 1]
	lbs		r5 = [r2 + 2]
	lbs		r6 = [r2 + 3]
	lhs		r7 = [r2 + 0]
	lhs		r8 = [r2 + 1]
	lws		r9 = [r2 + 0]
	lbus	r10 = [r2 + 0]
	lbus	r11 = [r2 + 1]
	lbus	r12 = [r2 + 2]
	lbus	r13 = [r2 + 3]
	lhus	r14 = [r2 + 0]
	lhus	r15 = [r2 + 1]
	sbs		[r2 + 0] = r1
	sbs		[r2 + 1] = r1
	sbs		[r2 + 2] = r1
	sbs		[r2 + 3] = r1
	sbs		[r2 + 0] = r1
	sbs		[r2 + 1] = r1
	sbs		[r2 + 2] = r1
	sbs		[r2 + 3] = r1
	lbs		r3 = [r2 + 0]
	lbs		r4 = [r2 + 1]
	lbs		r5 = [r2 + 2]
	lbs		r6 = [r2 + 3]
	lhs		r7 = [r2 + 0]
	lhs		r8 = [r2 + 1]
	lws		r9 = [r2 + 0]
	lbus	r10 = [r2 + 0]
	lbus	r11 = [r2 + 1]
	lbus	r12 = [r2 + 2]
	lbus	r13 = [r2 + 3]
	lhus	r14 = [r2 + 0]
	lhus	r15 = [r2 + 1]
	halt
	nop
	nop
	nop
