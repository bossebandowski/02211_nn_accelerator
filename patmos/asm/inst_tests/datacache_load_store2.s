# Test case for data cache load and store
	.word	252
	add 	r1 = r0, 0xFF0FF000
	add 	r1 = r0, 0xFF0FF000
	addi	r2 = r0, 4
	swc		[r2 + 0] = r1
	swc		[r2 + 0] = r1
	lbc		r3 = [r2 + 0]
	lbc		r4 = [r2 + 1]
	lbc		r5 = [r2 + 2]
	lbc		r6 = [r2 + 3]
	lhc		r7 = [r2 + 0]
	lhc		r8 = [r2 + 1]
	lwc		r9 = [r2 + 0]
	lbuc	r10 = [r2 + 0]
	lbuc	r11 = [r2 + 1]
	lbuc	r12 = [r2 + 2]
	lbuc	r13 = [r2 + 3]
	lhuc	r14 = [r2 + 0]
	lhuc	r15 = [r2 + 1]
	shc		[r2 + 0] = r1
	shc		[r2 + 1] = r1
	shc		[r2 + 0] = r1
	shc		[r2 + 1] = r1
	lbc		r3 = [r2 + 0]
	lbc		r4 = [r2 + 1]
	lbc		r5 = [r2 + 2]
	lbc		r6 = [r2 + 3]
	lhc		r7 = [r2 + 0]
	lhc		r8 = [r2 + 1]
	lwc		r9 = [r2 + 0]
	lbuc	r10 = [r2 + 0]
	lbuc	r11 = [r2 + 1]
	lbuc	r12 = [r2 + 2]
	lbuc	r13 = [r2 + 3]
	lhuc	r14 = [r2 + 0]
	lhuc	r15 = [r2 + 1]
	sbc		[r2 + 0] = r1
	sbc		[r2 + 1] = r1
	sbc		[r2 + 2] = r1
	sbc		[r2 + 3] = r1
	sbc		[r2 + 0] = r1
	sbc		[r2 + 1] = r1
	sbc		[r2 + 2] = r1
	sbc		[r2 + 3] = r1
	lbc		r3 = [r2 + 0]
	lbc		r4 = [r2 + 1]
	lbc		r5 = [r2 + 2]
	lbc		r6 = [r2 + 3]
	lhc		r7 = [r2 + 0]
	lhc		r8 = [r2 + 1]
	lwc		r9 = [r2 + 0]
	lbuc	r10 = [r2 + 0]
	lbuc	r11 = [r2 + 1]
	lbuc	r12 = [r2 + 2]
	lbuc	r13 = [r2 + 3]
	lhuc	r14 = [r2 + 0]
	lhuc	r15 = [r2 + 1]
	halt
	nop
	nop
	nop
