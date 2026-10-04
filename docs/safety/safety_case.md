# Safety Case and Hazard Controls

## 1. Safety principle

Software is not the only barrier. Use independent physical isolation, guarded propellers, appropriate fusing, pressure containment practices, strain relief, and controlled test areas. Safety limits and recovery behavior must be reviewed before deployment.

## 2. Hazard register starter

### Unexpected thruster start
Causes: startup transient, stale command, wiring error, software defect. Controls: hardware kill/isolation, guarded thrusters, neutral-before-arm, deliberate arming, command freshness, output watchdog, restrained testing.

### Water ingress
Causes: damaged O-ring, dirty sealing face, penetrator failure, cracked enclosure. Controls: inspection, pressure test, leak sensing, current protection, emergency recovery, post-dive maintenance.

### Battery fault or overheating
Causes: short circuit, overcurrent, wrong charger, damaged cell, enclosure heat. Controls: chemistry-appropriate BMS/charger, fusing near source, isolation, thermal monitoring, conservative load budget, fire-safe procedures.

### Pressure-vessel failure
Causes: over-depth, material defect, incorrect assembly. Controls: rated components, design review, unmanned barriered proof test, depth limit, inspection and retirement criteria.

### Tether entanglement or excessive load
Controls: strain relief, guards, tether management, abort limit, recovery line, operator coordination.

### Loss of communication
Controls: stale-command detection, predefined safe output, visible alarm, physical recovery plan. Do not assume surfacing is always safe; mission configuration must define behavior.

### Runaway controller
Controls: command/wrench limits, rate limits, independent watchdog, estimator plausibility, achieved-wrench monitoring, mode supervisor, hardware kill.

## 3. Safety requirements

- Propulsion shall remain inhibited after boot until deliberate arming criteria pass.
- Invalid or stale commands shall not be reused indefinitely.
- Non-finite control calculations shall produce safe output and a latched diagnostic.
- Emergency stop shall bypass ordinary mode logic.
- Re-arming after critical fault shall require deliberate operator action.
- Safety-critical limits shall be configuration-controlled and logged.
- Tests shall define abort criteria and a person responsible for calling abort.

## 4. Fault response matrix

Classify faults as advisory, degraded, failsafe, and emergency stop. For each fault define detection logic, debounce, latching, actuator response, operator indication, logging, reset conditions, and verification test.

**Firmware implementation.**
- Each fault is a Dem event with exactly these four classes, plus a debounce and latch configuration (`firmware/variants/nereid_v1/src/Dem_Cfg.c`).
- The worst active class drives the VehicleMode state machine: DEGRADED → reduced authority, FAILSAFE → propulsion neutral with operator reset, EMERGENCY → latched emergency stop. See [`state_machines.md`](../software/state_machines.md).
- Operator indication is the red LED pattern now, and telemetry from M2.

Current event table:

| Event | Class | Latched | Detection |
|---|---|---|---|
| DEV_ERROR | advisory | yes | Det programming-error report |
| TASK_OVERRUN | advisory | yes | task missed its period |
| WDGM_SUPERVISION | failsafe | yes | control task alive count outside 8–12 per 100 ms; persists → IWDG reset |
| WATCHDOG_RESET | advisory | yes | last reset caused by a watchdog |
| CONFIG_INVALID | failsafe | no | configuration validation (M6) |

Planned: COMM_LOST (failsafe, M2), sensor stale/implausible (degraded, M4), LEAK (emergency, M6), battery low/critical (degraded/failsafe, M6). The classes need a safety review before the first wet test.

## 5. Operational boundaries

Define maximum depth, current, tether length/load, temperature, battery limits, water type, visibility, allowed weather, launch/recovery method, and minimum crew. Exceeding an envelope boundary requires abort or explicit engineering disposition.

## 6. Disclaimer

This document is a starter safety argument, not professional certification. Applicable electrical, battery, pressure, marine, institutional, and local regulations must be identified for the actual build and operating location.
