
#ifdef __ELF__
	.text
	.p2align 4,,15
.globl snapshot
	.type	snapshot, @function
snapshot:
.LFB3:
	movq	%rax, rax(%rip)
	movq	%rbx, rbx(%rip)
	movq	%rcx, rcx(%rip)
	movq	%rdx, rdx(%rip)
	movq	%rdi, rdi(%rip)
	movq	%rsi, rsi(%rip)
	movq	%rbp, rbp(%rip)
	movq	%rsp, rsp(%rip)
	movq	%r8, r8(%rip)
	movq	%r9, r9(%rip)
	movq	%r10, r10(%rip)
	movq	%r11, r11(%rip)
	movq	%r12, r12(%rip)
	movq	%r13, r13(%rip)
	movq	%r14, r14(%rip)
	movq	%r15, r15(%rip)
	vmovdqu	%ymm0, ymm_regs+0(%rip)
	vmovdqu	%ymm1, ymm_regs+32(%rip)
	vmovdqu	%ymm2, ymm_regs+64(%rip)
	vmovdqu	%ymm3, ymm_regs+96(%rip)
	vmovdqu	%ymm4, ymm_regs+128(%rip)
	vmovdqu	%ymm5, ymm_regs+160(%rip)
	vmovdqu	%ymm6, ymm_regs+192(%rip)
	vmovdqu	%ymm7, ymm_regs+224(%rip)
	vmovdqu	%ymm8, ymm_regs+256(%rip)
	vmovdqu	%ymm9, ymm_regs+288(%rip)
	vmovdqu	%ymm10, ymm_regs+320(%rip)
	vmovdqu	%ymm11, ymm_regs+352(%rip)
	vmovdqu	%ymm12, ymm_regs+384(%rip)
	vmovdqu	%ymm13, ymm_regs+416(%rip)
	vmovdqu	%ymm14, ymm_regs+448(%rip)
	vmovdqu	%ymm15, ymm_regs+480(%rip)
	jmp	*callthis(%rip)
.LFE3:
	.size	snapshot, .-snapshot

	.p2align 4,,15
.globl snapshot_ret
	.type	snapshot_ret, @function
snapshot_ret:
	movq	%rdi, rdi(%rip)
	subq	$8, %rsp
	call	*callthis(%rip)
	addq	$8, %rsp
	movq	%rax, rax(%rip)
	movq	%rdx, rdx(%rip)
	vmovdqu	%ymm0, ymm_regs+0(%rip)
	vmovdqu	%ymm1, ymm_regs+32(%rip)
	fstpt	x87_regs(%rip)
	fstpt	x87_regs+16(%rip)
	fldt	x87_regs+16(%rip)
	fldt	x87_regs(%rip)
	ret
	.size	snapshot_ret, .-snapshot_ret

	.comm	callthis,8,8
	.comm	rax,8,8
	.comm	rbx,8,8
	.comm	rcx,8,8
	.comm	rdx,8,8
	.comm	rsi,8,8
	.comm	rdi,8,8
	.comm	rsp,8,8
	.comm	rbp,8,8
	.comm	r8,8,8
	.comm	r9,8,8
	.comm	r10,8,8
	.comm	r11,8,8
	.comm	r12,8,8
	.comm	r13,8,8
	.comm	r14,8,8
	.comm	r15,8,8
	.comm	ymm_regs,512,32
	.comm	x87_regs,128,32
	.comm   volatile_var,8,8
#ifdef __linux__
	.section	.note.GNU-stack,"",@progbits
#endif

#elif defined(__APPLE__)

	.text
	.p2align 4,,15
	.globl _snapshot
_snapshot:
.LFB3:
	movq	%rax, _rax(%rip)
	movq	%rbx, _rbx(%rip)
	movq	%rcx, _rcx(%rip)
	movq	%rdx, _rdx(%rip)
	movq	%rdi, _rdi(%rip)
	movq	%rsi, _rsi(%rip)
	movq	%rbp, _rbp(%rip)
	movq	%rsp, _rsp(%rip)
	movq	%r8, _r8(%rip)
	movq	%r9, _r9(%rip)
	movq	%r10, _r10(%rip)
	movq	%r11, _r11(%rip)
	movq	%r12, _r12(%rip)
	movq	%r13, _r13(%rip)
	movq	%r14, _r14(%rip)
	movq	%r15, _r15(%rip)
	vmovdqu	%ymm0, _ymm_regs+0(%rip)
	vmovdqu	%ymm1, _ymm_regs+32(%rip)
	vmovdqu	%ymm2, _ymm_regs+64(%rip)
	vmovdqu	%ymm3, _ymm_regs+96(%rip)
	vmovdqu	%ymm4, _ymm_regs+128(%rip)
	vmovdqu	%ymm5, _ymm_regs+160(%rip)
	vmovdqu	%ymm6, _ymm_regs+192(%rip)
	vmovdqu	%ymm7, _ymm_regs+224(%rip)
	vmovdqu	%ymm8, _ymm_regs+256(%rip)
	vmovdqu	%ymm9, _ymm_regs+288(%rip)
	vmovdqu	%ymm10, _ymm_regs+320(%rip)
	vmovdqu	%ymm11, _ymm_regs+352(%rip)
	vmovdqu	%ymm12, _ymm_regs+384(%rip)
	vmovdqu	%ymm13, _ymm_regs+416(%rip)
	vmovdqu	%ymm14, _ymm_regs+448(%rip)
	vmovdqu	%ymm15, _ymm_regs+480(%rip)
	jmp	*_callthis(%rip)
.LFE3:

	.p2align 4,,15
	.globl _snapshot_ret
_snapshot_ret:
	movq	%rdi, _rdi(%rip)
	subq	$8, %rsp
	call	*_callthis(%rip)
	addq	$8, %rsp
	movq	%rax, _rax(%rip)
	movq	%rdx, _rdx(%rip)
	vmovdqu	%ymm0, _ymm_regs+0(%rip)
	vmovdqu	%ymm1, _ymm_regs+32(%rip)
	fstpt	_x87_regs(%rip)
	fstpt	_x87_regs+16(%rip)
	fldt	_x87_regs+16(%rip)
	fldt	_x87_regs(%rip)
	ret

	.globl	_callthis
	.zerofill __DATA,__bss,_callthis,8,3
	.globl	_rax
	.zerofill __DATA,__bss,_rax,8,3
	.globl	_rbx
	.zerofill __DATA,__bss,_rbx,8,3
	.globl	_rcx
	.zerofill __DATA,__bss,_rcx,8,3
	.globl	_rdx
	.zerofill __DATA,__bss,_rdx,8,3
	.globl	_rsi
	.zerofill __DATA,__bss,_rsi,8,3
	.globl	_rdi
	.zerofill __DATA,__bss,_rdi,8,3
	.globl	_rsp
	.zerofill __DATA,__bss,_rsp,8,3
	.globl	_rbp
	.zerofill __DATA,__bss,_rbp,8,3
	.globl	_r8
	.zerofill __DATA,__bss,_r8,8,3
	.globl	_r9
	.zerofill __DATA,__bss,_r9,8,3
	.globl	_r10
	.zerofill __DATA,__bss,_r10,8,3
	.globl	_r11
	.zerofill __DATA,__bss,_r11,8,3
	.globl	_r12
	.zerofill __DATA,__bss,_r12,8,3
	.globl	_r13
	.zerofill __DATA,__bss,_r13,8,3
	.globl	_r14
	.zerofill __DATA,__bss,_r14,8,3
	.globl	_r15
	.zerofill __DATA,__bss,_r15,8,3
	.globl	_ymm_regs
	.zerofill __DATA,__bss,_ymm_regs,512,5
	.globl	_x87_regs
	.zerofill __DATA,__bss,_x87_regs,128,5
	.globl	_volatile_var
	.zerofill __DATA,__bss,_volatile_var,8,3

#else
#error unknown object format
#endif

