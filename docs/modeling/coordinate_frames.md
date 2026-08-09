# Coordinate Frames and Conventions

## 1. Mandatory decision

Adopt one convention before creating a plant or controller. This project recommends North-East-Down (NED) for the inertial navigation frame and a forward-starboard-down BODY frame. If the mechanical team uses CAD axes that differ, define an explicit fixed transform rather than silently changing signs.

## 2. State vectors

Pose in NED:

\[
\eta = [x,\;y,\;z,\;\phi,\;\theta,\;\psi]^T
\]

where \(x\) is north, \(y\) east, \(z\) down, and \(\phi,\theta,\psi\) are roll, pitch, yaw. Body velocity:

\[
\nu = [u,\;v,\;w,\;p,\;q,\;r]^T
\]

with linear velocity \(\nu_1=[u,v,w]^T\) and angular velocity \(\nu_2=[p,q,r]^T\). Generalized body wrench:

\[
\tau=[X,\;Y,\;Z,\;K,\;M,\;N]^T.
\]

## 3. Rotation matrix

For the yaw-pitch-roll sequence \(R_{nb}=R_z(\psi)R_y(\theta)R_x(\phi)\), let \(c_\alpha=\cos\alpha\), \(s_\alpha=\sin\alpha\). Then

\[
R_{nb}=\begin{bmatrix}
c_\psi c_\theta & c_\psi s_\theta s_\phi-s_\psi c_\phi & c_\psi s_\theta c_\phi+s_\psi s_\phi\\
s_\psi c_\theta & s_\psi s_\theta s_\phi+c_\psi c_\phi & s_\psi s_\theta c_\phi-c_\psi s_\phi\\
-s_\theta & c_\theta s_\phi & c_\theta c_\phi
\end{bmatrix}.
\]

A vector transforms from BODY to NED as \(v^n=R_{nb}v^b\), and \(R_{bn}=R_{nb}^T\).

## 4. Euler-angle kinematics

\[
\dot{\eta}=J(\eta)\nu,\qquad
J(\eta)=\begin{bmatrix}R_{nb}&0\\0&T(\phi,\theta)\end{bmatrix}
\]

where

\[
T=\begin{bmatrix}
1 & s_\phi\tan\theta & c_\phi\tan\theta\\
0 & c_\phi & -s_\phi\\
0 & s_\phi/c_\theta & c_\phi/c_\theta
\end{bmatrix}.
\]

This representation is singular at \(\theta=\pm90^\circ\). Use a unit quaternion internally when large pitch angles are possible.

## 5. Quaternion convention

Use scalar-first \(q_{nb}=[q_0,q_1,q_2,q_3]^T\), representing BODY orientation relative to NED. Enforce

\[
\|q\|_2=1.
\]

With body rate \(\omega=[p,q,r]^T\),

\[
\dot q=\frac{1}{2}\Omega(\omega)q,
\]

\[
\Omega(\omega)=\begin{bmatrix}
0&-p&-q&-r\\p&0&r&-q\\q&-r&0&p\\r&q&-p&0
\end{bmatrix}.
\]

Normalize after numerical propagation, while treating persistent norm drift as a defect rather than hiding it.

## 6. Skew-symmetric operator

For \(a=[a_1,a_2,a_3]^T\),

\[
S(a)=\begin{bmatrix}0&-a_3&a_2\\a_3&0&-a_1\\-a_2&a_1&0\end{bmatrix},
\qquad S(a)b=a\times b.
\]

## 7. Depth, altitude, and pressure

Depth is positive downward from the chosen free-surface datum. Altitude is positive distance above the bottom and is not interchangeable with depth. For shallow water, gauge pressure can be approximated by

\[
p_g\approx\rho g h,
\]

so \(h\approx p_g/(\rho g)\). Calibration must account for atmospheric reference, density, temperature effects, offset, scale, and installation elevation.

## 8. Required frame audit

For each sensor and actuator, document native axes, mounting rotation, origin offset, units, sign, timestamp source, calibration, and transform owner. Add tests for zero orientation, +90-degree rotations, gravity direction, positive depth, positive yaw, and every thruster's positive-force direction.
