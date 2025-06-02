	.text
	.file	"lab2.cpp"
                                        # Start of file scope inline assembly
	.globl	_ZSt21ios_base_library_initv

                                        # End of file scope inline assembly
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          # -- Begin function _Z8init_rhov
.LCPI0_0:
	.quad	0x3f4062d40efa55d6              # double 5.0006250781347672E-4
.LCPI0_1:
	.quad	0xc005555555555555              # double -2.6666666666666665
.LCPI0_2:
	.quad	0xbff5555555555555              # double -1.3333333333333333
.LCPI0_3:
	.quad	0x3fc47ae147ae147c              # double 0.16000000000000003
	.text
	.globl	_Z8init_rhov
	.p2align	4, 0x90
	.type	_Z8init_rhov,@function
_Z8init_rhov:                           # @_Z8init_rhov
	.cfi_startproc
# %bb.0:
	xorl	%eax, %eax
	movsd	.LCPI0_0(%rip), %xmm0           # xmm0 = [5.0006250781347672E-4,0.0E+0]
	movsd	.LCPI0_1(%rip), %xmm1           # xmm1 = [-2.6666666666666665E+0,0.0E+0]
	movsd	.LCPI0_2(%rip), %xmm2           # xmm2 = [-1.3333333333333333E+0,0.0E+0]
	movsd	.LCPI0_3(%rip), %xmm3           # xmm3 = [1.6000000000000003E-1,0.0E+0]
	movabsq	$4591870180066957722, %rcx      # imm = 0x3FB999999999999A
	movabsq	$-4631501856787818086, %rdx     # imm = 0xBFB999999999999A
	xorl	%esi, %esi
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_4:                                #   in Loop: Header=BB0_1 Depth=1
	incq	%rsi
	addq	$8, %rax
	cmpq	$8000, %rsi                     # imm = 0x1F40
	je	.LBB0_5
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_2 Depth 2
	xorps	%xmm4, %xmm4
	cvtsi2sd	%esi, %xmm4
	mulsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm5
	addsd	%xmm1, %xmm5
	mulsd	%xmm5, %xmm5
	addsd	%xmm2, %xmm4
	mulsd	%xmm4, %xmm4
	movq	rho_grid(%rip), %rdi
	addq	%rax, %rdi
	xorl	%r8d, %r8d
	jmp	.LBB0_2
	.p2align	4, 0x90
.LBB0_3:                                #   in Loop: Header=BB0_2 Depth=2
	movq	%rcx, (%rdi)
.LBB0_9:                                #   in Loop: Header=BB0_2 Depth=2
	incq	%r8
	addq	$64000, %rdi                    # imm = 0xFA00
	cmpq	$8000, %r8                      # imm = 0x1F40
	je	.LBB0_4
.LBB0_2:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	xorps	%xmm6, %xmm6
	cvtsi2sd	%r8d, %xmm6
	mulsd	%xmm0, %xmm6
	movapd	%xmm6, %xmm7
	addsd	%xmm2, %xmm7
	mulsd	%xmm7, %xmm7
	addsd	%xmm5, %xmm7
	ucomisd	%xmm3, %xmm7
	jb	.LBB0_3
# %bb.6:                                #   in Loop: Header=BB0_2 Depth=2
	addsd	%xmm1, %xmm6
	mulsd	%xmm6, %xmm6
	addsd	%xmm4, %xmm6
	ucomisd	%xmm3, %xmm6
	jae	.LBB0_8
# %bb.7:                                #   in Loop: Header=BB0_2 Depth=2
	movq	%rdx, (%rdi)
	jmp	.LBB0_9
	.p2align	4, 0x90
.LBB0_8:                                #   in Loop: Header=BB0_2 Depth=2
	movq	$0, (%rdi)
	jmp	.LBB0_9
.LBB0_5:
	retq
.Lfunc_end0:
	.size	_Z8init_rhov, .Lfunc_end0-_Z8init_rhov
	.cfi_endproc
                                        # -- End function
	.globl	_Z4dumpv                        # -- Begin function _Z4dumpv
	.p2align	4, 0x90
	.type	_Z4dumpv,@function
_Z4dumpv:                               # @_Z4dumpv
.Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, DW.ref.__gxx_personality_v0
	.cfi_lsda 27, .Lexception0
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$512, %rsp                      # imm = 0x200
	.cfi_def_cfa_offset 528
	.cfi_offset %rbx, -16
	leaq	.L.str(%rip), %rsi
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movl	$4, %edx
	callq	_ZNSt14basic_ofstreamIcSt11char_traitsIcEEC1EPKcSt13_Ios_Openmode@PLT
	movq	Phi_grid_new(%rip), %rsi
.Ltmp0:
	movl	$512000000, %edx                # imm = 0x1E848000
	movq	%rbx, %rdi
	callq	_ZNSo5writeEPKcl@PLT
.Ltmp1:
# %bb.1:
	leaq	8(%rsp), %rdi
.Ltmp2:
	callq	_ZNSt13basic_filebufIcSt11char_traitsIcEE5closeEv@PLT
.Ltmp3:
# %bb.2:
	testq	%rax, %rax
	jne	.LBB1_4
# %bb.3:
	movq	(%rsp), %rax
	movq	-24(%rax), %rax
	leaq	(%rsp,%rax), %rdi
	movl	32(%rsp,%rax), %esi
	orl	$4, %esi
.Ltmp4:
	callq	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
.Ltmp5:
.LBB1_4:
	movq	%rsp, %rdi
	callq	_ZNSt14basic_ofstreamIcSt11char_traitsIcEED1Ev@PLT
	addq	$512, %rsp                      # imm = 0x200
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB1_5:
	.cfi_def_cfa_offset 528
.Ltmp6:
	movq	%rax, %rbx
	movq	%rsp, %rdi
	callq	_ZNSt14basic_ofstreamIcSt11char_traitsIcEED1Ev@PLT
	movq	%rbx, %rdi
	callq	_Unwind_Resume@PLT
.Lfunc_end1:
	.size	_Z4dumpv, .Lfunc_end1-_Z4dumpv
	.cfi_endproc
	.section	.gcc_except_table,"a",@progbits
	.p2align	2, 0x0
GCC_except_table1:
.Lexception0:
	.byte	255                             # @LPStart Encoding = omit
	.byte	255                             # @TType Encoding = omit
	.byte	1                               # Call site Encoding = uleb128
	.uleb128 .Lcst_end0-.Lcst_begin0
.Lcst_begin0:
	.uleb128 .Lfunc_begin0-.Lfunc_begin0    # >> Call Site 1 <<
	.uleb128 .Ltmp0-.Lfunc_begin0           #   Call between .Lfunc_begin0 and .Ltmp0
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp0-.Lfunc_begin0           # >> Call Site 2 <<
	.uleb128 .Ltmp5-.Ltmp0                  #   Call between .Ltmp0 and .Ltmp5
	.uleb128 .Ltmp6-.Lfunc_begin0           #     jumps to .Ltmp6
	.byte	0                               #   On action: cleanup
	.uleb128 .Ltmp5-.Lfunc_begin0           # >> Call Site 3 <<
	.uleb128 .Lfunc_end1-.Ltmp5             #   Call between .Ltmp5 and .Lfunc_end1
	.byte	0                               #     has no landing pad
	.byte	0                               #   On action: cleanup
.Lcst_end0:
	.p2align	2, 0x0
                                        # -- End function
	.text
	.globl	_Z12update_layerv               # -- Begin function _Z12update_layerv
	.p2align	4, 0x90
	.type	_Z12update_layerv,@function
_Z12update_layerv:                      # @_Z12update_layerv
	.cfi_startproc
# %bb.0:
	movq	Phi_grid_new(%rip), %rax
	movq	Phi_grid(%rip), %rcx
	movq	%rcx, Phi_grid_new(%rip)
	movq	%rax, Phi_grid(%rip)
	retq
.Lfunc_end2:
	.size	_Z12update_layerv, .Lfunc_end2-_Z12update_layerv
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          # -- Begin function main
.LCPI3_0:
	.quad	0x3f4062d40efa55d6              # double 5.0006250781347672E-4
.LCPI3_1:
	.quad	0xc005555555555555              # double -2.6666666666666665
.LCPI3_2:
	.quad	0xbff5555555555555              # double -1.3333333333333333
.LCPI3_3:
	.quad	0x3fc47ae147ae147c              # double 0.16000000000000003
.LCPI3_11:
	.quad	0x3e112e0be826d695              # double 1.0000000000000001E-9
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0
.LCPI3_4:
	.quad	-9223372034707292160            # 0x8000000080000000
	.quad	-9223372034707292160            # 0x8000000080000000
.LCPI3_5:
	.quad	0x3fa9999963e6450e              # double 0.049999993748437985
	.quad	0x3fa9999963e6450e              # double 0.049999993748437985
.LCPI3_6:
	.quad	0x3e6ad9aa4584cb98              # double 5.0012496091015501E-8
	.quad	0x3e6ad9aa4584cb98              # double 5.0012496091015501E-8
.LCPI3_7:
	.quad	0x3e3ad9aa4584cb98              # double 6.2515620113769376E-9
	.quad	0x3e3ad9aa4584cb98              # double 6.2515620113769376E-9
.LCPI3_8:
	.quad	0x3fc9999963e64510              # double 0.19999997499375199
	.quad	0x3fc9999963e64510              # double 0.19999997499375199
.LCPI3_9:
	.quad	0x3fc9999963e64510              # double 0.19999997499375199
	.quad	0x3fa9999963e6450e              # double 0.049999993748437985
.LCPI3_10:
	.quad	0x3e6ad9aa4584cb98              # double 5.0012496091015501E-8
	.quad	0x3e3ad9aa4584cb98              # double 6.2515620113769376E-9
	.text
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r15
	.cfi_def_cfa_offset 24
	pushq	%r14
	.cfi_def_cfa_offset 32
	pushq	%r13
	.cfi_def_cfa_offset 40
	pushq	%r12
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	subq	$312, %rsp                      # imm = 0x138
	.cfi_def_cfa_offset 368
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	.cfi_offset %rbp, -16
	xorl	%eax, %eax
	movsd	.LCPI3_0(%rip), %xmm0           # xmm0 = [5.0006250781347672E-4,0.0E+0]
	movsd	.LCPI3_1(%rip), %xmm1           # xmm1 = [-2.6666666666666665E+0,0.0E+0]
	movsd	.LCPI3_2(%rip), %xmm2           # xmm2 = [-1.3333333333333333E+0,0.0E+0]
	movsd	.LCPI3_3(%rip), %xmm3           # xmm3 = [1.6000000000000003E-1,0.0E+0]
	movabsq	$4591870180066957722, %rcx      # imm = 0x3FB999999999999A
	movabsq	$-4631501856787818086, %rdx     # imm = 0xBFB999999999999A
	xorl	%esi, %esi
	jmp	.LBB3_1
	.p2align	4, 0x90
.LBB3_8:                                #   in Loop: Header=BB3_1 Depth=1
	incq	%rsi
	addq	$8, %rax
	cmpq	$8000, %rsi                     # imm = 0x1F40
	je	.LBB3_9
.LBB3_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB3_2 Depth 2
	xorps	%xmm4, %xmm4
	cvtsi2sd	%esi, %xmm4
	mulsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm5
	addsd	%xmm1, %xmm5
	mulsd	%xmm5, %xmm5
	addsd	%xmm2, %xmm4
	mulsd	%xmm4, %xmm4
	movq	rho_grid(%rip), %rdi
	addq	%rax, %rdi
	xorl	%r8d, %r8d
	jmp	.LBB3_2
	.p2align	4, 0x90
.LBB3_3:                                #   in Loop: Header=BB3_2 Depth=2
	movq	%rcx, (%rdi)
.LBB3_7:                                #   in Loop: Header=BB3_2 Depth=2
	incq	%r8
	addq	$64000, %rdi                    # imm = 0xFA00
	cmpq	$8000, %r8                      # imm = 0x1F40
	je	.LBB3_8
.LBB3_2:                                #   Parent Loop BB3_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	xorps	%xmm6, %xmm6
	cvtsi2sd	%r8d, %xmm6
	mulsd	%xmm0, %xmm6
	movapd	%xmm6, %xmm7
	addsd	%xmm2, %xmm7
	mulsd	%xmm7, %xmm7
	addsd	%xmm5, %xmm7
	ucomisd	%xmm3, %xmm7
	jb	.LBB3_3
# %bb.4:                                #   in Loop: Header=BB3_2 Depth=2
	addsd	%xmm1, %xmm6
	mulsd	%xmm6, %xmm6
	addsd	%xmm4, %xmm6
	ucomisd	%xmm3, %xmm6
	jae	.LBB3_6
# %bb.5:                                #   in Loop: Header=BB3_2 Depth=2
	movq	%rdx, (%rdi)
	jmp	.LBB3_7
	.p2align	4, 0x90
.LBB3_6:                                #   in Loop: Header=BB3_2 Depth=2
	movq	$0, (%rdi)
	jmp	.LBB3_7
.LBB3_9:
	callq	_ZNSt6chrono3_V212system_clock3nowEv@PLT
	movq	rho_grid(%rip), %rsi
	leaq	64008(%rsi), %rdi
	leaq	511935992(%rsi), %r8
	leaq	8(%rsi), %rdx
	leaq	511871992(%rsi), %rcx
	leaq	128008(%rsi), %r9
	leaq	511999992(%rsi), %r10
	leaq	64000(%rsi), %r11
	leaq	511935984(%rsi), %rbx
	leaq	64016(%rsi), %r14
	addq	$511936000, %rsi                # imm = 0x1E838600
	leaq	new_delta(%rip), %r15
	movq	%rcx, 96(%rsp)                  # 8-byte Spill
	cmpq	%r15, %rcx
	seta	%cl
	movq	%rax, 56(%rsp)                  # 8-byte Spill
	leaq	new_delta+8(%rip), %r12
	movq	%rdx, 104(%rsp)                 # 8-byte Spill
	cmpq	%r12, %rdx
	setb	%al
	andb	%cl, %al
	movq	%r10, 88(%rsp)                  # 8-byte Spill
	cmpq	%r15, %r10
	seta	%cl
	movq	%r9, 16(%rsp)                   # 8-byte Spill
	cmpq	%r12, %r9
	setb	%r9b
	andb	%cl, %r9b
	movq	%rbx, 72(%rsp)                  # 8-byte Spill
	cmpq	%r15, %rbx
	seta	%cl
	movq	%r11, 80(%rsp)                  # 8-byte Spill
	cmpq	%r12, %r11
	setb	%dl
	andb	%cl, %dl
	movq	%rsi, 128(%rsp)                 # 8-byte Spill
	cmpq	%r15, %rsi
	seta	%cl
	movq	%r14, 64(%rsp)                  # 8-byte Spill
	cmpq	%r12, %r14
	setb	%sil
	andb	%cl, %sil
	leaq	delta(%rip), %rcx
	cmpq	%rcx, %r12
	seta	%cl
	leaq	delta+8(%rip), %r10
	cmpq	%r15, %r10
	seta	%r10b
	andb	%cl, %r10b
	movb	%r10b, 4(%rsp)                  # 1-byte Spill
	orb	%al, %r9b
	movb	%r9b, 6(%rsp)                   # 1-byte Spill
	orb	%dl, %sil
	movb	%sil, 5(%rsp)                   # 1-byte Spill
	movq	%r8, 112(%rsp)                  # 8-byte Spill
	movq	%r8, %xmm0
	movdqa	%xmm0, 288(%rsp)                # 16-byte Spill
	movq	%rdi, 120(%rsp)                 # 8-byte Spill
	movq	%rdi, %xmm0
	movdqa	%xmm0, 272(%rsp)                # 16-byte Spill
	movq	%r15, %xmm0
	pshufd	$68, %xmm0, %xmm0               # xmm0 = xmm0[0,1,0,1]
	movdqa	.LCPI3_4(%rip), %xmm1           # xmm1 = [9223372039002259456,9223372039002259456]
	pxor	%xmm1, %xmm0
	movdqa	%xmm0, 256(%rsp)                # 16-byte Spill
	movq	%r12, %xmm0
	pshufd	$68, %xmm0, %xmm0               # xmm0 = xmm0[0,1,0,1]
	pxor	%xmm1, %xmm0
	movdqa	%xmm0, 240(%rsp)                # 16-byte Spill
	movapd	.LCPI3_9(%rip), %xmm5           # xmm5 = [1.9999997499375199E-1,4.9999993748437985E-2]
	movapd	.LCPI3_10(%rip), %xmm6          # xmm6 = [5.0012496091015501E-8,6.2515620113769376E-9]
	movapd	.LCPI3_5(%rip), %xmm7           # xmm7 = [4.9999993748437985E-2,4.9999993748437985E-2]
	movapd	.LCPI3_6(%rip), %xmm8           # xmm8 = [5.0012496091015501E-8,5.0012496091015501E-8]
	movapd	.LCPI3_7(%rip), %xmm9           # xmm9 = [6.2515620113769376E-9,6.2515620113769376E-9]
	movapd	.LCPI3_8(%rip), %xmm10          # xmm10 = [1.9999997499375199E-1,1.9999997499375199E-1]
	movq	Phi_grid_new(%rip), %rax
	movq	Phi_grid(%rip), %rsi
	xorl	%ecx, %ecx
	jmp	.LBB3_10
	.p2align	4, 0x90
.LBB3_18:                               #   in Loop: Header=BB3_10 Depth=1
	movq	%rsi, Phi_grid_new(%rip)
	movq	232(%rsp), %rdx                 # 8-byte Reload
	movq	%rdx, Phi_grid(%rip)
	movsd	%xmm11, delta(%rip)
	movl	28(%rsp), %ecx                  # 4-byte Reload
	incl	%ecx
	movq	%rsi, %rax
	movq	%rdx, %rsi
	cmpl	$100, %ecx
	je	.LBB3_19
.LBB3_10:                               # =>This Loop Header: Depth=1
                                        #     Child Loop BB3_11 Depth 2
                                        #       Child Loop BB3_15 Depth 3
                                        #       Child Loop BB3_13 Depth 3
	movl	%ecx, 28(%rsp)                  # 4-byte Spill
	leaq	64008(%rax), %r8
	movq	%rax, 232(%rsp)                 # 8-byte Spill
	leaq	511935992(%rax), %r13
	leaq	64000(%rsi), %r15
	movq	%r15, 144(%rsp)                 # 8-byte Spill
	leaq	511935984(%rsi), %r12
	movq	%r12, 136(%rsp)                 # 8-byte Spill
	leaq	64016(%rsi), %rdx
	movq	%rdx, 32(%rsp)                  # 8-byte Spill
	leaq	511936000(%rsi), %r9
	movq	%r9, 224(%rsp)                  # 8-byte Spill
	leaq	8(%rsi), %rdi
	movq	%rdi, 216(%rsp)                 # 8-byte Spill
	leaq	511871992(%rsi), %rbx
	movq	%rbx, 208(%rsp)                 # 8-byte Spill
	leaq	128008(%rsi), %r10
	leaq	511999992(%rsi), %r11
	movq	%r11, 200(%rsp)                 # 8-byte Spill
	leaq	511871984(%rsi), %r14
	movq	%r14, 184(%rsp)                 # 8-byte Spill
	leaq	new_delta+8(%rip), %rax
	cmpq	%rax, %r8
	setb	%al
	leaq	new_delta(%rip), %rcx
	cmpq	%rcx, %r13
	seta	%cl
	andb	%al, %cl
	movb	%cl, 15(%rsp)                   # 1-byte Spill
	cmpq	%r12, %r8
	setb	%al
	cmpq	%r13, %r15
	setb	%cl
	andb	%al, %cl
	movb	%cl, 14(%rsp)                   # 1-byte Spill
	cmpq	%r9, %r8
	setb	%al
	cmpq	%r13, %rdx
	setb	%r15b
	andb	%al, %r15b
	cmpq	%rbx, %r8
	setb	%al
	cmpq	%r13, %rdi
	setb	%cl
	andb	%al, %cl
	movb	%cl, 13(%rsp)                   # 1-byte Spill
	cmpq	%r11, %r8
	setb	%al
	cmpq	%r13, %r10
	setb	%cl
	andb	%al, %cl
	movb	%cl, 12(%rsp)                   # 1-byte Spill
	cmpq	%r14, %r8
	setb	%al
	cmpq	%r13, %rsi
	setb	%cl
	andb	%al, %cl
	movb	%cl, 11(%rsp)                   # 1-byte Spill
	leaq	511872000(%rsi), %rax
	movq	%rax, 176(%rsp)                 # 8-byte Spill
	cmpq	%rax, %r8
	setb	%al
	leaq	16(%rsi), %rcx
	movq	%rcx, 192(%rsp)                 # 8-byte Spill
	cmpq	%r13, %rcx
	setb	%bpl
	andb	%al, %bpl
	leaq	511999984(%rsi), %rax
	movq	%rax, 160(%rsp)                 # 8-byte Spill
	cmpq	%rax, %r8
	setb	%al
	leaq	128000(%rsi), %rcx
	movq	%rcx, 168(%rsp)                 # 8-byte Spill
	cmpq	%r13, %rcx
	setb	%cl
	andb	%al, %cl
	movb	%cl, 10(%rsp)                   # 1-byte Spill
	leaq	512000000(%rsi), %rax
	movq	%rax, 152(%rsp)                 # 8-byte Spill
	cmpq	%rax, %r8
	setb	%al
	leaq	128016(%rsi), %r11
	cmpq	%r13, %r11
	setb	%bl
	andb	%al, %bl
	cmpq	112(%rsp), %r8                  # 8-byte Folded Reload
	setb	%al
	cmpq	%r13, 120(%rsp)                 # 8-byte Folded Reload
	setb	%cl
	andb	%al, %cl
	movb	%cl, 9(%rsp)                    # 1-byte Spill
	cmpq	96(%rsp), %r8                   # 8-byte Folded Reload
	setb	%al
	cmpq	%r13, 104(%rsp)                 # 8-byte Folded Reload
	setb	%dil
	andb	%al, %dil
	cmpq	88(%rsp), %r8                   # 8-byte Folded Reload
	setb	%al
	cmpq	%r13, 16(%rsp)                  # 8-byte Folded Reload
	setb	%cl
	andb	%al, %cl
	movb	%cl, 8(%rsp)                    # 1-byte Spill
	cmpq	72(%rsp), %r8                   # 8-byte Folded Reload
	setb	%al
	cmpq	%r13, 80(%rsp)                  # 8-byte Folded Reload
	setb	%r9b
	andb	%al, %r9b
	cmpq	128(%rsp), %r8                  # 8-byte Folded Reload
	setb	%al
	cmpq	%r13, 64(%rsp)                  # 8-byte Folded Reload
	setb	%cl
	andb	%al, %cl
	movb	%cl, 7(%rsp)                    # 1-byte Spill
	leaq	511935992(%rsi), %r12
	cmpq	%r12, %r8
	setb	%al
	leaq	64008(%rsi), %r14
	cmpq	%r13, %r14
	setb	%cl
	andb	%al, %cl
	leaq	delta+8(%rip), %rax
	cmpq	%rax, %r8
	setb	%al
	leaq	delta(%rip), %rdx
	cmpq	%rdx, %r13
	seta	%r13b
	andb	%al, %r13b
	leaq	new_delta(%rip), %rax
	cmpq	%rax, 136(%rsp)                 # 8-byte Folded Reload
	seta	%al
	leaq	new_delta+8(%rip), %rdx
	cmpq	%rdx, 144(%rsp)                 # 8-byte Folded Reload
	setb	%dl
	andb	%al, %dl
	leaq	new_delta(%rip), %rax
	cmpq	%rax, %r12
	seta	%r12b
	leaq	new_delta+8(%rip), %rax
	cmpq	%rax, %r14
	setb	%al
	andb	%r12b, %al
	movq	208(%rsp), %xmm12               # 8-byte Folded Reload
                                        # xmm12 = mem[0],zero
	movq	224(%rsp), %xmm11               # 8-byte Folded Reload
                                        # xmm11 = mem[0],zero
	movq	184(%rsp), %xmm13               # 8-byte Folded Reload
                                        # xmm13 = mem[0],zero
	movq	200(%rsp), %xmm4                # 8-byte Folded Reload
                                        # xmm4 = mem[0],zero
	movq	160(%rsp), %xmm15               # 8-byte Folded Reload
                                        # xmm15 = mem[0],zero
	movq	176(%rsp), %xmm1                # 8-byte Folded Reload
                                        # xmm1 = mem[0],zero
	movq	152(%rsp), %xmm3                # 8-byte Folded Reload
                                        # xmm3 = mem[0],zero
	movsd	216(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	movsd	32(%rsp), %xmm14                # 8-byte Reload
                                        # xmm14 = mem[0],zero
	movq	168(%rsp), %xmm0                # 8-byte Folded Reload
                                        # xmm0 = mem[0],zero
	punpcklqdq	%xmm12, %xmm11          # xmm11 = xmm11[0],xmm12[0]
	punpcklqdq	%xmm13, %xmm4           # xmm4 = xmm4[0],xmm13[0]
	punpcklqdq	%xmm15, %xmm1           # xmm1 = xmm1[0],xmm15[0]
	punpcklqdq	288(%rsp), %xmm3        # 16-byte Folded Reload
                                        # xmm3 = xmm3[0],mem[0]
	movlhps	%xmm2, %xmm14                   # xmm14 = xmm14[0],xmm2[0]
	movaps	%xmm14, 32(%rsp)                # 16-byte Spill
	movq	%rsi, %xmm2
	movq	%r10, %xmm12
	punpcklqdq	%xmm2, %xmm12           # xmm12 = xmm12[0],xmm2[0]
	movq	192(%rsp), %xmm13               # 8-byte Folded Reload
                                        # xmm13 = mem[0],zero
	punpcklqdq	%xmm0, %xmm13           # xmm13 = xmm13[0],xmm0[0]
	movq	%r11, %xmm15
	punpcklqdq	272(%rsp), %xmm15       # 16-byte Folded Reload
                                        # xmm15 = xmm15[0],mem[0]
	movdqa	.LCPI3_4(%rip), %xmm0           # xmm0 = [9223372039002259456,9223372039002259456]
	pxor	%xmm0, %xmm3
	movdqa	%xmm3, %xmm2
	movdqa	256(%rsp), %xmm0                # 16-byte Reload
	pcmpgtd	%xmm0, %xmm2
	pshufd	$160, %xmm2, %xmm14             # xmm14 = xmm2[0,0,2,2]
	pcmpeqd	%xmm0, %xmm3
	pshufd	$245, %xmm3, %xmm3              # xmm3 = xmm3[1,1,3,3]
	pand	%xmm14, %xmm3
	pshufd	$245, %xmm2, %xmm2              # xmm2 = xmm2[1,1,3,3]
	por	%xmm3, %xmm2
	pxor	.LCPI3_4(%rip), %xmm1
	movdqa	%xmm1, %xmm3
	pcmpgtd	%xmm0, %xmm3
	pshufd	$160, %xmm3, %xmm14             # xmm14 = xmm3[0,0,2,2]
	pcmpeqd	%xmm0, %xmm1
	pshufd	$245, %xmm1, %xmm1              # xmm1 = xmm1[1,1,3,3]
	pand	%xmm14, %xmm1
	pshufd	$245, %xmm3, %xmm3              # xmm3 = xmm3[1,1,3,3]
	por	%xmm1, %xmm3
	packssdw	%xmm2, %xmm3
	movdqa	.LCPI3_4(%rip), %xmm14          # xmm14 = [9223372039002259456,9223372039002259456]
	pxor	%xmm14, %xmm4
	movdqa	%xmm4, %xmm1
	pcmpgtd	%xmm0, %xmm1
	pshufd	$160, %xmm1, %xmm2              # xmm2 = xmm1[0,0,2,2]
	pcmpeqd	%xmm0, %xmm4
	pshufd	$245, %xmm4, %xmm4              # xmm4 = xmm4[1,1,3,3]
	pand	%xmm2, %xmm4
	pshufd	$245, %xmm1, %xmm1              # xmm1 = xmm1[1,1,3,3]
	por	%xmm4, %xmm1
	pxor	%xmm14, %xmm11
	movdqa	%xmm11, %xmm14
	pcmpgtd	%xmm0, %xmm14
	pshufd	$160, %xmm14, %xmm2             # xmm2 = xmm14[0,0,2,2]
	pcmpeqd	%xmm0, %xmm11
	pshufd	$245, %xmm11, %xmm4             # xmm4 = xmm11[1,1,3,3]
	pand	%xmm2, %xmm4
	pshufd	$245, %xmm14, %xmm0             # xmm0 = xmm14[1,1,3,3]
	por	%xmm4, %xmm0
	packssdw	%xmm1, %xmm0
	packssdw	%xmm3, %xmm0
	movdqa	.LCPI3_4(%rip), %xmm4           # xmm4 = [9223372039002259456,9223372039002259456]
	pxor	%xmm4, %xmm15
	movdqa	240(%rsp), %xmm11               # 16-byte Reload
	movdqa	%xmm11, %xmm1
	pcmpgtd	%xmm15, %xmm1
	pshufd	$160, %xmm1, %xmm2              # xmm2 = xmm1[0,0,2,2]
	pcmpeqd	%xmm11, %xmm15
	pshufd	$245, %xmm15, %xmm3             # xmm3 = xmm15[1,1,3,3]
	pand	%xmm2, %xmm3
	pshufd	$245, %xmm1, %xmm1              # xmm1 = xmm1[1,1,3,3]
	por	%xmm3, %xmm1
	pxor	%xmm4, %xmm13
	movdqa	%xmm4, %xmm14
	movdqa	%xmm11, %xmm2
	pcmpgtd	%xmm13, %xmm2
	pshufd	$160, %xmm2, %xmm3              # xmm3 = xmm2[0,0,2,2]
	pcmpeqd	%xmm11, %xmm13
	pshufd	$245, %xmm13, %xmm4             # xmm4 = xmm13[1,1,3,3]
	pand	%xmm3, %xmm4
	pshufd	$245, %xmm2, %xmm2              # xmm2 = xmm2[1,1,3,3]
	por	%xmm4, %xmm2
	packssdw	%xmm1, %xmm2
	pxor	%xmm14, %xmm12
	movdqa	%xmm11, %xmm1
	pcmpgtd	%xmm12, %xmm1
	pshufd	$160, %xmm1, %xmm3              # xmm3 = xmm1[0,0,2,2]
	pcmpeqd	%xmm11, %xmm12
	pshufd	$245, %xmm12, %xmm4             # xmm4 = xmm12[1,1,3,3]
	pand	%xmm3, %xmm4
	pshufd	$245, %xmm1, %xmm1              # xmm1 = xmm1[1,1,3,3]
	por	%xmm4, %xmm1
	movdqa	32(%rsp), %xmm12                # 16-byte Reload
	pxor	%xmm14, %xmm12
	movdqa	%xmm11, %xmm3
	pcmpgtd	%xmm12, %xmm3
	pshufd	$160, %xmm3, %xmm4              # xmm4 = xmm3[0,0,2,2]
	pcmpeqd	%xmm11, %xmm12
	pshufd	$245, %xmm12, %xmm11            # xmm11 = xmm12[1,1,3,3]
	pand	%xmm4, %xmm11
	pshufd	$245, %xmm3, %xmm3              # xmm3 = xmm3[1,1,3,3]
	por	%xmm11, %xmm3
	packssdw	%xmm1, %xmm3
	packssdw	%xmm2, %xmm3
	pand	%xmm0, %xmm3
	pmovmskb	%xmm3, %r14d
	testl	$43690, %r14d                   # imm = 0xAAAA
	setne	%r14b
	orb	15(%rsp), %r15b                 # 1-byte Folded Reload
	orb	14(%rsp), %r15b                 # 1-byte Folded Reload
	movzbl	12(%rsp), %r12d                 # 1-byte Folded Reload
	orb	13(%rsp), %r12b                 # 1-byte Folded Reload
	orb	11(%rsp), %bpl                  # 1-byte Folded Reload
	orb	%r12b, %bpl
	orb	%r15b, %bpl
	orb	10(%rsp), %bl                   # 1-byte Folded Reload
	orb	9(%rsp), %dil                   # 1-byte Folded Reload
	orb	%bl, %dil
	orb	8(%rsp), %r9b                   # 1-byte Folded Reload
	orb	7(%rsp), %cl                    # 1-byte Folded Reload
	orb	%r9b, %cl
	orb	%dil, %cl
	orb	%bpl, %cl
	orb	%r13b, %dl
	orb	6(%rsp), %dl                    # 1-byte Folded Reload
	orb	4(%rsp), %al                    # 1-byte Folded Reload
	orb	5(%rsp), %al                    # 1-byte Folded Reload
	orb	%dl, %al
	orb	%cl, %al
	orb	%r14b, %al
	movl	$1, %ecx
	movq	16(%rsp), %rdx                  # 8-byte Reload
	jmp	.LBB3_11
	.p2align	4, 0x90
.LBB3_16:                               #   in Loop: Header=BB3_11 Depth=2
	movupd	-24(%r11,%r9,8), %xmm0
	maxpd	%xmm0, %xmm11
	maxpd	%xmm12, %xmm11
	unpckhpd	%xmm11, %xmm11                  # xmm11 = xmm11[1,1]
	movsd	%xmm11, new_delta(%rip)
.LBB3_17:                               #   in Loop: Header=BB3_11 Depth=2
	incq	%rcx
	addq	$64000, %r11                    # imm = 0xFA00
	addq	$64000, %r8                     # imm = 0xFA00
	addq	$64000, %rdx                    # imm = 0xFA00
	addq	$64000, %r10                    # imm = 0xFA00
	cmpq	$7999, %rcx                     # imm = 0x1F3F
	je	.LBB3_18
.LBB3_11:                               #   Parent Loop BB3_10 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB3_15 Depth 3
                                        #       Child Loop BB3_13 Depth 3
	testb	$1, %al
	je	.LBB3_14
# %bb.12:                               #   in Loop: Header=BB3_11 Depth=2
	movq	$-7998, %rdi                    # imm = 0xE0C2
	.p2align	4, 0x90
.LBB3_13:                               #   Parent Loop BB3_10 Depth=1
                                        #     Parent Loop BB3_11 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movsd	63984(%rdx,%rdi,8), %xmm0       # xmm0 = mem[0],zero
	addsd	-64016(%rdx,%rdi,8), %xmm0
	addsd	-24(%rdx,%rdi,8), %xmm0
	addsd	-8(%rdx,%rdi,8), %xmm0
	movupd	63984(%r10,%rdi,8), %xmm1
	movsd	-8(%r10,%rdi,8), %xmm2          # xmm2 = mem[0],zero
	movhpd	-64008(%r10,%rdi,8), %xmm2      # xmm2 = xmm2[0],mem[0]
	movsd	-24(%r10,%rdi,8), %xmm3         # xmm3 = mem[0],zero
	movhpd	-64024(%r10,%rdi,8), %xmm3      # xmm3 = xmm3[0],mem[0]
	addpd	%xmm2, %xmm3
	movsd	-64016(%r10,%rdi,8), %xmm2      # xmm2 = mem[0],zero
	movhpd	63976(%r10,%rdi,8), %xmm2       # xmm2 = xmm2[0],mem[0]
	addpd	%xmm1, %xmm2
	addpd	%xmm3, %xmm2
	movsd	-16(%rdx,%rdi,8), %xmm1         # xmm1 = mem[0],zero
	unpcklpd	%xmm0, %xmm1                    # xmm1 = xmm1[0],xmm0[0]
	mulpd	%xmm5, %xmm2
	mulpd	%xmm6, %xmm1
	addpd	%xmm2, %xmm1
	movapd	%xmm1, %xmm11
	unpckhpd	%xmm1, %xmm11                   # xmm11 = xmm11[1],xmm1[1]
	addsd	%xmm1, %xmm11
	movsd	%xmm11, 63984(%r8,%rdi,8)
	maxsd	-16(%r10,%rdi,8), %xmm11
	maxsd	delta(%rip), %xmm11
	movsd	%xmm11, new_delta(%rip)
	incq	%rdi
	jne	.LBB3_13
	jmp	.LBB3_17
	.p2align	4, 0x90
.LBB3_14:                               #   in Loop: Header=BB3_11 Depth=2
	movsd	delta(%rip), %xmm11             # xmm11 = mem[0],zero
	unpcklpd	%xmm11, %xmm11                  # xmm11 = xmm11[0,0]
	movq	$-7998, %rdi                    # imm = 0xE0C2
	.p2align	4, 0x90
.LBB3_15:                               #   Parent Loop BB3_10 Depth=1
                                        #     Parent Loop BB3_11 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movq	%rdi, %r9
	movupd	-32(%r11,%rdi,8), %xmm0
	movupd	-16(%r11,%rdi,8), %xmm1
	addpd	%xmm0, %xmm1
	movupd	63976(%r11,%rdi,8), %xmm0
	movupd	-64032(%r11,%rdi,8), %xmm2
	movupd	-64024(%r11,%rdi,8), %xmm12
	addpd	%xmm0, %xmm12
	addpd	%xmm1, %xmm12
	movupd	-64016(%r11,%rdi,8), %xmm0
	addpd	%xmm2, %xmm0
	movupd	63968(%r11,%rdi,8), %xmm1
	movupd	63984(%r11,%rdi,8), %xmm2
	addpd	%xmm1, %xmm2
	addpd	%xmm0, %xmm2
	mulpd	%xmm7, %xmm2
	movupd	-16(%rdx,%rdi,8), %xmm0
	mulpd	%xmm8, %xmm0
	movupd	-64016(%rdx,%rdi,8), %xmm1
	movupd	63984(%rdx,%rdi,8), %xmm3
	addpd	%xmm1, %xmm3
	movupd	-24(%rdx,%rdi,8), %xmm1
	movupd	-8(%rdx,%rdi,8), %xmm4
	addpd	%xmm1, %xmm4
	addpd	%xmm3, %xmm4
	mulpd	%xmm9, %xmm4
	addpd	%xmm0, %xmm4
	mulpd	%xmm10, %xmm12
	addpd	%xmm2, %xmm12
	addpd	%xmm4, %xmm12
	movupd	%xmm12, 63984(%r8,%rdi,8)
	addq	$2, %rdi
	jne	.LBB3_15
	jmp	.LBB3_16
.LBB3_19:
	callq	_ZNSt6chrono3_V212system_clock3nowEv@PLT
	subq	56(%rsp), %rax                  # 8-byte Folded Reload
	xorps	%xmm0, %xmm0
	cvtsi2sd	%rax, %xmm0
	mulsd	.LCPI3_11(%rip), %xmm0
	movsd	%xmm0, 16(%rsp)                 # 8-byte Spill
	movq	_ZSt4cout@GOTPCREL(%rip), %rbx
	leaq	.L.str.1(%rip), %rsi
	movl	$6, %edx
	movq	%rbx, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rbx, %rdi
	movsd	16(%rsp), %xmm0                 # 8-byte Reload
                                        # xmm0 = mem[0],zero
	callq	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	(%rax), %rcx
	movq	-24(%rcx), %rcx
	movq	240(%rax,%rcx), %r14
	testq	%r14, %r14
	je	.LBB3_28
# %bb.20:
	cmpb	$0, 56(%r14)
	je	.LBB3_22
# %bb.21:
	movzbl	67(%r14), %ecx
	jmp	.LBB3_23
.LBB3_22:
	movq	%r14, %rdi
	movq	%rax, %r15
	callq	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	(%r14), %rax
	movq	%r14, %rdi
	movl	$10, %esi
	callq	*48(%rax)
	movl	%eax, %ecx
	movq	%r15, %rax
.LBB3_23:
	movsbl	%cl, %esi
	movq	%rax, %rdi
	callq	_ZNSo3putEc@PLT
	movq	%rax, %rdi
	callq	_ZNSo5flushEv@PLT
	leaq	.L.str.2(%rip), %rsi
	movl	$54, %edx
	movq	%rbx, %rdi
	callq	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	(%rbx), %rax
	movq	-24(%rax), %rax
	movq	240(%rbx,%rax), %rbx
	testq	%rbx, %rbx
	je	.LBB3_28
# %bb.24:
	cmpb	$0, 56(%rbx)
	je	.LBB3_26
# %bb.25:
	movzbl	67(%rbx), %eax
	jmp	.LBB3_27
.LBB3_26:
	movq	%rbx, %rdi
	callq	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	(%rbx), %rax
	movq	%rbx, %rdi
	movl	$10, %esi
	callq	*48(%rax)
.LBB3_27:
	movsbl	%al, %esi
	movq	_ZSt4cout@GOTPCREL(%rip), %rdi
	callq	_ZNSo3putEc@PLT
	movq	%rax, %rdi
	callq	_ZNSo5flushEv@PLT
	xorl	%eax, %eax
	addq	$312, %rsp                      # imm = 0x138
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%r12
	.cfi_def_cfa_offset 40
	popq	%r13
	.cfi_def_cfa_offset 32
	popq	%r14
	.cfi_def_cfa_offset 24
	popq	%r15
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.LBB3_28:
	.cfi_def_cfa_offset 368
	callq	_ZSt16__throw_bad_castv@PLT
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
	.cfi_endproc
                                        # -- End function
	.type	new_delta,@object               # @new_delta
	.bss
	.globl	new_delta
	.p2align	3, 0x0
new_delta:
	.quad	0x0000000000000000              # double 0
	.size	new_delta, 8

	.type	delta,@object                   # @delta
	.data
	.globl	delta
	.p2align	3, 0x0
delta:
	.quad	0x7fefffffffffffff              # double 1.7976931348623157E+308
	.size	delta, 8

	.type	buf,@object                     # @buf
	.bss
	.globl	buf
	.p2align	4, 0x0
buf:
	.zero	1536000000
	.size	buf, 1536000000

	.type	Phi_grid_new,@object            # @Phi_grid_new
	.data
	.globl	Phi_grid_new
	.p2align	3, 0x0
Phi_grid_new:
	.quad	buf+512000000
	.size	Phi_grid_new, 8

	.type	Phi_grid,@object                # @Phi_grid
	.globl	Phi_grid
	.p2align	3, 0x0
Phi_grid:
	.quad	buf
	.size	Phi_grid, 8

	.type	rho_grid,@object                # @rho_grid
	.globl	rho_grid
	.p2align	3, 0x0
rho_grid:
	.quad	buf+1024000000
	.size	rho_grid, 8

	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"double.dat"
	.size	.L.str, 11

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"time: "
	.size	.L.str.1, 7

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"Warning: not dumping phi grid to file (grid too large)"
	.size	.L.str.2, 55

	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.p2align	3, 0x0
	.type	DW.ref.__gxx_personality_v0,@object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.ident	"Ubuntu clang version 18.1.3 (1ubuntu1)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __gxx_personality_v0
	.addrsig_sym _Unwind_Resume
	.addrsig_sym buf
	.addrsig_sym _ZSt4cout
