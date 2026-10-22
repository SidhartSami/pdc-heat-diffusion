# External References and Literature

This document records the foundational texts, numerical analysis literature, and specifications used in the design and implementation of the parallel heat diffusion solver.

1. **Finite Difference Methods for Ordinary and Partial Differential Equations: Steady-State and Time-Dependent Problems**
   Randall J. LeVeque, SIAM (Society for Industrial and Applied Mathematics), 2007.
   - Chapter 9: Diffusion Equations and Parabolic Problems.
   - Detailed derivation of the explicit FTCS scheme and CFL stability bound $\Delta t \le \frac{\Delta x^2}{2 d \alpha}$.

2. **Using MPI: Portable Parallel Programming with the Message-Passing Interface (3rd Edition)**
   William Gropp, Eusebius Ewing Lusk, and Anthony Skjellum, MIT Press, 2014.
   - Cartesian process topologies (`MPI_Cart_create`, `MPI_Cart_shift`).
   - Non-blocking communication patterns (`MPI_Isend`, `MPI_Irecv`, `MPI_Waitall`).

3. **MPI: A Message-Passing Interface Standard, Version 3.1 & 4.0**
   Message Passing Interface Forum, 2015/2021.
   - Note on the removal of deprecated C++ bindings (Annex C).
   - Clarification of MPI Threading Levels (`MPI_THREAD_SINGLE`, `MPI_THREAD_FUNNELED`, `MPI_THREAD_SERIALIZED`, `MPI_THREAD_MULTIPLE`).

4. **Using OpenMP: The Next Step (Beyond the Basics)**
   Ruud van der Pas, Eric Stotzer, and Christian Terboven, MIT Press, 2017.
   - NUMA architectures and first-touch page placement policies.
   - Thread binding and processor affinity (`OMP_PROC_BIND`, `OMP_PLACES`).

5. **Introduction to High Performance Computing for Scientists and Engineers**
   Georg Hager and Gerhard Wellein, CRC Press, 2010.
   - Chapter 4: Stencil Computations and Cache Blocking.
   - Roofline model and arithmetic intensity of finite difference stencils.
