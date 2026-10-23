000000000000a740 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]>:
    a740:	f3 0f 1e fa          	endbr64
    a744:	41 57                	push   %r15
    a746:	41 56                	push   %r14
    a748:	41 55                	push   %r13
    a74a:	41 54                	push   %r12
    a74c:	55                   	push   %rbp
    a74d:	53                   	push   %rbx
    a74e:	48 89 fb             	mov    %rdi,%rbx
    a751:	48 83 ec 58          	sub    $0x58,%rsp
    a755:	f2 0f 10 4f 20       	movsd  0x20(%rdi),%xmm1
    a75a:	f2 0f 10 57 18       	movsd  0x18(%rdi),%xmm2
    a75f:	8b 47 28             	mov    0x28(%rdi),%eax
    a762:	8b 6f 2c             	mov    0x2c(%rdi),%ebp
    a765:	f2 0f 11 4c 24 08    	movsd  %xmm1,0x8(%rsp)
    a76b:	4c 8b 27             	mov    (%rdi),%r12
    a76e:	f2 0f 11 14 24       	movsd  %xmm2,(%rsp)
    a773:	89 44 24 20          	mov    %eax,0x20(%rsp)
    a777:	e8 e4 8e ff ff       	call   3660 <GOMP_single_start@plt>
    a77c:	f2 0f 10 14 24       	movsd  (%rsp),%xmm2
    a781:	f2 0f 10 4c 24 08    	movsd  0x8(%rsp),%xmm1
    a787:	84 c0                	test   %al,%al
    a789:	74 23                	je     a7ae <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x6e>
    a78b:	f2 0f 11 54 24 08    	movsd  %xmm2,0x8(%rsp)
    a791:	f2 0f 11 0c 24       	movsd  %xmm1,(%rsp)
    a796:	e8 d5 93 ff ff       	call   3b70 <omp_get_num_threads@plt>
    a79b:	f2 0f 10 0c 24       	movsd  (%rsp),%xmm1
    a7a0:	f2 0f 10 54 24 08    	movsd  0x8(%rsp),%xmm2
    a7a6:	41 89 84 24 98 00 00 	mov    %eax,0x98(%r12)
    a7ad:	00 
    a7ae:	f2 0f 11 54 24 40    	movsd  %xmm2,0x40(%rsp)
    a7b4:	f2 0f 11 4c 24 38    	movsd  %xmm1,0x38(%rsp)
    a7ba:	e8 71 93 ff ff       	call   3b30 <GOMP_barrier@plt>
    a7bf:	e8 ac 93 ff ff       	call   3b70 <omp_get_num_threads@plt>
    a7c4:	41 89 c4             	mov    %eax,%r12d
    a7c7:	e8 94 92 ff ff       	call   3a60 <omp_get_thread_num@plt>
    a7cc:	41 89 c3             	mov    %eax,%r11d
    a7cf:	89 e8                	mov    %ebp,%eax
    a7d1:	99                   	cltd
    a7d2:	41 f7 fc             	idiv   %r12d
    a7d5:	41 39 d3             	cmp    %edx,%r11d
    a7d8:	8d 48 01             	lea    0x1(%rax),%ecx
    a7db:	0f 4c c1             	cmovl  %ecx,%eax
    a7de:	b9 00 00 00 00       	mov    $0x0,%ecx
    a7e3:	0f 4c d1             	cmovl  %ecx,%edx
    a7e6:	44 0f af d8          	imul   %eax,%r11d
    a7ea:	41 01 d3             	add    %edx,%r11d
    a7ed:	44 01 d8             	add    %r11d,%eax
    a7f0:	89 44 24 08          	mov    %eax,0x8(%rsp)
    a7f4:	41 39 c3             	cmp    %eax,%r11d
    a7f7:	0f 8d f2 01 00 00    	jge    a9ef <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2af>
    a7fd:	8b 74 24 20          	mov    0x20(%rsp),%esi
    a801:	85 f6                	test   %esi,%esi
    a803:	0f 8e e6 01 00 00    	jle    a9ef <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2af>
    a809:	48 8b 53 08          	mov    0x8(%rbx),%rdx
    a80d:	48 8b 43 10          	mov    0x10(%rbx),%rax
    a811:	4d 63 db             	movslq %r11d,%r11
    a814:	f2 0f 10 54 24 40    	movsd  0x40(%rsp),%xmm2
    a81a:	f2 0f 10 4c 24 38    	movsd  0x38(%rsp),%xmm1
    a820:	48 63 58 08          	movslq 0x8(%rax),%rbx
    a824:	4c 63 72 08          	movslq 0x8(%rdx),%r14
    a828:	48 8b 40 10          	mov    0x10(%rax),%rax
    a82c:	48 8b 4a 10          	mov    0x10(%rdx),%rcx
    a830:	89 f2                	mov    %esi,%edx
    a832:	48 89 5c 24 10       	mov    %rbx,0x10(%rsp)
    a837:	89 f3                	mov    %esi,%ebx
    a839:	83 e2 fe             	and    $0xfffffffe,%edx
    a83c:	48 89 04 24          	mov    %rax,(%rsp)
    a840:	4b 8d 04 36          	lea    (%r14,%r14,1),%rax
    a844:	d1 eb                	shr    $1,%ebx
    a846:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
    a84b:	8d 46 ff             	lea    -0x1(%rsi),%eax
    a84e:	83 ce 01             	or     $0x1,%esi
    a851:	48 c1 e3 04          	shl    $0x4,%rbx
    a855:	48 89 44 24 48       	mov    %rax,0x48(%rsp)
    a85a:	48 63 c6             	movslq %esi,%rax
    a85d:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    a862:	8d 42 02             	lea    0x2(%rdx),%eax
    a865:	48 98                	cltq
    a867:	89 54 24 24          	mov    %edx,0x24(%rsp)
    a86b:	48 89 44 24 30       	mov    %rax,0x30(%rsp)
    a870:	49 83 c3 01          	add    $0x1,%r11
    a874:	4c 8b 7c 24 10       	mov    0x10(%rsp),%r15
    a879:	4c 89 dd             	mov    %r11,%rbp
    a87c:	49 0f af ee          	imul   %r14,%rbp
    a880:	4d 0f af fb          	imul   %r11,%r15
    a884:	4d 8d 2c 2e          	lea    (%r14,%rbp,1),%r13
    a888:	4d 89 ec             	mov    %r13,%r12
    a88b:	4c 2b 64 24 18       	sub    0x18(%rsp),%r12
    a890:	83 7c 24 20 01       	cmpl   $0x1,0x20(%rsp)
    a895:	0f 84 65 01 00 00    	je     aa00 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2c0>
    a89b:	48 8b 04 24          	mov    (%rsp),%rax
    a89f:	4a 8d 3c ed 10 00 00 	lea    0x10(,%r13,8),%rdi
    a8a6:	00 
    a8a7:	4c 8d 04 ed 08 00 00 	lea    0x8(,%rbp,8),%r8
    a8ae:	00 
    a8af:	4c 8d 0c 39          	lea    (%rcx,%rdi,1),%r9
    a8b3:	4a 8d 34 01          	lea    (%rcx,%r8,1),%rsi
    a8b7:	4a 8d 54 f8 08       	lea    0x8(%rax,%r15,8),%rdx
    a8bc:	4a 8d 04 e5 10 00 00 	lea    0x10(,%r12,8),%rax
    a8c3:	00 
    a8c4:	49 39 d1             	cmp    %rdx,%r9
    a8c7:	4c 8d 0c 01          	lea    (%rcx,%rax,1),%r9
    a8cb:	41 0f 95 c2          	setne  %r10b
    a8cf:	49 39 d1             	cmp    %rdx,%r9
    a8d2:	41 0f 95 c1          	setne  %r9b
    a8d6:	45 84 ca             	test   %r9b,%r10b
    a8d9:	0f 84 21 01 00 00    	je     aa00 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2c0>
    a8df:	49 89 d1             	mov    %rdx,%r9
    a8e2:	49 29 f1             	sub    %rsi,%r9
    a8e5:	49 83 f9 10          	cmp    $0x10,%r9
    a8e9:	0f 86 11 01 00 00    	jbe    aa00 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2c0>
    a8ef:	66 0f 28 ea          	movapd %xmm2,%xmm5
    a8f3:	66 0f 28 e1          	movapd %xmm1,%xmm4
    a8f7:	4e 8d 54 01 08       	lea    0x8(%rcx,%r8,1),%r10
    a8fc:	66 0f 14 ed          	unpcklpd %xmm5,%xmm5
    a900:	4c 8d 44 39 f8       	lea    -0x8(%rcx,%rdi,1),%r8
    a905:	4c 8d 0c e9          	lea    (%rcx,%rbp,8),%r9
    a909:	48 8d 7c 01 f8       	lea    -0x8(%rcx,%rax,1),%rdi
    a90e:	66 0f 14 e4          	unpcklpd %xmm4,%xmm4
    a912:	31 c0                	xor    %eax,%eax
    a914:	90                   	nop
    a915:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
    a91c:	00 00 00 00 
    a920:	66 44 0f 10 04 06    	movupd (%rsi,%rax,1),%xmm8
    a926:	66 41 0f 10 04 02    	movupd (%r10,%rax,1),%xmm0
    a92c:	66 41 0f 10 34 01    	movupd (%r9,%rax,1),%xmm6
    a932:	66 0f 10 3c 07       	movupd (%rdi,%rax,1),%xmm7
    a937:	66 41 0f 28 d8       	movapd %xmm8,%xmm3
    a93c:	66 41 0f 58 d8       	addpd  %xmm8,%xmm3
    a941:	66 0f 5c c3          	subpd  %xmm3,%xmm0
    a945:	66 0f 58 c6          	addpd  %xmm6,%xmm0
    a949:	66 41 0f 10 34 00    	movupd (%r8,%rax,1),%xmm6
    a94f:	66 0f 5c f3          	subpd  %xmm3,%xmm6
    a953:	66 0f 59 c5          	mulpd  %xmm5,%xmm0
    a957:	66 0f 58 fe          	addpd  %xmm6,%xmm7
    a95b:	66 0f 59 fc          	mulpd  %xmm4,%xmm7
    a95f:	66 0f 58 c7          	addpd  %xmm7,%xmm0
    a963:	66 41 0f 58 c0       	addpd  %xmm8,%xmm0
    a968:	0f 11 04 02          	movups %xmm0,(%rdx,%rax,1)
    a96c:	48 83 c0 10          	add    $0x10,%rax
    a970:	48 39 c3             	cmp    %rax,%rbx
    a973:	75 ab                	jne    a920 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x1e0>
    a975:	8b 74 24 24          	mov    0x24(%rsp),%esi
    a979:	39 74 24 20          	cmp    %esi,0x20(%rsp)
    a97d:	74 65                	je     a9e4 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2a4>
    a97f:	48 8b 7c 24 28       	mov    0x28(%rsp),%rdi
    a984:	48 8d 44 3d 00       	lea    0x0(%rbp,%rdi,1),%rax
    a989:	49 01 fd             	add    %rdi,%r13
    a98c:	49 01 fc             	add    %rdi,%r12
    a98f:	49 01 ff             	add    %rdi,%r15
    a992:	f2 0f 10 24 c1       	movsd  (%rcx,%rax,8),%xmm4
    a997:	48 8b 44 24 30       	mov    0x30(%rsp),%rax
    a99c:	f2 42 0f 10 1c e9    	movsd  (%rcx,%r13,8),%xmm3
    a9a2:	66 0f 28 ec          	movapd %xmm4,%xmm5
    a9a6:	48 01 e8             	add    %rbp,%rax
    a9a9:	f2 0f 58 ec          	addsd  %xmm4,%xmm5
    a9ad:	f2 0f 10 04 c1       	movsd  (%rcx,%rax,8),%xmm0
    a9b2:	89 f0                	mov    %esi,%eax
    a9b4:	48 01 e8             	add    %rbp,%rax
    a9b7:	f2 0f 5c c5          	subsd  %xmm5,%xmm0
    a9bb:	f2 0f 5c dd          	subsd  %xmm5,%xmm3
    a9bf:	f2 0f 58 04 c1       	addsd  (%rcx,%rax,8),%xmm0
    a9c4:	f2 42 0f 58 1c e1    	addsd  (%rcx,%r12,8),%xmm3
    a9ca:	48 8b 04 24          	mov    (%rsp),%rax
    a9ce:	f2 0f 59 c2          	mulsd  %xmm2,%xmm0
    a9d2:	f2 0f 59 d9          	mulsd  %xmm1,%xmm3
    a9d6:	f2 0f 58 c3          	addsd  %xmm3,%xmm0
    a9da:	f2 0f 58 c4          	addsd  %xmm4,%xmm0
    a9de:	f2 42 0f 11 04 f8    	movsd  %xmm0,(%rax,%r15,8)
    a9e4:	44 39 5c 24 08       	cmp    %r11d,0x8(%rsp)
    a9e9:	0f 8f 81 fe ff ff    	jg     a870 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x130>
    a9ef:	48 83 c4 58          	add    $0x58,%rsp
    a9f3:	5b                   	pop    %rbx
    a9f4:	5d                   	pop    %rbp
    a9f5:	41 5c                	pop    %r12
    a9f7:	41 5d                	pop    %r13
    a9f9:	41 5e                	pop    %r14
    a9fb:	41 5f                	pop    %r15
    a9fd:	c3                   	ret
    a9fe:	66 90                	xchg   %ax,%ax
    aa00:	48 8b 74 24 48       	mov    0x48(%rsp),%rsi
    aa05:	48 8b 14 24          	mov    (%rsp),%rdx
    aa09:	48 8d 04 e9          	lea    (%rcx,%rbp,8),%rax
    aa0d:	49 29 ec             	sub    %rbp,%r12
    aa10:	48 01 ee             	add    %rbp,%rsi
    aa13:	4a 8d 54 fa 08       	lea    0x8(%rdx,%r15,8),%rdx
    aa18:	48 8d 74 f1 08       	lea    0x8(%rcx,%rsi,8),%rsi
    aa1d:	0f 1f 00             	nopl   (%rax)
    aa20:	f2 0f 10 60 08       	movsd  0x8(%rax),%xmm4
    aa25:	f2 42 0f 10 44 f0 08 	movsd  0x8(%rax,%r14,8),%xmm0
    aa2c:	48 83 c2 08          	add    $0x8,%rdx
    aa30:	f2 0f 10 58 10       	movsd  0x10(%rax),%xmm3
    aa35:	66 0f 28 ec          	movapd %xmm4,%xmm5
    aa39:	f2 0f 58 ec          	addsd  %xmm4,%xmm5
    aa3d:	f2 0f 5c c5          	subsd  %xmm5,%xmm0
    aa41:	f2 0f 5c dd          	subsd  %xmm5,%xmm3
    aa45:	f2 42 0f 58 44 e0 08 	addsd  0x8(%rax,%r12,8),%xmm0
    aa4c:	f2 0f 58 18          	addsd  (%rax),%xmm3
    aa50:	48 83 c0 08          	add    $0x8,%rax
    aa54:	f2 0f 59 c1          	mulsd  %xmm1,%xmm0
    aa58:	f2 0f 59 da          	mulsd  %xmm2,%xmm3
    aa5c:	f2 0f 58 c3          	addsd  %xmm3,%xmm0
    aa60:	f2 0f 58 c4          	addsd  %xmm4,%xmm0
    aa64:	f2 0f 11 42 f8       	movsd  %xmm0,-0x8(%rdx)
    aa69:	48 39 c6             	cmp    %rax,%rsi
    aa6c:	75 b2                	jne    aa20 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2e0>
    aa6e:	e9 71 ff ff ff       	jmp    a9e4 <heat::MpiSolver::step2DNoOverlap(heat::Grid2D&, heat::Grid2D&, heat::HaloExchange2D&, heat::Decomposition2D const&, heat::BoundaryType, double) const [clone ._omp_fn.0]+0x2a4>
    aa73:	66 90                	xchg   %ax,%ax
    aa75:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1)
    aa7c:	00 00 00 00 

