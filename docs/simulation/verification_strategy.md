# Simulation and Verification Strategy

## 1. Fidelity ladder

1. Algebraic sign and unit tests.
2. 1-DOF open-loop analytical comparisons.
3. 1-DOF closed-loop simulations.
4. Reduced coupled model.
5. Nonlinear 6-DOF simulation.
6. Controller software-in-the-loop (SIL).
7. Processor or hardware-in-the-loop (HIL).
8. Dry bench tests with propulsion made safe.
9. Restrained wet tests.
10. Pool tests.
11. Controlled open-water tests.

Progress only when entry and exit criteria are met. Simulation does not replace physical safety controls.

## 2. Python and Simulink parity

Both plants shall consume the same parameter definitions or generated representations, use identical units and frames, expose the same states and wrench inputs, and run shared test vectors. Exact numerical equality is not required across solvers, but trajectories must satisfy declared tolerances.

Recommended parity cases:

- zero state, zero input;
- pure force on each translational axis;
- pure moment on each rotational axis;
- gravity/buoyancy at selected attitudes;
- current-induced relative damping;
- single-thruster command;
- actuator saturation and lag;
- randomized bounded regression cases.

## 3. Analytical test cases

For constant acceleration with no damping,

\[
v(t)=v_0+at,\qquad x(t)=x_0+v_0t+\frac12at^2.
\]

For a first-order actuator step,

\[
T(t)=T_{ss}+(T_0-T_{ss})e^{-t/\tau_T}.
\]

For linear free decay,

\[
m_e\dot v+dv=0
\quad\Rightarrow\quad
v(t)=v_0e^{-dt/m_e}.
\]

These cases isolate integration and sign errors before nonlinear tests.

## 4. Numerical verification

Run timestep convergence studies. If \(x_h\) and \(x_{h/2}\) differ materially, results are not timestep-independent. Monitor energy, quaternion norm, solver warnings, stiffness, algebraic loops, discontinuities, and zero crossings.

## 5. Uncertainty and robustness

Vary mass, buoyancy, centers, inertia, added mass, damping, thrust scale, actuator lag, sensor bias/noise, latency, current, tether load, voltage, and sample jitter. A Monte Carlo result must show distributions and failure rates, not only a successful trace.

## 6. SIL

SIL runs the production controller interface against a simulated plant. Verify mode transitions, stale data, saturation, anti-windup, command timeout, logging, configuration validation, and deterministic replay.

## 7. HIL

HIL adds real compute, interfaces, driver timing, or ESC emulation. Test loop timing, clock drift, packet loss, reboot behavior, watchdog, sensor dropout, corrupted messages, brownout recovery, and physical emergency stop. Keep energetic actuators disconnected or mechanically safe unless the test specifically requires them.

## 8. Requirements traceability

Each requirement should have an identifier, rationale, verification method, test procedure, expected result, evidence path, status, and reviewer. Use objective pass/fail thresholds defined before running the test.

## 9. Regression baseline

Store scenario configuration, parameter revision, commit hash, solver and timestep, expected metrics, tolerances, and evidence. Avoid using binary plots as the only oracle; compare numerical metrics and invariants.
