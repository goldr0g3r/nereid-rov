# Hydrodynamics, Hydrostatics, and Environment

## 1. Weight and buoyancy

\[
W=mg,\qquad B=\rho g\nabla,
\]

where \(\nabla\) is displaced volume. In a NED inertial frame, gravity points down and buoyancy up. The ideal body-frame forces are obtained by rotating inertial forces into BODY:

\[
f_g^b=R_{bn}[0,0,W]^T,\qquad f_b^b=R_{bn}[0,0,-B]^T.
\]

If \(r_g\) and \(r_b\) are body-origin offsets to the centers of gravity and buoyancy,

\[
\tau_g=\begin{bmatrix}f_g^b\\r_g\times f_g^b\end{bmatrix},\qquad
\tau_b=\begin{bmatrix}f_b^b\\r_b\times f_b^b\end{bmatrix}.
\]

The restoring vector used on the left side of the dynamic equation is

\[
g(\eta)=-(\tau_g+\tau_b).
\]

Verify this sign against a simple case: a positively buoyant, level, stationary ROV must accelerate upward in NED, meaning negative \(\dot w\).

## 2. Static stability

Common design intent is slight positive buoyancy and center of buoyancy above center of gravity. For small roll angle \(\phi\), a restoring moment may be approximated as

\[
K_R\approx-k_\phi\phi,
\qquad k_\phi\approx B\,GM_T,
\]

where \(GM_T\) is an effective transverse metacentric or lever measure for the configuration. For a fully submerged rigid body, derive the moment directly from \(r_b-r_g\). Do not use surface-vessel formulas without checking applicability.

## 3. Damping

A practical low-speed model combines linear and quadratic damping:

\[
D(\nu_r)\nu_r=D_L\nu_r+D_Q|\nu_r|\nu_r,
\]

where absolute value is elementwise in the diagonal approximation. Per-axis force can be written

\[
F_i=-d_i v_i-d_{i|i|}|v_i|v_i.
\]

A geometry-based estimate for quadratic drag is

\[
F_D=-\frac{1}{2}\rho C_D A |v_r|v_r.
\]

The coefficient and projected area depend on direction, Reynolds number, blockage, appendages, tether, and thruster flow. Use estimates for initialization only, then identify from tests.

## 4. Water current

Store current in NED and transform it into body axes at each step:

\[
v_c^b=R_{bn}v_c^n,
\qquad \nu_{r,linear}=\nu_1-v_c^b.
\]

Models may include steady current, slowly varying current, bounded gusts, and vertical current. Scenario files must state whether the current is inertial-fixed or body-fixed.

## 5. Tether model

At minimum, model tether load as bounded disturbance. A lumped model may use

\[
f_t=-k_t\Delta l\,\hat e-c_t\dot l\,\hat e+f_{drag},
\]

only when the tether is taut, where \(\hat e\) is the line direction. Tether moment is

\[
m_t=r_{attach}\times f_t.
\]

A simple drag approximation is

\[
f_{drag}\approx-\frac12\rho C_{D,t}A_t|v_{rel}|v_{rel}.
\]

The tether can dominate mini-ROV behavior. Validate with the intended length, buoyancy, bend radius, and current.

## 6. Compressibility and density

For shallow pool tests, constant density is usually sufficient. For saltwater or temperature variation, record density at test time because it changes buoyancy and the pressure-to-depth conversion. Flexible buoyancy materials and trapped gas may change displaced volume with depth.

## 7. Identification tests

- Weigh the assembled ROV in air.
- Measure submerged apparent weight to estimate net buoyancy.
- Suspend from multiple orientations to estimate center of gravity.
- Use tilt/restoring tests to infer center-of-buoyancy offset.
- Conduct per-axis tow or free-decay tests for damping.
- Repeat at several speeds to separate linear and quadratic terms.
- Record uncertainty and configuration, including tether and payload.

## 8. Validation plots

For each axis, compare measured and predicted velocity, displacement, force, settling time, decay envelope, residual, and parameter confidence. Validate on a dataset not used for fitting.
