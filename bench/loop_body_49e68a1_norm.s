movsd  xmm2,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x80]
9a6:	00
movapd xmm3,xmm2
add    rax,rsi
addsd  xmm3,xmm2
movsd  xmm0,QWORD PTR [r9+rax*8]
mov    eax,ebx
add    rax,rsi
subsd  xmm0,xmm3
addsd  xmm0,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x30]
add    rax,rdi
movsd  xmm1,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x28]
mulsd  xmm0,xmm7
add    rax,rdi
subsd  xmm1,xmm3
addsd  xmm1,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x20]
add    rax,rdi
mulsd  xmm1,xmm6
addsd  xmm0,xmm1
movsd  xmm1,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x18]
add    rax,rdi
subsd  xmm1,xmm3
addsd  xmm1,QWORD PTR [r9+rax*8]
mov    rax,QWORD PTR [rsp+0x10]
mulsd  xmm1,xmm5
addsd  xmm0,xmm1
addsd  xmm0,xmm2
movsd  QWORD PTR [rax+rdx*8],xmm0
cmp    r14,QWORD PTR [rsp+0x70]
jne    7d0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x190>
