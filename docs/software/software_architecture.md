# Onboard and Topside Software Architecture

## 1. Onboard services

- Hardware abstraction and drivers
- Sensor acquisition and timestamping
- Calibration and health monitoring
- State estimation
- Command validation and reference shaping
- Control and allocation
- Safety supervisor and watchdog
- Telemetry and logging
- Configuration loading and validation

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
