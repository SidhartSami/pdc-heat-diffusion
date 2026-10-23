0000000000008a10 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const>:
    8a10:	f3 0f 1e fa          	endbr64
    8a14:	8b 46 04             	mov    0x4(%rsi),%eax
    8a17:	85 c0                	test   %eax,%eax
    8a19:	0f 8e c3 02 00 00    	jle    8ce2 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x2d2>
    8a1f:	41 57                	push   %r15
    8a21:	41 56                	push   %r14
    8a23:	41 55                	push   %r13
    8a25:	41 54                	push   %r12
    8a27:	55                   	push   %rbp
    8a28:	53                   	push   %rbx
    8a29:	8b 1e                	mov    (%rsi),%ebx
    8a2b:	89 5c 24 d8          	mov    %ebx,-0x28(%rsp)
    8a2f:	85 db                	test   %ebx,%ebx
    8a31:	0f 8e 27 02 00 00    	jle    8c5e <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x24e>
    8a37:	66 0f ef c9          	pxor   %xmm1,%xmm1
    8a3b:	4c 63 76 08          	movslq 0x8(%rsi),%r14
    8a3f:	48 89 44 24 c8       	mov    %rax,-0x38(%rsp)
    8a44:	41 89 db             	mov    %ebx,%r11d
    8a47:	f2 0f 2a cb          	cvtsi2sd %ebx,%xmm1
    8a4b:	48 8b 4e 10          	mov    0x10(%rsi),%rcx
    8a4f:	48 63 72 08          	movslq 0x8(%rdx),%rsi
    8a53:	41 d1 eb             	shr    $1,%r11d
    8a56:	f2 0f 10 05 aa 38 00 	movsd  0x38aa(%rip),%xmm0        # c308 <_IO_stdin_used+0x308>
    8a5d:	00 
    8a5e:	48 8b 52 10          	mov    0x10(%rdx),%rdx
    8a62:	45 31 e4             	xor    %r12d,%r12d
    8a65:	49 c1 e3 04          	shl    $0x4,%r11
    8a69:	48 89 74 24 c0       	mov    %rsi,-0x40(%rsp)
    8a6e:	89 de                	mov    %ebx,%esi
    8a70:	66 0f 28 d0          	movapd %xmm0,%xmm2
    8a74:	83 e6 fe             	and    $0xfffffffe,%esi
    8a77:	48 89 54 24 b8       	mov    %rdx,-0x48(%rsp)
    8a7c:	f2 0f 5e d1          	divsd  %xmm1,%xmm2
    8a80:	66 0f ef c9          	pxor   %xmm1,%xmm1
    8a84:	89 74 24 dc          	mov    %esi,-0x24(%rsp)
    8a88:	f2 0f 2a c8          	cvtsi2sd %eax,%xmm1
    8a8c:	4b 8d 04 36          	lea    (%r14,%r14,1),%rax
    8a90:	48 89 44 24 d0       	mov    %rax,-0x30(%rsp)
    8a95:	89 d8                	mov    %ebx,%eax
    8a97:	83 c8 01             	or     $0x1,%eax
    8a9a:	48 98                	cltq
    8a9c:	48 89 44 24 e0       	mov    %rax,-0x20(%rsp)
    8aa1:	8d 46 02             	lea    0x2(%rsi),%eax
    8aa4:	48 98                	cltq
    8aa6:	48 89 44 24 e8       	mov    %rax,-0x18(%rsp)
    8aab:	8d 43 ff             	lea    -0x1(%rbx),%eax
    8aae:	48 89 44 24 f0       	mov    %rax,-0x10(%rsp)
    8ab3:	f2 0f 5e c1          	divsd  %xmm1,%xmm0
    8ab7:	f2 0f 10 4f 10       	movsd  0x10(%rdi),%xmm1
    8abc:	f2 0f 59 4f 18       	mulsd  0x18(%rdi),%xmm1
    8ac1:	f2 0f 59 d2          	mulsd  %xmm2,%xmm2
    8ac5:	66 0f 28 da          	movapd %xmm2,%xmm3
    8ac9:	66 0f 28 d1          	movapd %xmm1,%xmm2
    8acd:	f2 0f 5e d3          	divsd  %xmm3,%xmm2
    8ad1:	f2 0f 59 c0          	mulsd  %xmm0,%xmm0
    8ad5:	f2 0f 5e c8          	divsd  %xmm0,%xmm1
    8ad9:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
    8ae0:	49 83 c4 01          	add    $0x1,%r12
    8ae4:	4c 8b 7c 24 c0       	mov    -0x40(%rsp),%r15
    8ae9:	4c 89 e3             	mov    %r12,%rbx
    8aec:	49 0f af de          	imul   %r14,%rbx
    8af0:	4d 0f af fc          	imul   %r12,%r15
    8af4:	4d 8d 2c 1e          	lea    (%r14,%rbx,1),%r13
    8af8:	4c 89 ed             	mov    %r13,%rbp
    8afb:	48 2b 6c 24 d0       	sub    -0x30(%rsp),%rbp
    8b00:	83 7c 24 d8 01       	cmpl   $0x1,-0x28(%rsp)
    8b05:	0f 84 65 01 00 00    	je     8c70 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x260>
    8b0b:	48 8b 44 24 b8       	mov    -0x48(%rsp),%rax
    8b10:	4e 8d 04 ed 10 00 00 	lea    0x10(,%r13,8),%r8
    8b17:	00 
    8b18:	48 8d 3c ed 10 00 00 	lea    0x10(,%rbp,8),%rdi
    8b1f:	00 
    8b20:	4e 8d 0c 01          	lea    (%rcx,%r8,1),%r9
    8b24:	4a 8d 54 f8 08       	lea    0x8(%rax,%r15,8),%rdx
    8b29:	48 8d 04 dd 08 00 00 	lea    0x8(,%rbx,8),%rax
    8b30:	00 
    8b31:	49 39 d1             	cmp    %rdx,%r9
    8b34:	4c 8d 0c 39          	lea    (%rcx,%rdi,1),%r9
    8b38:	48 8d 34 01          	lea    (%rcx,%rax,1),%rsi
    8b3c:	41 0f 95 c2          	setne  %r10b
    8b40:	49 39 d1             	cmp    %rdx,%r9
    8b43:	41 0f 95 c1          	setne  %r9b
    8b47:	45 84 ca             	test   %r9b,%r10b
    8b4a:	0f 84 20 01 00 00    	je     8c70 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x260>
    8b50:	49 89 d1             	mov    %rdx,%r9
    8b53:	49 29 f1             	sub    %rsi,%r9
    8b56:	49 83 f9 10          	cmp    $0x10,%r9
    8b5a:	0f 86 10 01 00 00    	jbe    8c70 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x260>
    8b60:	66 0f 28 ea          	movapd %xmm2,%xmm5
    8b64:	66 0f 28 e1          	movapd %xmm1,%xmm4
    8b68:	4c 8d 54 01 08       	lea    0x8(%rcx,%rax,1),%r10
    8b6d:	31 c0                	xor    %eax,%eax
    8b6f:	4c 8d 0c d9          	lea    (%rcx,%rbx,8),%r9
    8b73:	66 0f 14 ed          	unpcklpd %xmm5,%xmm5
    8b77:	4e 8d 44 01 f8       	lea    -0x8(%rcx,%r8,1),%r8
    8b7c:	48 8d 7c 39 f8       	lea    -0x8(%rcx,%rdi,1),%rdi
    8b81:	66 0f 14 e4          	unpcklpd %xmm4,%xmm4
    8b85:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
    8b8c:	00 00 00 00 
    8b90:	66 44 0f 10 04 06    	movupd (%rsi,%rax,1),%xmm8
    8b96:	66 41 0f 10 04 02    	movupd (%r10,%rax,1),%xmm0
    8b9c:	66 41 0f 10 34 01    	movupd (%r9,%rax,1),%xmm6
    8ba2:	66 0f 10 3c 07       	movupd (%rdi,%rax,1),%xmm7
    8ba7:	66 41 0f 28 d8       	movapd %xmm8,%xmm3
    8bac:	66 41 0f 58 d8       	addpd  %xmm8,%xmm3
    8bb1:	66 0f 5c c3          	subpd  %xmm3,%xmm0
    8bb5:	66 0f 58 c6          	addpd  %xmm6,%xmm0
    8bb9:	66 41 0f 10 34 00    	movupd (%r8,%rax,1),%xmm6
    8bbf:	66 0f 5c f3          	subpd  %xmm3,%xmm6
    8bc3:	66 0f 59 c5          	mulpd  %xmm5,%xmm0
    8bc7:	66 0f 58 fe          	addpd  %xmm6,%xmm7
    8bcb:	66 0f 59 fc          	mulpd  %xmm4,%xmm7
    8bcf:	66 0f 58 c7          	addpd  %xmm7,%xmm0
    8bd3:	66 41 0f 58 c0       	addpd  %xmm8,%xmm0
    8bd8:	0f 11 04 02          	movups %xmm0,(%rdx,%rax,1)
    8bdc:	48 83 c0 10          	add    $0x10,%rax
    8be0:	49 39 c3             	cmp    %rax,%r11
    8be3:	75 ab                	jne    8b90 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x180>
    8be5:	8b 74 24 dc          	mov    -0x24(%rsp),%esi
    8be9:	39 74 24 d8          	cmp    %esi,-0x28(%rsp)
    8bed:	74 64                	je     8c53 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x243>
    8bef:	48 8b 7c 24 e0       	mov    -0x20(%rsp),%rdi
    8bf4:	48 8d 04 3b          	lea    (%rbx,%rdi,1),%rax
    8bf8:	49 01 fd             	add    %rdi,%r13
    8bfb:	48 01 fd             	add    %rdi,%rbp
    8bfe:	49 01 ff             	add    %rdi,%r15
    8c01:	f2 0f 10 24 c1       	movsd  (%rcx,%rax,8),%xmm4
    8c06:	48 8b 44 24 e8       	mov    -0x18(%rsp),%rax
    8c0b:	f2 42 0f 10 1c e9    	movsd  (%rcx,%r13,8),%xmm3
    8c11:	66 0f 28 ec          	movapd %xmm4,%xmm5
    8c15:	48 01 d8             	add    %rbx,%rax
    8c18:	f2 0f 58 ec          	addsd  %xmm4,%xmm5
    8c1c:	f2 0f 10 04 c1       	movsd  (%rcx,%rax,8),%xmm0
    8c21:	89 f0                	mov    %esi,%eax
    8c23:	48 01 d8             	add    %rbx,%rax
    8c26:	f2 0f 5c c5          	subsd  %xmm5,%xmm0
    8c2a:	f2 0f 5c dd          	subsd  %xmm5,%xmm3
    8c2e:	f2 0f 58 04 c1       	addsd  (%rcx,%rax,8),%xmm0
    8c33:	f2 0f 58 1c e9       	addsd  (%rcx,%rbp,8),%xmm3
    8c38:	48 8b 44 24 b8       	mov    -0x48(%rsp),%rax
    8c3d:	f2 0f 59 c2          	mulsd  %xmm2,%xmm0
    8c41:	f2 0f 59 d9          	mulsd  %xmm1,%xmm3
    8c45:	f2 0f 58 c3          	addsd  %xmm3,%xmm0
    8c49:	f2 0f 58 c4          	addsd  %xmm4,%xmm0
    8c4d:	f2 42 0f 11 04 f8    	movsd  %xmm0,(%rax,%r15,8)
    8c53:	4c 3b 64 24 c8       	cmp    -0x38(%rsp),%r12
    8c58:	0f 85 82 fe ff ff    	jne    8ae0 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0xd0>
    8c5e:	5b                   	pop    %rbx
    8c5f:	5d                   	pop    %rbp
    8c60:	41 5c                	pop    %r12
    8c62:	41 5d                	pop    %r13
    8c64:	41 5e                	pop    %r14
    8c66:	41 5f                	pop    %r15
    8c68:	c3                   	ret
    8c69:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
    8c70:	48 8b 74 24 f0       	mov    -0x10(%rsp),%rsi
    8c75:	48 8b 54 24 b8       	mov    -0x48(%rsp),%rdx
    8c7a:	48 8d 04 d9          	lea    (%rcx,%rbx,8),%rax
    8c7e:	48 29 dd             	sub    %rbx,%rbp
    8c81:	48 01 de             	add    %rbx,%rsi
    8c84:	4a 8d 54 fa 08       	lea    0x8(%rdx,%r15,8),%rdx
    8c89:	48 8d 74 f1 08       	lea    0x8(%rcx,%rsi,8),%rsi
    8c8e:	66 90                	xchg   %ax,%ax
    8c90:	f2 0f 10 60 08       	movsd  0x8(%rax),%xmm4
    8c95:	f2 42 0f 10 44 f0 08 	movsd  0x8(%rax,%r14,8),%xmm0
    8c9c:	48 83 c2 08          	add    $0x8,%rdx
    8ca0:	f2 0f 10 58 10       	movsd  0x10(%rax),%xmm3
    8ca5:	66 0f 28 ec          	movapd %xmm4,%xmm5
    8ca9:	f2 0f 58 ec          	addsd  %xmm4,%xmm5
    8cad:	f2 0f 5c c5          	subsd  %xmm5,%xmm0
    8cb1:	f2 0f 5c dd          	subsd  %xmm5,%xmm3
    8cb5:	f2 0f 58 44 e8 08    	addsd  0x8(%rax,%rbp,8),%xmm0
    8cbb:	f2 0f 58 18          	addsd  (%rax),%xmm3
    8cbf:	48 83 c0 08          	add    $0x8,%rax
    8cc3:	f2 0f 59 c1          	mulsd  %xmm1,%xmm0
    8cc7:	f2 0f 59 da          	mulsd  %xmm2,%xmm3
    8ccb:	f2 0f 58 c3          	addsd  %xmm3,%xmm0
    8ccf:	f2 0f 58 c4          	addsd  %xmm4,%xmm0
    8cd3:	f2 0f 11 42 f8       	movsd  %xmm0,-0x8(%rdx)
    8cd8:	48 39 c6             	cmp    %rax,%rsi
    8cdb:	75 b3                	jne    8c90 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x280>
    8cdd:	e9 71 ff ff ff       	jmp    8c53 <heat::SerialSolver::step2D(heat::Grid2D const&, heat::Grid2D&) const+0x243>
    8ce2:	c3                   	ret
    8ce3:	90                   	nop
    8ce4:	90                   	nop
    8ce5:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
    8cec:	00 00 00 00 

