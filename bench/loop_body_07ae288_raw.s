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
