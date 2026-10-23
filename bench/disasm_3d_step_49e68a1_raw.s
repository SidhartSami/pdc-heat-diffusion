0000000000000640 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0>:
     640:	f3 0f 1e fa          	endbr64
     644:	41 57                	push   r15
     646:	41 56                	push   r14
     648:	41 55                	push   r13
     64a:	41 54                	push   r12
     64c:	55                   	push   rbp
     64d:	48 89 fd             	mov    rbp,rdi
     650:	53                   	push   rbx
     651:	48 81 ec c8 00 00 00 	sub    rsp,0xc8
     658:	e8 00 00 00 00       	call   65d <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x1d>
     65d:	89 c3                	mov    ebx,eax
     65f:	e8 00 00 00 00       	call   664 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x24>
     664:	89 c1                	mov    ecx,eax
     666:	8b 45 28             	mov    eax,DWORD PTR [rbp+0x28]
     669:	99                   	cdq
     66a:	f7 fb                	idiv   ebx
     66c:	39 d1                	cmp    ecx,edx
     66e:	8d 70 01             	lea    esi,[rax+0x1]
     671:	0f 4d f0             	cmovge esi,eax
     674:	b8 00 00 00 00       	mov    eax,0x0
     679:	0f 4c d0             	cmovl  edx,eax
     67c:	0f af ce             	imul   ecx,esi
     67f:	8d 04 11             	lea    eax,[rcx+rdx*1]
     682:	44 8d 0c 06          	lea    r9d,[rsi+rax*1]
     686:	44 39 c8             	cmp    eax,r9d
     689:	0f 8d e2 03 00 00    	jge    a71 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x431>
     68f:	8b 7d 2c             	mov    edi,DWORD PTR [rbp+0x2c]
     692:	85 ff                	test   edi,edi
     694:	0f 8e d7 03 00 00    	jle    a71 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x431>
     69a:	44 8b 65 30          	mov    r12d,DWORD PTR [rbp+0x30]
     69e:	44 89 64 24 04       	mov    DWORD PTR [rsp+0x4],r12d
     6a3:	45 85 e4             	test   r12d,r12d
     6a6:	0f 8e c5 03 00 00    	jle    a71 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x431>
     6ac:	4c 8b 55 20          	mov    r10,QWORD PTR [rbp+0x20]
     6b0:	4c 8b 5d 18          	mov    r11,QWORD PTR [rbp+0x18]
     6b4:	48 98                	cdqe
     6b6:	44 89 8c 24 b4 00 00 	mov    DWORD PTR [rsp+0xb4],r9d
     6bd:	00 
     6be:	48 8d 50 01          	lea    rdx,[rax+0x1]
     6c2:	f2 0f 10 7d 10       	movsd  xmm7,QWORD PTR [rbp+0x10]
     6c7:	f2 0f 10 75 08       	movsd  xmm6,QWORD PTR [rbp+0x8]
     6cc:	4c 8d 40 02          	lea    r8,[rax+0x2]
     6d0:	4d 63 72 0c          	movsxd r14,DWORD PTR [r10+0xc]
     6d4:	49 63 5b 0c          	movsxd rbx,DWORD PTR [r11+0xc]
     6d8:	41 8b 72 10          	mov    esi,DWORD PTR [r10+0x10]
     6dc:	4d 8b 7a 18          	mov    r15,QWORD PTR [r10+0x18]
     6e0:	45 8b 53 10          	mov    r10d,DWORD PTR [r11+0x10]
     6e4:	f2 0f 10 6d 00       	movsd  xmm5,QWORD PTR [rbp+0x0]
     6e9:	48 89 5c 24 60       	mov    QWORD PTR [rsp+0x60],rbx
     6ee:	41 0f af f6          	imul   esi,r14d
     6f2:	4d 89 f9             	mov    r9,r15
     6f5:	44 0f af d3          	imul   r10d,ebx
     6f9:	49 8b 5b 18          	mov    rbx,QWORD PTR [r11+0x18]
     6fd:	4d 89 f3             	mov    r11,r14
     700:	48 89 5c 24 10       	mov    QWORD PTR [rsp+0x10],rbx
     705:	4b 8d 1c 36          	lea    rbx,[r14+r14*1]
     709:	48 89 5c 24 68       	mov    QWORD PTR [rsp+0x68],rbx
     70e:	89 fb                	mov    ebx,edi
     710:	44 89 e7             	mov    edi,r12d
     713:	4d 63 d2             	movsxd r10,r10d
     716:	d1 ef                	shr    edi,1
     718:	48 89 5c 24 70       	mov    QWORD PTR [rsp+0x70],rbx
     71d:	44 89 e3             	mov    ebx,r12d
     720:	48 c1 e7 04          	shl    rdi,0x4
     724:	83 e3 fe             	and    ebx,0xfffffffe
     727:	4c 89 94 24 b8 00 00 	mov    QWORD PTR [rsp+0xb8],r10
     72e:	00 
     72f:	49 89 fd             	mov    r13,rdi
     732:	44 89 e7             	mov    edi,r12d
     735:	89 9c 24 b0 00 00 00 	mov    DWORD PTR [rsp+0xb0],ebx
     73c:	83 cf 01             	or     edi,0x1
     73f:	4d 89 ea             	mov    r10,r13
     742:	48 63 ff             	movsxd rdi,edi
     745:	48 89 7c 24 78       	mov    QWORD PTR [rsp+0x78],rdi
     74a:	8d 7b 02             	lea    edi,[rbx+0x2]
     74d:	48 63 df             	movsxd rbx,edi
     750:	48 63 fe             	movsxd rdi,esi
     753:	48 89 d6             	mov    rsi,rdx
     756:	48 89 9c 24 80 00 00 	mov    QWORD PTR [rsp+0x80],rbx
     75d:	00 
     75e:	66 90                	xchg   ax,ax
     760:	48 0f af c7          	imul   rax,rdi
     764:	48 89 fb             	mov    rbx,rdi
     767:	4c 89 84 24 90 00 00 	mov    QWORD PTR [rsp+0x90],r8
     76e:	00 
     76f:	45 31 f6             	xor    r14d,r14d
     772:	48 0f af da          	imul   rbx,rdx
     776:	48 89 bc 24 98 00 00 	mov    QWORD PTR [rsp+0x98],rdi
     77d:	00 
     77e:	48 89 94 24 a0 00 00 	mov    QWORD PTR [rsp+0xa0],rdx
     785:	00 
     786:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
     78b:	48 8b 84 24 b8 00 00 	mov    rax,QWORD PTR [rsp+0xb8]
     792:	00 
     793:	48 89 5c 24 38       	mov    QWORD PTR [rsp+0x38],rbx
     798:	4c 89 c3             	mov    rbx,r8
     79b:	48 0f af c2          	imul   rax,rdx
     79f:	48 89 b4 24 a8 00 00 	mov    QWORD PTR [rsp+0xa8],rsi
     7a6:	00 
     7a7:	48 0f af df          	imul   rbx,rdi
     7ab:	48 89 44 24 58       	mov    QWORD PTR [rsp+0x58],rax
     7b0:	8b 44 24 04          	mov    eax,DWORD PTR [rsp+0x4]
     7b4:	48 89 5c 24 48       	mov    QWORD PTR [rsp+0x48],rbx
     7b9:	83 e8 01             	sub    eax,0x1
     7bc:	48 89 84 24 88 00 00 	mov    QWORD PTR [rsp+0x88],rax
     7c3:	00 
     7c4:	90                   	nop
     7c5:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
     7cc:	00 00 00 00 
     7d0:	49 83 c6 01          	add    r14,0x1
     7d4:	4c 89 df             	mov    rdi,r11
     7d7:	48 8b 54 24 60       	mov    rdx,QWORD PTR [rsp+0x60]
     7dc:	48 8b 4c 24 38       	mov    rcx,QWORD PTR [rsp+0x38]
     7e1:	49 0f af fe          	imul   rdi,r14
     7e5:	49 0f af d6          	imul   rdx,r14
     7e9:	48 03 54 24 58       	add    rdx,QWORD PTR [rsp+0x58]
     7ee:	48 89 54 24 08       	mov    QWORD PTR [rsp+0x8],rdx
     7f3:	49 8d 04 3b          	lea    rax,[r11+rdi*1]
     7f7:	48 8d 34 39          	lea    rsi,[rcx+rdi*1]
     7fb:	48 89 c3             	mov    rbx,rax
     7fe:	48 2b 5c 24 68       	sub    rbx,QWORD PTR [rsp+0x68]
     803:	48 89 5c 24 50       	mov    QWORD PTR [rsp+0x50],rbx
     808:	83 7c 24 04 01       	cmp    DWORD PTR [rsp+0x4],0x1
     80d:	0f 84 75 02 00 00    	je     a88 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x448>
     813:	4c 8d 3c 08          	lea    r15,[rax+rcx*1]
     817:	48 8b 44 24 48       	mov    rax,QWORD PTR [rsp+0x48]
     81c:	48 01 cb             	add    rbx,rcx
     81f:	4e 8d 2c fd 10 00 00 	lea    r13,[r15*8+0x10]
     826:	00 
     827:	4c 8d 24 dd 10 00 00 	lea    r12,[rbx*8+0x10]
     82e:	00 
     82f:	4c 89 7c 24 30       	mov    QWORD PTR [rsp+0x30],r15
     834:	48 8d 0c 38          	lea    rcx,[rax+rdi*1]
     838:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
     83d:	4f 8d 3c 21          	lea    r15,[r9+r12*1]
     841:	48 89 5c 24 28       	mov    QWORD PTR [rsp+0x28],rbx
     846:	48 8d 2c cd 10 00 00 	lea    rbp,[rcx*8+0x10]
     84d:	00 
     84e:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
     853:	4b 8d 0c 29          	lea    rcx,[r9+r13*1]
     857:	4c 8d 04 38          	lea    r8,[rax+rdi*1]
     85b:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
     860:	4a 8d 1c c5 10 00 00 	lea    rbx,[r8*8+0x10]
     867:	00 
     868:	4c 89 44 24 18       	mov    QWORD PTR [rsp+0x18],r8
     86d:	4c 8d 04 f5 08 00 00 	lea    r8,[rsi*8+0x8]
     874:	00 
     875:	48 8d 44 d0 08       	lea    rax,[rax+rdx*8+0x8]
     87a:	4b 8d 14 01          	lea    rdx,[r9+r8*1]
     87e:	48 39 c1             	cmp    rcx,rax
     881:	0f 95 c1             	setne  cl
     884:	49 39 c7             	cmp    r15,rax
     887:	41 0f 95 c7          	setne  r15b
     88b:	44 21 f9             	and    ecx,r15d
     88e:	4d 8d 3c 19          	lea    r15,[r9+rbx*1]
     892:	49 39 c7             	cmp    r15,rax
     895:	41 0f 95 c7          	setne  r15b
     899:	44 21 f9             	and    ecx,r15d
     89c:	4d 8d 3c 29          	lea    r15,[r9+rbp*1]
     8a0:	49 39 c7             	cmp    r15,rax
     8a3:	41 0f 95 c7          	setne  r15b
     8a7:	44 84 f9             	test   cl,r15b
     8aa:	0f 84 d8 01 00 00    	je     a88 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x448>
     8b0:	48 89 c1             	mov    rcx,rax
     8b3:	48 29 d1             	sub    rcx,rdx
     8b6:	48 83 f9 10          	cmp    rcx,0x10
     8ba:	0f 86 c8 01 00 00    	jbe    a88 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x448>
     8c0:	66 0f 28 e7          	movapd xmm4,xmm7
     8c4:	66 0f 28 de          	movapd xmm3,xmm6
     8c8:	66 0f 28 d5          	movapd xmm2,xmm5
     8cc:	31 c9                	xor    ecx,ecx
     8ce:	66 0f 14 e4          	unpcklpd xmm4,xmm4
     8d2:	4f 8d 44 01 08       	lea    r8,[r9+r8*1+0x8]
     8d7:	49 8d 3c f1          	lea    rdi,[r9+rsi*8]
     8db:	66 0f 14 db          	unpcklpd xmm3,xmm3
     8df:	4f 8d 6c 29 f8       	lea    r13,[r9+r13*1-0x8]
     8e4:	4f 8d 64 21 f8       	lea    r12,[r9+r12*1-0x8]
     8e9:	66 0f 14 d2          	unpcklpd xmm2,xmm2
     8ed:	49 8d 6c 29 f8       	lea    rbp,[r9+rbp*1-0x8]
     8f2:	49 8d 5c 19 f8       	lea    rbx,[r9+rbx*1-0x8]
     8f7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
     8fe:	00 00 
     900:	66 44 0f 10 04 0a    	movupd xmm8,XMMWORD PTR [rdx+rcx*1]
     906:	66 41 0f 10 04 08    	movupd xmm0,XMMWORD PTR [r8+rcx*1]
     90c:	66 0f 10 0c 0f       	movupd xmm1,XMMWORD PTR [rdi+rcx*1]
     911:	66 45 0f 10 14 0c    	movupd xmm10,XMMWORD PTR [r12+rcx*1]
     917:	66 45 0f 28 c8       	movapd xmm9,xmm8
     91c:	66 44 0f 10 1c 0b    	movupd xmm11,XMMWORD PTR [rbx+rcx*1]
     922:	66 45 0f 58 c8       	addpd  xmm9,xmm8
     927:	66 41 0f 5c c1       	subpd  xmm0,xmm9
     92c:	66 0f 58 c1          	addpd  xmm0,xmm1
     930:	66 41 0f 10 4c 0d 00 	movupd xmm1,XMMWORD PTR [r13+rcx*1+0x0]
     937:	66 41 0f 5c c9       	subpd  xmm1,xmm9
     93c:	66 0f 59 c4          	mulpd  xmm0,xmm4
     940:	66 41 0f 58 ca       	addpd  xmm1,xmm10
     945:	66 0f 59 cb          	mulpd  xmm1,xmm3
     949:	66 0f 58 c1          	addpd  xmm0,xmm1
     94d:	66 0f 10 4c 0d 00    	movupd xmm1,XMMWORD PTR [rbp+rcx*1+0x0]
     953:	66 41 0f 5c c9       	subpd  xmm1,xmm9
     958:	66 41 0f 58 cb       	addpd  xmm1,xmm11
     95d:	66 0f 59 ca          	mulpd  xmm1,xmm2
     961:	66 0f 58 c1          	addpd  xmm0,xmm1
     965:	66 41 0f 58 c0       	addpd  xmm0,xmm8
     96a:	0f 11 04 08          	movups XMMWORD PTR [rax+rcx*1],xmm0
     96e:	48 83 c1 10          	add    rcx,0x10
     972:	4c 39 d1             	cmp    rcx,r10
     975:	75 89                	jne    900 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x2c0>
     977:	8b 9c 24 b0 00 00 00 	mov    ebx,DWORD PTR [rsp+0xb0]
     97e:	3b 5c 24 04          	cmp    ebx,DWORD PTR [rsp+0x4]
     982:	0f 84 a1 00 00 00    	je     a29 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x3e9>
     988:	48 8b 7c 24 78       	mov    rdi,QWORD PTR [rsp+0x78]
     98d:	48 8b 54 24 08       	mov    rdx,QWORD PTR [rsp+0x8]
     992:	48 8d 04 3e          	lea    rax,[rsi+rdi*1]
     996:	48 01 fa             	add    rdx,rdi
     999:	f2 41 0f 10 14 c1    	movsd  xmm2,QWORD PTR [r9+rax*8]
     99f:	48 8b 84 24 80 00 00 	mov    rax,QWORD PTR [rsp+0x80]
     9a6:	00 
     9a7:	66 0f 28 da          	movapd xmm3,xmm2
     9ab:	48 01 f0             	add    rax,rsi
     9ae:	f2 0f 58 da          	addsd  xmm3,xmm2
     9b2:	f2 41 0f 10 04 c1    	movsd  xmm0,QWORD PTR [r9+rax*8]
     9b8:	89 d8                	mov    eax,ebx
     9ba:	48 01 f0             	add    rax,rsi
     9bd:	f2 0f 5c c3          	subsd  xmm0,xmm3
     9c1:	f2 41 0f 58 04 c1    	addsd  xmm0,QWORD PTR [r9+rax*8]
     9c7:	48 8b 44 24 30       	mov    rax,QWORD PTR [rsp+0x30]
     9cc:	48 01 f8             	add    rax,rdi
     9cf:	f2 41 0f 10 0c c1    	movsd  xmm1,QWORD PTR [r9+rax*8]
     9d5:	48 8b 44 24 28       	mov    rax,QWORD PTR [rsp+0x28]
     9da:	f2 0f 59 c7          	mulsd  xmm0,xmm7
     9de:	48 01 f8             	add    rax,rdi
     9e1:	f2 0f 5c cb          	subsd  xmm1,xmm3
     9e5:	f2 41 0f 58 0c c1    	addsd  xmm1,QWORD PTR [r9+rax*8]
     9eb:	48 8b 44 24 20       	mov    rax,QWORD PTR [rsp+0x20]
     9f0:	48 01 f8             	add    rax,rdi
     9f3:	f2 0f 59 ce          	mulsd  xmm1,xmm6
     9f7:	f2 0f 58 c1          	addsd  xmm0,xmm1
     9fb:	f2 41 0f 10 0c c1    	movsd  xmm1,QWORD PTR [r9+rax*8]
     a01:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
     a06:	48 01 f8             	add    rax,rdi
     a09:	f2 0f 5c cb          	subsd  xmm1,xmm3
     a0d:	f2 41 0f 58 0c c1    	addsd  xmm1,QWORD PTR [r9+rax*8]
     a13:	48 8b 44 24 10       	mov    rax,QWORD PTR [rsp+0x10]
     a18:	f2 0f 59 cd          	mulsd  xmm1,xmm5
     a1c:	f2 0f 58 c1          	addsd  xmm0,xmm1
     a20:	f2 0f 58 c2          	addsd  xmm0,xmm2
     a24:	f2 0f 11 04 d0       	movsd  QWORD PTR [rax+rdx*8],xmm0
     a29:	4c 3b 74 24 70       	cmp    r14,QWORD PTR [rsp+0x70]
     a2e:	0f 85 9c fd ff ff    	jne    7d0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x190>
     a34:	4c 8b 84 24 90 00 00 	mov    r8,QWORD PTR [rsp+0x90]
     a3b:	00 
     a3c:	48 8b 94 24 a0 00 00 	mov    rdx,QWORD PTR [rsp+0xa0]
     a43:	00 
     a44:	48 8b b4 24 a8 00 00 	mov    rsi,QWORD PTR [rsp+0xa8]
     a4b:	00 
     a4c:	48 8b bc 24 98 00 00 	mov    rdi,QWORD PTR [rsp+0x98]
     a53:	00 
     a54:	48 83 c2 01          	add    rdx,0x1
     a58:	49 83 c0 01          	add    r8,0x1
     a5c:	48 89 f0             	mov    rax,rsi
     a5f:	39 b4 24 b4 00 00 00 	cmp    DWORD PTR [rsp+0xb4],esi
     a66:	7e 09                	jle    a71 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x431>
     a68:	48 83 c6 01          	add    rsi,0x1
     a6c:	e9 ef fc ff ff       	jmp    760 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x120>
     a71:	48 81 c4 c8 00 00 00 	add    rsp,0xc8
     a78:	5b                   	pop    rbx
     a79:	5d                   	pop    rbp
     a7a:	41 5c                	pop    r12
     a7c:	41 5d                	pop    r13
     a7e:	41 5e                	pop    r14
     a80:	41 5f                	pop    r15
     a82:	c3                   	ret
     a83:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
     a88:	48 8b 5c 24 08       	mov    rbx,QWORD PTR [rsp+0x8]
     a8d:	48 8b 54 24 10       	mov    rdx,QWORD PTR [rsp+0x10]
     a92:	49 8d 04 f1          	lea    rax,[r9+rsi*8]
     a96:	4c 8b 44 24 50       	mov    r8,QWORD PTR [rsp+0x50]
     a9b:	48 8b 4c 24 40       	mov    rcx,QWORD PTR [rsp+0x40]
     aa0:	48 8d 54 da 08       	lea    rdx,[rdx+rbx*8+0x8]
     aa5:	48 8b 5c 24 38       	mov    rbx,QWORD PTR [rsp+0x38]
     aaa:	48 03 b4 24 88 00 00 	add    rsi,QWORD PTR [rsp+0x88]
     ab1:	00 
     ab2:	49 29 f8             	sub    r8,rdi
     ab5:	48 8b 7c 24 48       	mov    rdi,QWORD PTR [rsp+0x48]
     aba:	49 8d 74 f1 08       	lea    rsi,[r9+rsi*8+0x8]
     abf:	48 29 d9             	sub    rcx,rbx
     ac2:	48 29 df             	sub    rdi,rbx
     ac5:	66 66 2e 0f 1f 84 00 	data16 cs nop WORD PTR [rax+rax*1+0x0]
     acc:	00 00 00 00 
     ad0:	f2 0f 10 50 08       	movsd  xmm2,QWORD PTR [rax+0x8]
     ad5:	f2 42 0f 10 44 d8 08 	movsd  xmm0,QWORD PTR [rax+r11*8+0x8]
     adc:	48 83 c2 08          	add    rdx,0x8
     ae0:	f2 0f 10 48 10       	movsd  xmm1,QWORD PTR [rax+0x10]
     ae5:	66 0f 28 da          	movapd xmm3,xmm2
     ae9:	f2 0f 58 da          	addsd  xmm3,xmm2
     aed:	f2 0f 5c c3          	subsd  xmm0,xmm3
     af1:	f2 0f 5c cb          	subsd  xmm1,xmm3
     af5:	f2 42 0f 58 44 c0 08 	addsd  xmm0,QWORD PTR [rax+r8*8+0x8]
     afc:	f2 0f 58 08          	addsd  xmm1,QWORD PTR [rax]
     b00:	f2 0f 59 c6          	mulsd  xmm0,xmm6
     b04:	f2 0f 59 cf          	mulsd  xmm1,xmm7
     b08:	f2 0f 58 c1          	addsd  xmm0,xmm1
     b0c:	f2 0f 10 4c f8 08    	movsd  xmm1,QWORD PTR [rax+rdi*8+0x8]
     b12:	f2 0f 5c cb          	subsd  xmm1,xmm3
     b16:	f2 0f 58 4c c8 08    	addsd  xmm1,QWORD PTR [rax+rcx*8+0x8]
     b1c:	48 83 c0 08          	add    rax,0x8
     b20:	f2 0f 59 cd          	mulsd  xmm1,xmm5
     b24:	f2 0f 58 c1          	addsd  xmm0,xmm1
     b28:	f2 0f 58 c2          	addsd  xmm0,xmm2
     b2c:	f2 0f 11 42 f8       	movsd  QWORD PTR [rdx-0x8],xmm0
     b31:	48 39 f0             	cmp    rax,rsi
     b34:	75 9a                	jne    ad0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x490>
     b36:	e9 ee fe ff ff       	jmp    a29 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x3e9>
     b3b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

