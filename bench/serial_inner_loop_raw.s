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
