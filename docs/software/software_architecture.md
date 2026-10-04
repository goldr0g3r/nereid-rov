# Onboard and Topside Software Architecture

This document states *what* the vehicle software must do. *How* the ECU firmware does it
(layers, modules, tasks, state machines) is in
[`firmware_architecture.md`](firmware_architecture.md) and [`state_machines.md`](state_machines.md).

| Software | Runs on | Location |
|---|---|---|
| ECU firmware | STM32F412 (NUCLEO-F412ZG), FreeRTOS | `firmware/` |
| Companion computer software (future) | Linux SBC on the vehicle | `software/onboard/` |
| Operator station, joystick, telemetry | topside PC | `software/topside/` |
| Tether protocol | shared definition | `software/protocols/` |

## 1. Onboard services

Each service below maps to a firmware module (in brackets).

- Hardware abstraction and drivers (MCAL, IoHwAb, device drivers)
- Sensor acquisition and timestamping (drivers + Gpt, M4)
- Calibration and health monitoring (drivers → Dem, M4/M6)
- State estimation (StateEst SWC, M4)
- Command validation and reference shaping (Com, PilotCmd SWC, M2)
- Control and allocation (DepthCtrl/HeadingCtrl/ThrustAlloc SWCs, M3/M5)
- Safety supervisor and watchdog (VehicleMode, BswM, Dem, WdgM + IWDG, **implemented**)
- Telemetry and logging (Log **implemented**; Com telemetry M2)
- Configuration loading and validation (variant config now; NvM + validation M6)

## 2. Deterministic control cycle

A cycle conceptually performs: read timestamp; obtain freshest sensor snapshot; validate age and health; update estimator; process pilot/reference input; evaluate mode; compute control wrench; allocate and limit; issue actuator outputs; log a coherent record; update watchdog.

No blocking network, disk, UI, or camera work belongs in the deterministic control path. Use bounded queues and define behavior for overflow.

## 3. Communication protocol

Every packet should include protocol version, message type, sequence number, sender timestamp, payload length, payload, and integrity check. Define byte order, units, frame, scaling, ranges, timeout, duplicate policy, and compatibility rules. Commands require freshness and arming context.

## 4. Logging schema

At minimum log monotonic time, wall time if available, mode, references, raw and calibrated sensors, estimated state, controller terms, requested and achieved wrench, thruster force and command, saturation, battery, leak state, communication age, faults, and configuration/software identifiers.

## 5. Configuration validation

Reject missing keys, wrong units, invalid matrix dimensions, non-finite numbers, non-positive mass, non-unit thruster directions, impossible limits, and duplicate identifiers. Configuration errors shall inhibit arming.

## 6. Startup and shutdown

Startup defaults to propulsion inhibited. Load and validate configuration, initialize outputs neutral, start sensors, establish estimator validity, verify safety inputs, then permit deliberate arming. Shutdown commands neutral first, confirms or times out safely, stops tasks, flushes logs, and de-energizes propulsion where hardware supports it.

## 7. Fault containment

A camera crash, telemetry loss, or log failure must not corrupt the control loop. Conversely, stale command, leak, estimator invalidity, actuator fault, or control deadline violation must reach the safety supervisor immediately.
