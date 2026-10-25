# Control Architecture

## 1. Separation of concerns

Guidance creates references, estimation produces state, control creates a desired body wrench, allocation distributes that wrench among thrusters, and drivers issue electrical commands. Keep these contracts independent.

## 2. Cascaded control

For depth, use an outer position loop and inner vertical-velocity loop when sensors and timing support it:

$$
e_z=z_d-z,
\qquad w_d=\operatorname{sat}(k_{pz}e_z,\,-w_{max},w_{max}),
$$

$$
e_w=w_d-w,
\qquad Z_d=k_{pw}e_w+k_{iw}\int e_wdt-k_{dw}\dot w+Z_{ff}.
$$

Because depth is positive down, verify that positive $Z_d$ commands a downward force. If vertical velocity is noisy or unavailable, begin with a direct depth PID and filtered derivative on measurement.

For heading, wrap angular error:

$$
e_\psi=\operatorname{wrap}_{[-\pi,\pi)}(\psi_d-\psi).
$$

A yaw-rate cascade is

$$
r_d=\operatorname{sat}(k_{p\psi}e_\psi,-r_{max},r_{max}),
\qquad N_d=k_{pr}(r_d-r)+k_{ir}\int(r_d-r)dt.
$$

## 3. PID law

A practical PID with derivative on measurement is

$$
u_{raw}=K_pe+K_i x_I-K_d\dot y+u_{ff},
$$

$$
\dot x_I=e+K_{aw}(u_{sat}-u_{raw}),
\qquad u_{sat}=\operatorname{sat}(u_{raw},u_{min},u_{max}).
$$

The back-calculation term prevents integrator windup during saturation. Conditional integration is an alternative. Log proportional, integral, derivative, feedforward, raw output, saturated output, and saturation reason.

## 4. Derivative filtering

An ideal derivative amplifies noise. One continuous form is

$$
D(s)=K_d\frac{N s}{s+N}.
$$

Select cutoff below dominant sensor-noise frequencies yet above required control bandwidth, then discretize using the actual controller sample time. Derivative on measurement avoids setpoint kick.

## 5. Feedforward and restoring compensation

When model confidence is adequate,

$$
\tau_d=\tau_{fb}+\hat g(\eta)+\tau_{command,ff}.
$$

Hydrostatic compensation reduces steady integrator demand. Limit compensation and disable it if pose or parameter validity fails.

## 6. Discrete implementation

Using sample $T_s$, a simple integrator is

$$
x_I[k]=x_I[k-1]+T_s e[k],
$$

but production logic must use measured bounded $\Delta t$, handle missed cycles, and reset or track integrator state across mode changes. Bumpless transfer initializes controller state so the new mode output matches the previous output.

## 7. Tuning order

1. Validate sensor signs and thruster signs.
2. Disable integral and nonlinear compensation.
3. Tune inner rate or velocity loop with conservative limits.
4. Tune outer attitude or position loop slower than the inner loop.
5. Add integral only for persistent bias.
6. Test saturation, setpoint steps, disturbances, and mode changes.
7. Repeat with voltage variation, tether load, noise, and model uncertainty.

A common bandwidth guideline is to make the outer loop several times slower than the inner loop, but determine the actual ratio through measured robustness and available actuator authority.

## 8. Linearized state-space design

Around equilibrium $(x_0,u_0)$,

$$
\delta\dot x=A\delta x+B\delta u,
\qquad \delta y=C\delta x+D\delta u.
$$

For LQR,

$$
J=\int_0^\infty (x^TQx+u^TRu)dt,
\qquad u=-Kx.
$$

Check controllability, state scaling, actuator limits, model validity, and estimator interaction. Do not deploy an advanced controller until the same safety, saturation, timing, and verification requirements applied to PID are satisfied.

## 9. Performance metrics

Use rise time, overshoot, settling time, steady-state error, RMS tracking error, integral absolute error, control effort, saturation fraction, disturbance rejection, gain/phase margin where applicable, and recovery after mode changes. Acceptance bounds belong to requirements, not to plots chosen after testing.
