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
