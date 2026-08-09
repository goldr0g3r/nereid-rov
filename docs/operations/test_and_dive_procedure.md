# Bench, Pool, and Dive Procedure

## 1. Before energizing propulsion

- Review test objective, hazards, roles, abort call, and recovery plan.
- Confirm propeller guards and clear exclusion zone.
- Inspect frame, tether, enclosure, connectors, seals, fasteners, and strain relief.
- Confirm calibration and configuration revisions.
- Check battery condition, polarity, fusing, isolation, and current limit.
- Verify physical kill method.
- Confirm all actuator outputs are neutral before power application.

## 2. Dry bench sequence

Prefer removing propellers or mechanically preventing hazardous motion. Power low-voltage logic first, verify sensor signs, mode display, command timeout, leak input, low-voltage behavior, watchdog, and emergency stop. Verify each thruster channel identity with the safest available method.

## 3. Pre-dive checks

1. Clean and inspect sealing surfaces and O-rings.
2. Assemble using approved lubricant and torque practice.
3. Perform the approved enclosure integrity check.
4. Confirm slight positive buoyancy and correct trim in a restrained shallow test.
5. Confirm depth datum at surface.
6. Confirm fresh command link and telemetry.
7. Arm only with vehicle in controlled water and personnel clear.
8. Pulse each thruster at low command and verify expected motion.
9. Test manual recovery and emergency stop.

## 4. Progressive pool test

- Manual low-authority maneuvering.
- Command-loss test while restrained.
- Depth sensor and estimator comparison.
- Open-loop pulse tests for model validation.
- Depth hold at small setpoint changes.
- Heading hold.
- Combined maneuvers.
- Disturbance and tether tests.

Increase authority one variable at a time. Keep a live anomaly log. Abort on ingress, abnormal current, battery alarm, unstable oscillation, lost telemetry, unexpected thrust, tether issue, enclosure damage, or operator uncertainty.

## 5. Post-dive

Disarm, isolate propulsion power, recover without pulling on penetrators, rinse as required, inspect for water and corrosion, download and checksum logs, record battery usage, document anomalies, and quarantine damaged components. Do not overwrite raw data.

## 6. Data package

Store test plan, configuration, commit hash, operator notes, raw telemetry, video index, processed results, anomaly report, and approval decision under a unique experiment identifier.
