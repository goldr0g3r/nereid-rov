# Thruster and Actuator Model

## 1. Model layers

Use separate layers for command, ESC behavior, motor/propeller dynamics, static thrust, installation effects, and body-wrench mapping. This prevents a PWM curve from being mistaken for a hydrodynamic model.

## 2. Static thrust map

A common first model is

\[
T=k_T n|n|,
\]

where \(n\) is shaft speed. If shaft speed is unavailable, identify force directly from normalized command \(c\in[-1,1]\):

\[
T(c)=\begin{cases}
k_f(c-c_{0f})^{\alpha_f},&c>c_{db,+}\\
0,&c_{db,-}\le c\le c_{db,+}\\
-k_r(-c-c_{0r})^{\alpha_r},&c<c_{db,-}.
\end{cases}
\]

Forward and reverse coefficients should be independent. Prefer a monotonic calibrated lookup table when data supports it. Clamp extrapolation beyond tested command and voltage.

## 3. Voltage dependence

A useful empirical extension is

\[
T(c,V)=T(c,V_0)\left(\frac{V}{V_0}\right)^\gamma,
\]

where \(\gamma\) is identified. Do not assume a universal exponent. Log bus voltage and current because battery sag changes available authority.

## 4. Dynamic response

Represent actuator lag by

\[
\tau_T\dot T+T=T_{ss}(c,V),
\]

with optional asymmetric rise/fall time constants, rate limits, transport delay, and zero-crossing behavior. Discrete exact update for constant input over \(\Delta t\) is

\[
T_{k+1}=e^{-\Delta t/\tau_T}T_k+(1-e^{-\Delta t/\tau_T})T_{ss,k}.
\]

## 5. Installation effects

Thruster performance can change with guards, frame blockage, nearby hulls, inflow, interaction between jets, and vehicle speed. Introduce an installation factor \(0<\kappa_i\le1\) only when supported by data:

\[
T_{installed,i}=\kappa_i T_{bench,i}.
\]

Avoid double-counting interaction as both reduced thrust and added vehicle drag.

## 6. Electrical command boundary

The actuator interface shall specify neutral, minimum and maximum command, update rate, arming sequence, timeout, invalid-command response, direction, deadband, slew rate, and fault feedback. Vendor-specific values belong in configuration and test evidence, not in generic control equations.

## 7. Characterization procedure

1. Mount the thruster in a rigid guarded test rig with a calibrated load cell.
2. Submerge at representative depth with adequate clearance from walls and surface.
3. Sweep command slowly in forward and reverse at several voltages.
4. Measure command, voltage, current, force, water temperature, and time.
5. Repeat randomized points to expose hysteresis and heating.
6. Fit separate forward/reverse maps and retain residuals.
7. Validate on withheld runs.
8. Record dry-operation restrictions from the manufacturer.

## 8. Acceptance checks

- Neutral command produces acceptably small force.
- Force sign agrees with geometry definition.
- Mapping is monotonic over each direction.
- Saturation and deadband are explicit.
- No command beyond calibration range is emitted.
- A stale command transitions to the defined safe output.
- Current limiting and brownout response are tested safely.

Commercial underwater thrusters may have different forward and reverse thrust and operational constraints, so project-specific supplier data and wet characterization are essential. See the bibliography.
