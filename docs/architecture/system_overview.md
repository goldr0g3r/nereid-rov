# System Overview

## 1. Scope

The system consists of a submerged vehicle, tether, topside operator station, power source, command and telemetry links, onboard compute, sensors, actuators, pressure housing, frame, buoyancy, ballast, and recovery provisions. The initial control target is stable manual operation with assisted depth and heading hold. The architecture must support expansion to station keeping and waypoint guidance.

## 2. Functional chain

```text
Operator / mission reference
          |
          v
Guidance and command shaping
          |
          v
State estimator <--- sensors and health monitors
          |
          v
Motion controllers
          |
          v
Generalized wrench command tau_d
          |
          v
Control allocation and saturation
          |
          v
ESCs and thrusters ---> ROV plant ---> environment
          ^                              |
          +---------- measured state ----+
```

The controller shall consume a reference, estimated state, health status, and sample time. It shall not depend on a specific simulation engine or hardware driver. Hardware abstraction converts normalized actuator commands into target-specific outputs.

## 3. Architectural layers

1. **Physical plant:** rigid-body inertia, added mass, damping, hydrostatics, thrusters, sensors, environment.
2. **Estimation:** calibration, filtering, attitude estimation, depth estimation, bias handling, health checks.
3. **Guidance:** pilot command mapping, setpoint generation, rate limiting, waypoint or line-of-sight logic.
4. **Control:** depth, heading, attitude, velocity, and position loops.
5. **Allocation:** maps requested body wrench to feasible thruster forces.
6. **Platform:** deterministic scheduling, drivers, communications, logging, watchdogs.
7. **Operation:** topside display, arming, emergency stop, checklists, recovery.

## 4. Canonical interfaces

### Estimated state

```text
position_ned_m      [x, y, z]
quaternion_nb       [q0, q1, q2, q3]
velocity_body_m_s   [u, v, w]
rate_body_rad_s     [p, q, r]
depth_m             scalar, positive down
health_flags        bit field or structured status
timestamp_s         monotonic time
```

### Desired generalized wrench

$$
\tau_d = [X_d,\;Y_d,\;Z_d,\;K_d,\;M_d,\;N_d]^T
$$

Translations are forces in newtons; rotations are moments in newton-metres. All wrench components are expressed in BODY unless an interface explicitly states otherwise.

### Thruster command

For thruster $i$, store command, requested force, achieved-force estimate, saturation state, direction, fault state, and timestamp. Separate allocation from electrical command conversion because forward and reverse thrust commonly differ.

## 5. Operating modes

- `POWER_OFF`: no propulsion energy.
- `SAFE`: compute alive, propulsion inhibited.
- `STANDBY`: checks active, not armed.
- `ARMED_MANUAL`: pilot directly requests body motion through bounded command shaping.
- `ARMED_ASSISTED`: depth, heading, or attitude hold enabled.
- `MISSION`: future autonomous guidance, if permitted.
- `DEGRADED`: failed sensor or actuator, reduced authority.
- `FAILSAFE`: neutral or predefined recovery behavior after a fault.
- `EMERGENCY_STOP`: immediate propulsion inhibition requiring deliberate reset.

Mode transitions require explicit guards. Arming must require fresh command, valid estimator, acceptable battery, no leak indication, configured limits, and neutral pilot input. Emergency stop shall override normal control.

The firmware implementation of these modes is the VehicleMode state machine. Its full transition table, guards and tests are in [`docs/software/state_machines.md`](../software/state_machines.md). `POWER_OFF` is physical. `MISSION` is deferred to roadmap M8.

## 5a. Compute partitioning

The **STM32 ECU** runs the deterministic, safety-relevant chain:

- I/O and sensor acquisition
- estimation
- control loops and allocation
- the mode/safety supervisor (VehicleMode, Dem, WdgM)

A **companion computer** may be added later for video, aided navigation and ML-based station keeping (roadmap M8). It sends references or wrench setpoints through the tether protocol. The ECU treats it as one more command source, with freshness and authority limits, and remains the only component that grants propulsion. See [`firmware_architecture.md` §10](../software/firmware_architecture.md).

## 6. Timing

Use monotonic timestamps and measure actual $\Delta t$. A practical starting decomposition is a fast actuator/control task, medium-rate estimation task, and slower telemetry/logging task. The ECU's initial task set (10 ms control, 20 ms comm, 100 ms housekeeping), with execution-time measurement and overrun/alive supervision, is defined in [`firmware_architecture.md` §6](../software/firmware_architecture.md). Exact rates are hardware-dependent and must be established through timing measurement. Every task needs a deadline, worst-case execution budget, stale-data threshold, and overrun response.

## 7. Configuration ownership

- Vehicle geometry and mass properties belong to `config/vehicle/`.
- Controller gains and limits belong to `config/controllers/`.
- Test-specific initial conditions belong to `config/experiments/`.
- Safety limits belong to `config/safety/` and require review.
- Raw data is immutable. Derived parameters record source dataset, method, date, uncertainty, and reviewer.

## 8. Quality attributes

- **Safety:** hardware propulsion isolation, bounded commands, default-safe startup.
- **Determinism:** bounded execution time and timestamped data.
- **Testability:** controllers run unchanged against simulation and hardware abstraction.
- **Traceability:** every model parameter links to measurement or assumption.
- **Observability:** log references, estimates, measurements, control terms, saturation, mode, and faults.
- **Reproducibility:** experiment configuration and software revision accompany every result.

## 9. Reference architecture rationale

The compact marine-craft model separates kinematics, inertia, Coriolis/centripetal effects, damping, hydrostatics, and applied wrench, which supports modular implementation and control design. Simulink projects similarly support dependency management, source control, componentization, and reusable referenced models. See `docs/references/bibliography.md`.
