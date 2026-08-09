# Six-Degree-of-Freedom Dynamics

## 1. Nonlinear marine-craft model

The recommended continuous-time model is

\[
\dot\eta=J(\eta)\nu,
\]

\[
M\dot\nu+C(\nu)\nu+D(\nu)\nu+g(\eta)=\tau_{thr}+\tau_{env}+\tau_{tether}.
\]

Here \(M=M_{RB}+M_A\) combines rigid-body and added-mass inertia, \(C=C_{RB}+C_A\) contains Coriolis and centripetal terms, \(D\) is hydrodynamic damping, and \(g\) is the hydrostatic restoring vector. Applied wrench is separated by physical source for logging and validation.

## 2. Rigid-body inertia

Let mass be \(m\), inertia about the body origin be \(I_O\), and center-of-gravity vector from body origin be \(r_g\). One useful form is

\[
M_{RB}=\begin{bmatrix}
mI_3 & -mS(r_g)\\
mS(r_g) & I_O
\end{bmatrix}.
\]

If inertia is measured about the center of gravity, use the parallel-axis theorem consistently:

\[
I_O=I_G-mS(r_g)S(r_g).
\]

The matrix must be symmetric and physically positive definite. Units are kg for translational diagonal terms, kg m for coupling, and kg m² for rotational terms.

## 3. Rigid-body Coriolis structure

For a model whose body origin is at the center of gravity, a common compact structure is

\[
C_{RB}(\nu)=\begin{bmatrix}
0_{3\times3}&-mS(\nu_1)\\
-mS(\nu_1)&-S(I_G\nu_2)
\end{bmatrix}.
\]

For an offset origin, derive \(C_{RB}\) from the exact selected inertia convention. Do not combine an offset-origin mass matrix with a center-of-gravity Coriolis matrix.

## 4. Added mass

Accelerating surrounding water creates apparent inertia. In the simplest diagonal approximation,

\[
M_A=-\operatorname{diag}(X_{\dot u},Y_{\dot v},Z_{\dot w},K_{\dot p},M_{\dot q},N_{\dot r}),
\]

where traditional hydrodynamic derivatives are often negative, making the implemented added-mass entries positive. State the sign convention with stored parameters. Coupling terms may be needed when geometry lacks symmetry.

A general constant added-mass matrix can be partitioned as

\[
M_A=\begin{bmatrix}A&B\\B^T&D\end{bmatrix}.
\]

The corresponding Coriolis matrix should be generated in a structure-preserving way so that \(\nu^T C_A(\nu)\nu=0\), subject to the adopted formulation.

## 5. Relative velocity in current

Hydrodynamic forces generally depend on velocity relative to water:

\[
\nu_r=\nu-\nu_c,
\]

where inertial current velocity must be transformed into BODY. A simplified current-aware model is

\[
M\dot\nu+C(\nu)\nu+D(\nu_r)\nu_r+g(\eta)=\tau+\tau_{other}.
\]

For advanced work, retain the full relative-motion equations and current acceleration terms. Clearly label any simplification and its validation range.

## 6. State-space derivative

For simulation,

\[
\dot\nu=M^{-1}\left[\tau_{thr}+\tau_{env}+\tau_{tether}-C(\nu)\nu-D(\nu_r)\nu_r-g(\eta)\right].
\]

Numerically solve the linear system rather than explicitly forming \(M^{-1}\). Integrate pose and velocity with a solver appropriate for the stiffness and sample rates. Bound unrealistic states and fail loudly on NaN, non-finite matrices, invalid quaternion norm, or non-positive mass properties.

## 7. Reduced-order models

### Heave model

Near level attitude and low horizontal speed:

\[
(m-Z_{\dot w})\dot w+d_w w+d_{w|w|}|w|w+(W-B)=Z_T+Z_d.
\]

With depth \(z\) positive down, \(\dot z\approx w\). This is the preferred first closed-loop model.

### Yaw model

\[
(I_z-N_{\dot r})\dot r+d_r r+d_{r|r|}|r|r=N_T+N_d,
\qquad \dot\psi\approx r.
\]

### Surge model

\[
(m-X_{\dot u})\dot u+d_u u+d_{u|u|}|u|u=X_T+X_d.
\]

Reduced models must remain traceable to the 6-DOF parameter set and identify neglected coupling.

## 8. Energy and structural checks

With no external input and no damping, the kinetic energy is

\[
T=\frac{1}{2}\nu^T M\nu.
\]

Recommended automated checks:

- \(M=M^T\) within tolerance.
- All eigenvalues of \(M\) are positive.
- \(\nu^TC(\nu)\nu\approx0\) over randomized vectors.
- Damping dissipates energy: \(\nu_r^T D(\nu_r)\nu_r\ge0\).
- Neutral equilibrium gives expected hydrostatic wrench.
- Zero input and zero disturbance do not create unexplained energy.

## 9. Parameter evidence

Each parameter shall carry value, SI unit, coordinate frame, sign convention, identification method, source data, uncertainty, operating range, date, author, and approval state. Classify parameters as measured, identified, supplier-provided, calculated, or assumed.
