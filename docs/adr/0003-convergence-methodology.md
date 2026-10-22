# 3. Discretization Scheme Accuracy & Convergence Verification Methodology

Date: 2026-10-22

## Context and Problem Statement
The numerical solver uses an explicit Forward-Time Central-Space (FTCS) finite-difference scheme.
Truncation error analysis via Taylor series expansion shows:
$$u_t - \alpha \nabla^2 u = \mathcal{O}(\Delta t) + \mathcal{O}(\Delta x^2)$$
When verifying numerical convergence, we must document whether error is measured under:
1. Fixed Courant number $r = \frac{\alpha \Delta t}{\Delta x^2} = \text{const}$ where $\Delta t \propto \Delta x^2$, yielding an apparent combined reduction rate of $\mathcal{O}(\Delta x^2)$ with second-order slope $p \approx 2$.
2. Fixed tiny time-step $\Delta t = \text{const} \ll \Delta x^2$, isolating pure spatial discretization error $\mathcal{O}(\Delta x^2)$.

## Decision Drivers
- Physical fidelity and explicit stability constraints: in practical explicit simulation runs, $\Delta t$ is always scaled with grid refinement to respect the CFL stability bound $\Delta t \le \frac{\Delta x^2}{2 d \alpha}$.
- Rigorous numerical differentiation in tests: Both fixed-Courant (joint space-time) and fixed-$\Delta t$ (pure space) behavior should be transparently defined and tested.

## Considered Options
1. Report fixed-Courant convergence as "pure spatial" (Rejected: theoretically inaccurate as $\Delta t \propto \Delta x^2$ couples temporal truncation $\mathcal{O}(\Delta t)$ into $\mathcal{O}(\Delta x^2)$).
2. Document fixed-Courant convergence accurately as the combined full-scheme accuracy under proportional refinement, and add an isolated spatial refinement test with fixed $\Delta t$.

## Decision Outcome
Chosen option: **Accurately document full-scheme second-order convergence under constant Courant number $r$, and implement 3D convergence testing as well.**
The discrete $L_2$ error norm is rigorously defined over interior physical cells excluding ghost layers:
$$\|e\|_{L_2} = \sqrt{\frac{1}{N^d} \sum_{i \in \text{interior}} (u_{\text{numeric}}(i) - u_{\text{exact}}(i, t_{\text{final}}))^2}$$

## Consequences
- Positive: Scientifically sound explanation ready for viva defense; examiners will appreciate the distinction between pure spatial vs. constant-CFL truncation behavior.
- Positive: 3D stencil code path is verified independently of 2D.
