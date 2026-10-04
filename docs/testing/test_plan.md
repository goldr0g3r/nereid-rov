# Verification and Test Plan

## 1. Test levels

- **Unit:** equations, transforms, filters, PID, maps, protocol encoding. Firmware: `firmware/tests/unit` (Unity, host), one test per lib module, service and SWC. SWCs are tested against a fake RTE.
- **Integration:** estimator plus sensors, controller plus allocator, drivers plus interfaces.
- **SIL:** production behavior against simulated plant. Firmware: the complete ECU build runs on the FreeRTOS POSIX port (`nereid_host`). Today it is driven by a scripted button (`host_smoke_mode_sequence`); from M5 it will be driven by the Python plant through a socket MCAL port.
- **HIL:** real compute and timing against simulated or emulated I/O.
- **Bench:** electrical and mechanical subsystem verification.
- **Wet restrained:** propulsion and sensing under controlled restraint.
- **Pool:** vehicle-level performance.
- **Acceptance:** requirements-based release evidence.

### 1.1 Firmware verification commands

```bash
cd firmware && cmake --preset host && cmake --build --preset host && ctest --preset host
python3 firmware/tools/check_layers.py
```

CI runs both on every push (`.github/workflows/firmware.yml`). A behaviour change to a state machine updates [`state_machines.md`](../software/state_machines.md) and its tests in the same commit.

## 2. Standard test record

Include test ID, requirement IDs, objective, hazards, personnel, configuration, serial numbers, calibration versions, software commit, parameter revision, equipment, prerequisites, procedure, expected result, abort criteria, raw data path, anomalies, actual result, pass/fail, and signatures.

## 3. Model tests

Verify transforms, mass-matrix symmetry, positive definiteness, Coriolis power property, damping dissipation, hydrostatic signs, quaternion normalization, current-relative velocity, and analytical reduced-order responses.

## 4. Controller tests

For each mode test zero error, positive/negative step, ramp, disturbance, noise, saturation, anti-windup, integrator reset, mode transfer, stale state, invalid state, actuator loss, and sample-time jitter. Confirm bounded output for every input.

## 5. Safety tests

Verify power-up neutral, arm guards, command timeout, communication loss, estimator invalidity, leak input, low voltage, overcurrent signal, watchdog reset, emergency stop, restart inhibit, and operator indication. Where possible inject faults without energizing propellers.

## 6. Example performance requirements

Project-specific values must replace placeholders:

- Depth step within `[TBD]` settling time.
- Depth overshoot below `[TBD]`.
- Steady depth error below `[TBD]` in stated current.
- Heading RMS error below `[TBD]`.
- No sustained saturation in nominal mission.
- Control deadline miss rate below `[TBD]`.
- Command timeout reaches safe actuator state within `[TBD]`.

## 7. Exit criteria

No unresolved critical safety defect; all required tests passed; anomalies dispositioned; raw evidence archived; calibration current; configuration frozen; rollback package available; operator checklist reviewed.
