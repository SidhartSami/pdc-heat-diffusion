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
