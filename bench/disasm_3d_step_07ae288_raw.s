0000000000000640 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0>:
     640:	f3 0f 1e fa          	endbr64
     644:	41 57                	push   r15
     646:	41 56                	push   r14
     648:	41 55                	push   r13
     64a:	41 54                	push   r12
     64c:	49 89 fc             	mov    r12,rdi
     64f:	55                   	push   rbp
     650:	53                   	push   rbx
     651:	48 81 ec f8 00 00 00 	sub    rsp,0xf8
     658:	48 8b 47 18          	mov    rax,QWORD PTR [rdi+0x18]
     65c:	44 8b 77 3c          	mov    r14d,DWORD PTR [rdi+0x3c]
     660:	8b 28                	mov    ebp,DWORD PTR [rax]
     662:	e8 00 00 00 00       	call   667 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x27>
     667:	41 89 c5             	mov    r13d,eax
     66a:	e8 00 00 00 00       	call   66f <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x2f>
     66f:	89 c1                	mov    ecx,eax
     671:	41 8d 44 2e ff       	lea    eax,[r14+rbp*1-0x1]
     676:	99                   	cdq
     677:	41 f7 fe             	idiv   r14d
     67a:	99                   	cdq
     67b:	41 f7 fd             	idiv   r13d
     67e:	39 d1                	cmp    ecx,edx
     680:	8d 70 01             	lea    esi,[rax+0x1]
     683:	0f 4d f0             	cmovge esi,eax
     686:	b8 00 00 00 00       	mov    eax,0x0
     68b:	0f 4c d0             	cmovl  edx,eax
     68e:	89 c8                	mov    eax,ecx
     690:	0f af c6             	imul   eax,esi
     693:	01 d0                	add    eax,edx
     695:	01 c6                	add    esi,eax
     697:	39 f0                	cmp    eax,esi
     699:	0f 8d 4c 05 00 00    	jge    beb <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5ab>
     69f:	41 0f af c6          	imul   eax,r14d
     6a3:	41 0f af f6          	imul   esi,r14d
     6a7:	8d 58 01             	lea    ebx,[rax+0x1]
     6aa:	89 c7                	mov    edi,eax
     6ac:	49 8b 44 24 20       	mov    rax,QWORD PTR [r12+0x20]
     6b1:	89 f2                	mov    edx,esi
     6b3:	89 9c 24 ac 00 00 00 	mov    DWORD PTR [rsp+0xac],ebx
     6ba:	8b 30                	mov    esi,DWORD PTR [rax]
     6bc:	41 8d 04 1e          	lea    eax,[r14+rbx*1]
     6c0:	8d 48 ff             	lea    ecx,[rax-0x1]
     6c3:	39 e9                	cmp    ecx,ebp
     6c5:	89 b4 24 c0 00 00 00 	mov    DWORD PTR [rsp+0xc0],esi
     6cc:	0f 4f cd             	cmovg  ecx,ebp
     6cf:	41 89 cb             	mov    r11d,ecx
     6d2:	85 f6                	test   esi,esi
     6d4:	0f 8e 0d 05 00 00    	jle    be7 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5a7>
     6da:	41 8b 4c 24 38       	mov    ecx,DWORD PTR [r12+0x38]
     6df:	45 8b 7c 24 40       	mov    r15d,DWORD PTR [r12+0x40]
     6e4:	49 63 f6             	movsxd rsi,r14d
     6e7:	f2 41 0f 10 7c 24 10 	movsd  xmm7,QWORD PTR [r12+0x10]
     6ee:	f2 41 0f 10 74 24 08 	movsd  xmm6,QWORD PTR [r12+0x8]
     6f5:	f2 41 0f 10 2c 24    	movsd  xmm5,QWORD PTR [r12]
     6fb:	4d 8b 6c 24 30       	mov    r13,QWORD PTR [r12+0x30]
     700:	44 89 7c 24 04       	mov    DWORD PTR [rsp+0x4],r15d
     705:	49 8b 5c 24 28       	mov    rbx,QWORD PTR [r12+0x28]
     70a:	4c 63 e7             	movsxd r12,edi
     70d:	48 63 f9             	movsxd rdi,ecx
     710:	48 89 bc 24 b8 00 00 	mov    QWORD PTR [rsp+0xb8],rdi
     717:	00 
     718:	8d 79 ff             	lea    edi,[rcx-0x1]
     71b:	44 89 f9             	mov    ecx,r15d
     71e:	d1 e9                	shr    ecx,1
     720:	89 bc 24 c4 00 00 00 	mov    DWORD PTR [rsp+0xc4],edi
     727:	44 89 ff             	mov    edi,r15d
     72a:	48 c1 e1 04          	shl    rcx,0x4
     72e:	49 89 cf             	mov    r15,rcx
     731:	89 f9                	mov    ecx,edi
     733:	83 e1 fe             	and    ecx,0xfffffffe
     736:	89 4c 24 5c          	mov    DWORD PTR [rsp+0x5c],ecx
     73a:	89 f9                	mov    ecx,edi
     73c:	83 c9 01             	or     ecx,0x1
     73f:	48 63 f9             	movsxd rdi,ecx
     742:	48 89 7c 24 68       	mov    QWORD PTR [rsp+0x68],rdi
     747:	89 ac 24 cc 00 00 00 	mov    DWORD PTR [rsp+0xcc],ebp
     74e:	41 ba 01 00 00 00    	mov    r10d,0x1
     754:	49 89 d9             	mov    r9,rbx
     757:	44 89 b4 24 c8 00 00 	mov    DWORD PTR [rsp+0xc8],r14d
     75e:	00 
     75f:	89 94 24 d0 00 00 00 	mov    DWORD PTR [rsp+0xd0],edx
     766:	89 84 24 d4 00 00 00 	mov    DWORD PTR [rsp+0xd4],eax
     76d:	48 89 b4 24 d8 00 00 	mov    QWORD PTR [rsp+0xd8],rsi
     774:	00 
     775:	4c 89 a4 24 b0 00 00 	mov    QWORD PTR [rsp+0xb0],r12
     77c:	00 
     77d:	8b 84 24 c4 00 00 00 	mov    eax,DWORD PTR [rsp+0xc4]
     784:	8b 9c 24 c0 00 00 00 	mov    ebx,DWORD PTR [rsp+0xc0]
     78b:	44 01 d0             	add    eax,r10d
     78e:	39 d8                	cmp    eax,ebx
     790:	0f 4f c3             	cmovg  eax,ebx
     793:	89 44 24 58          	mov    DWORD PTR [rsp+0x58],eax
     797:	44 39 9c 24 ac 00 00 	cmp    DWORD PTR [rsp+0xac],r11d
     79e:	00 
     79f:	0f 8f c8 03 00 00    	jg     b6d <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x52d>
     7a5:	44 39 54 24 58       	cmp    DWORD PTR [rsp+0x58],r10d
     7aa:	0f 8c bd 03 00 00    	jl     b6d <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x52d>
     7b0:	8b 44 24 04          	mov    eax,DWORD PTR [rsp+0x4]
     7b4:	85 c0                	test   eax,eax
     7b6:	0f 8e b1 03 00 00    	jle    b6d <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x52d>
     7bc:	49 63 79 0c          	movsxd rdi,DWORD PTR [r9+0xc]
     7c0:	41 8b 49 10          	mov    ecx,DWORD PTR [r9+0x10]
     7c4:	4c 89 94 24 98 00 00 	mov    QWORD PTR [rsp+0x98],r10
     7cb:	00 
     7cc:	4d 63 65 0c          	movsxd r12,DWORD PTR [r13+0xc]
     7d0:	41 8b 55 10          	mov    edx,DWORD PTR [r13+0x10]
     7d4:	4c 89 8c 24 e8 00 00 	mov    QWORD PTR [rsp+0xe8],r9
     7db:	00 
     7dc:	48 8b 9c 24 b0 00 00 	mov    rbx,QWORD PTR [rsp+0xb0]
     7e3:	00 
     7e4:	0f af cf             	imul   ecx,edi
     7e7:	4d 8b 75 18          	mov    r14,QWORD PTR [r13+0x18]
     7eb:	48 89 7c 24 50       	mov    QWORD PTR [rsp+0x50],rdi
     7f0:	49 8b 79 18          	mov    rdi,QWORD PTR [r9+0x18]
     7f4:	41 0f af d4          	imul   edx,r12d
     7f8:	44 89 9c 24 a8 00 00 	mov    DWORD PTR [rsp+0xa8],r11d
     7ff:	00 
     800:	48 8d 43 01          	lea    rax,[rbx+0x1]
     804:	48 8d 73 02          	lea    rsi,[rbx+0x2]
     808:	4c 89 ac 24 e0 00 00 	mov    QWORD PTR [rsp+0xe0],r13
     80f:	00 
     810:	4d 89 f5             	mov    r13,r14
     813:	48 89 7c 24 08       	mov    QWORD PTR [rsp+0x8],rdi
     818:	48 89 df             	mov    rdi,rbx
     81b:	8b 5c 24 5c          	mov    ebx,DWORD PTR [rsp+0x5c]
     81f:	4c 63 c1             	movsxd r8,ecx
     822:	4c 89 84 24 a0 00 00 	mov    QWORD PTR [rsp+0xa0],r8
     829:	00 
     82a:	4d 89 fe             	mov    r14,r15
     82d:	48 63 d2             	movsxd rdx,edx
     830:	4d 89 e7             	mov    r15,r12
     833:	8d 4b 02             	lea    ecx,[rbx+0x2]
     836:	48 63 d9             	movsxd rbx,ecx
     839:	48 89 c1             	mov    rcx,rax
     83c:	48 89 5c 24 60       	mov    QWORD PTR [rsp+0x60],rbx
     841:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
     845:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
     84c:	00 00 00 00 
     850:	48 89 d0             	mov    rax,rdx
     853:	48 89 7c 24 78       	mov    QWORD PTR [rsp+0x78],rdi
     858:	4c 8b a4 24 98 00 00 	mov    r12,QWORD PTR [rsp+0x98]
     85f:	00 
     860:	48 0f af c1          	imul   rax,rcx
     864:	48 89 94 24 80 00 00 	mov    QWORD PTR [rsp+0x80],rdx
     86b:	00 
     86c:	48 89 b4 24 88 00 00 	mov    QWORD PTR [rsp+0x88],rsi
     873:	00 
     874:	48 89 8c 24 90 00 00 	mov    QWORD PTR [rsp+0x90],rcx
     87b:	00 
     87c:	48 89 44 24 30       	mov    QWORD PTR [rsp+0x30],rax
     881:	48 89 d0             	mov    rax,rdx
     884:	48 0f af c6          	imul   rax,rsi
     888:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
     88d:	48 89 f8             	mov    rax,rdi
     890:	48 0f af c2          	imul   rax,rdx
     894:	48 89 44 24 38       	mov    QWORD PTR [rsp+0x38],rax
     899:	48 8b 84 24 a0 00 00 	mov    rax,QWORD PTR [rsp+0xa0]
     8a0:	00 
     8a1:	48 0f af c1          	imul   rax,rcx
     8a5:	48 89 44 24 48       	mov    QWORD PTR [rsp+0x48],rax
     8aa:	8b 44 24 04          	mov    eax,DWORD PTR [rsp+0x4]
     8ae:	83 e8 01             	sub    eax,0x1
     8b1:	48 89 44 24 70       	mov    QWORD PTR [rsp+0x70],rax
     8b6:	66 2e 0f 1f 84 00 00 	cs nop WORD PTR [rax+rax*1+0x0]
     8bd:	00 00 00 
     8c0:	4c 89 f8             	mov    rax,r15
     8c3:	48 8b 7c 24 30       	mov    rdi,QWORD PTR [rsp+0x30]
     8c8:	48 8b 5c 24 50       	mov    rbx,QWORD PTR [rsp+0x50]
     8cd:	49 0f af c4          	imul   rax,r12
     8d1:	49 0f af dc          	imul   rbx,r12
     8d5:	48 03 5c 24 48       	add    rbx,QWORD PTR [rsp+0x48]
     8da:	48 8d 2c 07          	lea    rbp,[rdi+rax*1]
     8de:	83 7c 24 04 01       	cmp    DWORD PTR [rsp+0x4],0x1
     8e3:	0f 84 17 03 00 00    	je     c00 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5c0>
     8e9:	49 8d 14 07          	lea    rdx,[r15+rax*1]
     8ed:	4c 8d 14 ed 08 00 00 	lea    r10,[rbp*8+0x8]
     8f4:	00 
     8f5:	48 8d 34 3a          	lea    rsi,[rdx+rdi*1]
     8f9:	49 8d 54 24 ff       	lea    rdx,[r12-0x1]
     8fe:	49 0f af d7          	imul   rdx,r15
     902:	4c 8d 0c f5 10 00 00 	lea    r9,[rsi*8+0x10]
     909:	00 
     90a:	48 89 74 24 28       	mov    QWORD PTR [rsp+0x28],rsi
     90f:	48 01 d7             	add    rdi,rdx
     912:	48 8b 54 24 40       	mov    rdx,QWORD PTR [rsp+0x40]
     917:	4c 8d 04 fd 10 00 00 	lea    r8,[rdi*8+0x10]
     91e:	00 
     91f:	48 89 7c 24 20       	mov    QWORD PTR [rsp+0x20],rdi
     924:	48 8d 0c 02          	lea    rcx,[rdx+rax*1]
     928:	48 8b 54 24 38       	mov    rdx,QWORD PTR [rsp+0x38]
     92d:	48 8d 3c cd 10 00 00 	lea    rdi,[rcx*8+0x10]
     934:	00 
     935:	48 89 4c 24 18       	mov    QWORD PTR [rsp+0x18],rcx
     93a:	4b 8d 4c 15 00       	lea    rcx,[r13+r10*1+0x0]
     93f:	48 01 d0             	add    rax,rdx
     942:	48 8b 54 24 08       	mov    rdx,QWORD PTR [rsp+0x8]
     947:	48 8d 34 c5 10 00 00 	lea    rsi,[rax*8+0x10]
     94e:	00 
     94f:	48 89 44 24 10       	mov    QWORD PTR [rsp+0x10],rax
     954:	49 8d 44 3d 00       	lea    rax,[r13+rdi*1+0x0]
     959:	48 8d 54 da 08       	lea    rdx,[rdx+rbx*8+0x8]
     95e:	4d 8d 5c 35 00       	lea    r11,[r13+rsi*1+0x0]
     963:	48 39 d0             	cmp    rax,rdx
     966:	0f 95 c0             	setne  al
     969:	49 39 d3             	cmp    r11,rdx
     96c:	41 0f 95 c3          	setne  r11b
     970:	44 21 d8             	and    eax,r11d
     973:	4f 8d 5c 05 00       	lea    r11,[r13+r8*1+0x0]
     978:	49 39 d3             	cmp    r11,rdx
     97b:	41 0f 95 c3          	setne  r11b
     97f:	44 21 d8             	and    eax,r11d
     982:	4f 8d 5c 0d 00       	lea    r11,[r13+r9*1+0x0]
     987:	49 39 d3             	cmp    r11,rdx
     98a:	41 0f 95 c3          	setne  r11b
     98e:	44 84 d8             	test   al,r11b
     991:	0f 84 69 02 00 00    	je     c00 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5c0>
     997:	48 89 d0             	mov    rax,rdx
     99a:	48 29 c8             	sub    rax,rcx
     99d:	48 83 f8 10          	cmp    rax,0x10
     9a1:	0f 86 59 02 00 00    	jbe    c00 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5c0>
     9a7:	66 0f 28 e7          	movapd xmm4,xmm7
     9ab:	66 0f 28 de          	movapd xmm3,xmm6
     9af:	66 0f 28 d5          	movapd xmm2,xmm5
     9b3:	31 c0                	xor    eax,eax
     9b5:	4f 8d 5c 15 08       	lea    r11,[r13+r10*1+0x8]
     9ba:	66 0f 14 e4          	unpcklpd xmm4,xmm4
     9be:	4d 8d 54 ed 00       	lea    r10,[r13+rbp*8+0x0]
     9c3:	66 0f 14 db          	unpcklpd xmm3,xmm3
     9c7:	4f 8d 4c 0d f8       	lea    r9,[r13+r9*1-0x8]
     9cc:	4f 8d 44 05 f8       	lea    r8,[r13+r8*1-0x8]
     9d1:	66 0f 14 d2          	unpcklpd xmm2,xmm2
     9d5:	49 8d 7c 3d f8       	lea    rdi,[r13+rdi*1-0x8]
     9da:	49 8d 74 35 f8       	lea    rsi,[r13+rsi*1-0x8]
     9df:	90                   	nop
     9e0:	66 44 0f 10 04 01    	movupd xmm8,XMMWORD PTR [rcx+rax*1]
     9e6:	66 41 0f 10 04 03    	movupd xmm0,XMMWORD PTR [r11+rax*1]
     9ec:	66 41 0f 10 0c 02    	movupd xmm1,XMMWORD PTR [r10+rax*1]
     9f2:	66 45 0f 10 14 00    	movupd xmm10,XMMWORD PTR [r8+rax*1]
     9f8:	66 45 0f 28 c8       	movapd xmm9,xmm8
     9fd:	66 44 0f 10 1c 06    	movupd xmm11,XMMWORD PTR [rsi+rax*1]
     a03:	66 45 0f 58 c8       	addpd  xmm9,xmm8
     a08:	66 41 0f 5c c1       	subpd  xmm0,xmm9
     a0d:	66 0f 58 c1          	addpd  xmm0,xmm1
     a11:	66 41 0f 10 0c 01    	movupd xmm1,XMMWORD PTR [r9+rax*1]
     a17:	66 41 0f 5c c9       	subpd  xmm1,xmm9
     a1c:	66 0f 59 c4          	mulpd  xmm0,xmm4
     a20:	66 41 0f 58 ca       	addpd  xmm1,xmm10
     a25:	66 0f 59 cb          	mulpd  xmm1,xmm3
     a29:	66 0f 58 c1          	addpd  xmm0,xmm1
     a2d:	66 0f 10 0c 07       	movupd xmm1,XMMWORD PTR [rdi+rax*1]
     a32:	66 41 0f 5c c9       	subpd  xmm1,xmm9
     a37:	66 41 0f 58 cb       	addpd  xmm1,xmm11
     a3c:	66 0f 59 ca          	mulpd  xmm1,xmm2
     a40:	66 0f 58 c1          	addpd  xmm0,xmm1
     a44:	66 41 0f 58 c0       	addpd  xmm0,xmm8
     a49:	0f 11 04 02          	movups XMMWORD PTR [rdx+rax*1],xmm0
     a4d:	48 83 c0 10          	add    rax,0x10
     a51:	4c 39 f0             	cmp    rax,r14
     a54:	75 8a                	jne    9e0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x3a0>
     a56:	8b 54 24 5c          	mov    edx,DWORD PTR [rsp+0x5c]
     a5a:	3b 54 24 04          	cmp    edx,DWORD PTR [rsp+0x4]
     a5e:	0f 84 a1 00 00 00    	je     b05 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x4c5>
     a64:	48 8b 4c 24 68       	mov    rcx,QWORD PTR [rsp+0x68]
     a69:	48 8d 44 0d 00       	lea    rax,[rbp+rcx*1+0x0]
     a6e:	48 01 cb             	add    rbx,rcx
     a71:	f2 41 0f 10 54 c5 00 	movsd  xmm2,QWORD PTR [r13+rax*8+0x0]
     a78:	48 8b 44 24 60       	mov    rax,QWORD PTR [rsp+0x60]
     a7d:	66 0f 28 da          	movapd xmm3,xmm2
     a81:	48 01 e8             	add    rax,rbp
     a84:	f2 0f 58 da          	addsd  xmm3,xmm2
     a88:	f2 41 0f 10 44 c5 00 	movsd  xmm0,QWORD PTR [r13+rax*8+0x0]
     a8f:	89 d0                	mov    eax,edx
     a91:	48 01 e8             	add    rax,rbp
     a94:	f2 0f 5c c3          	subsd  xmm0,xmm3
     a98:	f2 41 0f 58 44 c5 00 	addsd  xmm0,QWORD PTR [r13+rax*8+0x0]
     a9f:	48 8b 44 24 28       	mov    rax,QWORD PTR [rsp+0x28]
     aa4:	48 01 c8             	add    rax,rcx
     aa7:	f2 41 0f 10 4c c5 00 	movsd  xmm1,QWORD PTR [r13+rax*8+0x0]
     aae:	48 8b 44 24 20       	mov    rax,QWORD PTR [rsp+0x20]
     ab3:	f2 0f 59 c7          	mulsd  xmm0,xmm7
     ab7:	48 01 c8             	add    rax,rcx
     aba:	f2 0f 5c cb          	subsd  xmm1,xmm3
     abe:	f2 41 0f 58 4c c5 00 	addsd  xmm1,QWORD PTR [r13+rax*8+0x0]
     ac5:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
     aca:	48 01 c8             	add    rax,rcx
     acd:	f2 0f 59 ce          	mulsd  xmm1,xmm6
     ad1:	f2 0f 58 c1          	addsd  xmm0,xmm1
     ad5:	f2 41 0f 10 4c c5 00 	movsd  xmm1,QWORD PTR [r13+rax*8+0x0]
     adc:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
     ae1:	48 01 c8             	add    rax,rcx
     ae4:	f2 0f 5c cb          	subsd  xmm1,xmm3
     ae8:	f2 41 0f 58 4c c5 00 	addsd  xmm1,QWORD PTR [r13+rax*8+0x0]
     aef:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
     af4:	f2 0f 59 cd          	mulsd  xmm1,xmm5
     af8:	f2 0f 58 c1          	addsd  xmm0,xmm1
     afc:	f2 0f 58 c2          	addsd  xmm0,xmm2
     b00:	f2 0f 11 04 d8       	movsd  QWORD PTR [rax+rbx*8],xmm0
     b05:	49 83 c4 01          	add    r12,0x1
     b09:	44 39 64 24 58       	cmp    DWORD PTR [rsp+0x58],r12d
     b0e:	0f 8d ac fd ff ff    	jge    8c0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x280>
     b14:	48 8b 7c 24 78       	mov    rdi,QWORD PTR [rsp+0x78]
     b19:	48 8b b4 24 88 00 00 	mov    rsi,QWORD PTR [rsp+0x88]
     b20:	00 
     b21:	48 8b 8c 24 90 00 00 	mov    rcx,QWORD PTR [rsp+0x90]
     b28:	00 
     b29:	48 8b 94 24 80 00 00 	mov    rdx,QWORD PTR [rsp+0x80]
     b30:	00 
     b31:	48 83 c6 01          	add    rsi,0x1
     b35:	48 83 c7 01          	add    rdi,0x1
     b39:	48 83 c1 01          	add    rcx,0x1
     b3d:	39 8c 24 a8 00 00 00 	cmp    DWORD PTR [rsp+0xa8],ecx
     b44:	0f 8d 06 fd ff ff    	jge    850 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x210>
     b4a:	4c 8b ac 24 e0 00 00 	mov    r13,QWORD PTR [rsp+0xe0]
     b51:	00 
     b52:	4c 8b 94 24 98 00 00 	mov    r10,QWORD PTR [rsp+0x98]
     b59:	00 
     b5a:	4d 89 f7             	mov    r15,r14
     b5d:	4c 8b 8c 24 e8 00 00 	mov    r9,QWORD PTR [rsp+0xe8]
     b64:	00 
     b65:	44 8b 9c 24 a8 00 00 	mov    r11d,DWORD PTR [rsp+0xa8]
     b6c:	00 
     b6d:	4c 03 94 24 b8 00 00 	add    r10,QWORD PTR [rsp+0xb8]
     b74:	00 
     b75:	44 39 94 24 c0 00 00 	cmp    DWORD PTR [rsp+0xc0],r10d
     b7c:	00 
     b7d:	0f 8d fa fb ff ff    	jge    77d <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x13d>
     b83:	8b 94 24 d0 00 00 00 	mov    edx,DWORD PTR [rsp+0xd0]
     b8a:	8b 84 24 d4 00 00 00 	mov    eax,DWORD PTR [rsp+0xd4]
     b91:	4c 89 cb             	mov    rbx,r9
     b94:	44 8b b4 24 c8 00 00 	mov    r14d,DWORD PTR [rsp+0xc8]
     b9b:	00 
     b9c:	8b ac 24 cc 00 00 00 	mov    ebp,DWORD PTR [rsp+0xcc]
     ba3:	48 8b b4 24 d8 00 00 	mov    rsi,QWORD PTR [rsp+0xd8]
     baa:	00 
     bab:	4c 8b a4 24 b0 00 00 	mov    r12,QWORD PTR [rsp+0xb0]
     bb2:	00 
     bb3:	39 c2                	cmp    edx,eax
     bb5:	7c 34                	jl     beb <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5ab>
     bb7:	42 8d 7c 30 ff       	lea    edi,[rax+r14*1-0x1]
     bbc:	49 01 f4             	add    r12,rsi
     bbf:	89 84 24 ac 00 00 00 	mov    DWORD PTR [rsp+0xac],eax
     bc6:	39 ef                	cmp    edi,ebp
     bc8:	0f 4f fd             	cmovg  edi,ebp
     bcb:	44 01 f0             	add    eax,r14d
     bce:	41 89 fb             	mov    r11d,edi
     bd1:	e9 71 fb ff ff       	jmp    747 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x107>
     bd6:	44 01 f0             	add    eax,r14d
     bd9:	8d 48 ff             	lea    ecx,[rax-0x1]
     bdc:	39 cd                	cmp    ebp,ecx
     bde:	7c 07                	jl     be7 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5a7>
     be0:	39 c2                	cmp    edx,eax
     be2:	7c 07                	jl     beb <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x5ab>
     be4:	44 01 f0             	add    eax,r14d
     be7:	39 c2                	cmp    edx,eax
     be9:	7d eb                	jge    bd6 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x596>
     beb:	48 81 c4 f8 00 00 00 	add    rsp,0xf8
     bf2:	5b                   	pop    rbx
     bf3:	5d                   	pop    rbp
     bf4:	41 5c                	pop    r12
     bf6:	41 5d                	pop    r13
     bf8:	41 5e                	pop    r14
     bfa:	41 5f                	pop    r15
     bfc:	c3                   	ret
     bfd:	0f 1f 00             	nop    DWORD PTR [rax]
     c00:	48 8b 74 24 08       	mov    rsi,QWORD PTR [rsp+0x8]
     c05:	48 8b 4c 24 38       	mov    rcx,QWORD PTR [rsp+0x38]
     c0a:	49 8d 44 ed 00       	lea    rax,[r13+rbp*8+0x0]
     c0f:	4c 89 ff             	mov    rdi,r15
     c12:	48 03 6c 24 70       	add    rbp,QWORD PTR [rsp+0x70]
     c17:	48 f7 df             	neg    rdi
     c1a:	48 8d 54 de 08       	lea    rdx,[rsi+rbx*8+0x8]
     c1f:	48 8b 5c 24 30       	mov    rbx,QWORD PTR [rsp+0x30]
     c24:	48 8b 74 24 40       	mov    rsi,QWORD PTR [rsp+0x40]
     c29:	4d 8d 44 ed 08       	lea    r8,[r13+rbp*8+0x8]
     c2e:	48 29 de             	sub    rsi,rbx
     c31:	48 29 d9             	sub    rcx,rbx
     c34:	90                   	nop
     c35:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
     c3c:	00 00 00 00 
     c40:	f2 0f 10 50 08       	movsd  xmm2,QWORD PTR [rax+0x8]
     c45:	f2 42 0f 10 44 f8 08 	movsd  xmm0,QWORD PTR [rax+r15*8+0x8]
     c4c:	48 83 c2 08          	add    rdx,0x8
     c50:	f2 0f 10 48 10       	movsd  xmm1,QWORD PTR [rax+0x10]
     c55:	66 0f 28 da          	movapd xmm3,xmm2
     c59:	f2 0f 58 da          	addsd  xmm3,xmm2
     c5d:	f2 0f 5c c3          	subsd  xmm0,xmm3
     c61:	f2 0f 5c cb          	subsd  xmm1,xmm3
     c65:	f2 0f 58 44 f8 08    	addsd  xmm0,QWORD PTR [rax+rdi*8+0x8]
     c6b:	f2 0f 58 08          	addsd  xmm1,QWORD PTR [rax]
     c6f:	f2 0f 59 c6          	mulsd  xmm0,xmm6
     c73:	f2 0f 59 cf          	mulsd  xmm1,xmm7
     c77:	f2 0f 58 c1          	addsd  xmm0,xmm1
     c7b:	f2 0f 10 4c f0 08    	movsd  xmm1,QWORD PTR [rax+rsi*8+0x8]
     c81:	f2 0f 5c cb          	subsd  xmm1,xmm3
     c85:	f2 0f 58 4c c8 08    	addsd  xmm1,QWORD PTR [rax+rcx*8+0x8]
     c8b:	48 83 c0 08          	add    rax,0x8
     c8f:	f2 0f 59 cd          	mulsd  xmm1,xmm5
     c93:	f2 0f 58 c1          	addsd  xmm0,xmm1
     c97:	f2 0f 58 c2          	addsd  xmm0,xmm2
     c9b:	f2 0f 11 42 f8       	movsd  QWORD PTR [rdx-0x8],xmm0
     ca0:	49 39 c0             	cmp    r8,rax
     ca3:	75 9b                	jne    c40 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x600>
     ca5:	e9 5b fe ff ff       	jmp    b05 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x4c5>
     caa:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]

