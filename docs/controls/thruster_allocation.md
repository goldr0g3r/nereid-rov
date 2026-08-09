# Thruster Geometry and Control Allocation

## 1. Geometry matrix

For thruster \(i\), let \(r_i\) be its position from the body origin and \(d_i\) a unit vector in the positive-force direction, both in BODY. Its wrench column is

\[
b_i=\begin{bmatrix}d_i\\r_i\times d_i\end{bmatrix}.
\]

Stack columns to obtain

\[
B=[b_1\;b_2\;\cdots\;b_n],
\qquad \tau=BT,
\]

where \(T=[T_1,\ldots,T_n]^T\).

## 2. Unconstrained allocation

For overactuated systems, the minimum-norm solution is

\[
T=B^\dagger\tau_d.
\]

A weighted solution is

\[
T=W^{-1}B^T(BW^{-1}B^T)^{-1}\tau_d,
\]

when the inverse exists. \(W\) can penalize inefficient or degraded thrusters.

## 3. Constrained allocation

Real allocation should account for asymmetric limits, rate limits, deadband, disabled thrusters, and priority. A bounded least-squares objective is

\[
\min_T \|Q(BT-\tau_d)\|_2^2+\lambda\|WT\|_2^2
\]

subject to

\[
T_{min}\le T\le T_{max},
\qquad \Delta T_{min}\le T-T_{prev}\le\Delta T_{max}.
\]

\(Q\) expresses axis priority. For example, a safety requirement may prioritize heave and attitude over surge.

## 4. Achieved wrench

After saturation, compute and report

\[
\tau_{ach}=BT_{cmd},
\qquad e_\tau=\tau_d-\tau_{ach}.
\]

Controllers and anti-windup logic need achieved rather than requested wrench. Allocation failure must be visible to the state machine and operator.

## 5. Authority analysis

Evaluate rank, singular values, condition number, force/moment envelope, and single-thruster failures. Full 6-DOF authority requires \(\operatorname{rank}(B)=6\), but many mini ROVs intentionally control fewer axes. State exactly which axes are independently controlled.

## 6. Geometry validation

For each thruster, issue a small positive test command with the vehicle safely restrained and verify observed force and moment signs. Compare measured static wrench to the predicted column \(b_i\). Wrong \(d_i\), swapped connectors, and mirrored mounting are critical defects.

## 7. Command conversion

Allocation produces desired force. The actuator map converts force to command:

\[
c_i=T_i^{-1}(T_{cmd,i},V).
\]

Use separate monotonic inverses for forward and reverse, include deadband handling, and clamp to the validated domain. Do not invert a non-monotonic fitted polynomial blindly.
