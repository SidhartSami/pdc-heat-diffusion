movsd  xmm2,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x60]
movapd xmm3,xmm2
add    rax,rbp
addsd  xmm3,xmm2
movsd  xmm0,QWORD PTR [r13+rax*8+0x0]
mov    eax,edx
add    rax,rbp
subsd  xmm0,xmm3
addsd  xmm0,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x28]
add    rax,rcx
movsd  xmm1,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x20]
mulsd  xmm0,xmm7
add    rax,rcx
subsd  xmm1,xmm3
addsd  xmm1,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x18]
add    rax,rcx
mulsd  xmm1,xmm6
addsd  xmm0,xmm1
movsd  xmm1,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x10]
add    rax,rcx
subsd  xmm1,xmm3
addsd  xmm1,QWORD PTR [r13+rax*8+0x0]
mov    rax,QWORD PTR [rsp+0x8]
mulsd  xmm1,xmm5
addsd  xmm0,xmm1
addsd  xmm0,xmm2
movsd  QWORD PTR [rax+rbx*8],xmm0
add    r12,0x1
cmp    DWORD PTR [rsp+0x58],r12d
jge    8c0 <_ZNK4heat9MpiSolver15step3DNoOverlapERNS_6Grid3DES2_RNS_14HaloExchange3DERKNS_15Decomposition3DENS_12BoundaryTypeEd._omp_fn.0+0x280>
