# Test case for global data memory load and store
	.word	252
	add 	r1 = r0, 0xFF0FF000
	add 	r1 = r0, 0xFF0FF000
	addi	r2 = r0, 4
	swm		[r2 + 0] = r1
	swm		[r2 + 0] = r1
	lbm		r3 = [r2 + 0]
	lbm		r4 = [r2 + 1]
	lbm		r5 = [r2 + 2]
	lbm		r6 = [r2 + 3]
	lhm		r7 = [r2 + 0]
	lhm		r8 = [r2 + 1]
	lwm		r9 = [r2 + 0]
	lbum	r10 = [r2 + 0]
	lbum	r11 = [r2 + 1]
	lbum	r12 = [r2 + 2]
	lbum	r13 = [r2 + 3]
	lhum	r14 = [r2 + 0]
	lhum	r15 = [r2 + 1]
	shm		[r2 + 0] = r1
	shm		[r2 + 1] = r1
	shm		[r2 + 0] = r1
	shm		[r2 + 1] = r1
	lbm		r3 = [r2 + 0]
	lbm		r4 = [r2 + 1]
	lbm		r5 = [r2 + 2]
	lbm		r6 = [r2 + 3]
	lhm		r7 = [r2 + 0]
	lhm		r8 = [r2 + 1]
	lwm		r9 = [r2 + 0]
	lbum	r10 = [r2 + 0]
	lbum	r11 = [r2 + 1]
	lbum	r12 = [r2 + 2]
	lbum	r13 = [r2 + 3]
	lhum	r14 = [r2 + 0]
	lhum	r15 = [r2 + 1]
	sbm		[r2 + 0] = r1
	sbm		[r2 + 1] = r1
	sbm		[r2 + 2] = r1
	sbm		[r2 + 3] = r1
	sbm		[r2 + 0] = r1
	sbm		[r2 + 1] = r1
	sbm		[r2 + 2] = r1
	sbm		[r2 + 3] = r1
	lbm		r3 = [r2 + 0]
	lbm		r4 = [r2 + 1]
	lbm		r5 = [r2 + 2]
	lbm		r6 = [r2 + 3]
	lhm		r7 = [r2 + 0]
	lhm		r8 = [r2 + 1]
	lwm		r9 = [r2 + 0]
	lbum	r10 = [r2 + 0]
	lbum	r11 = [r2 + 1]
	lbum	r12 = [r2 + 2]
	lbum	r13 = [r2 + 3]
	lhum	r14 = [r2 + 0]
	lhum	r15 = [r2 + 1]
	halt
	nop
	nop
	nop
