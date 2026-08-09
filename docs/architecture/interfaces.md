# Interface Control Document

For every interface define producer, consumer, transport, schema version, timing, units, coordinate frame, valid range, invalid representation, freshness limit, rate, latency, precision, initialization, shutdown, fault behavior, and test vector.

## Canonical contracts

- Pose: NED position plus scalar-first BODY-to-NED unit quaternion.
- Twist: BODY linear and angular velocity in SI units.
- Wrench: BODY force and moment in SI units.
- Thruster force: signed newtons along configured positive axis.
- Depth: metres, positive down from current surface datum.
- Time: monotonic seconds for control; wall-clock time only for correlation.

## Versioning

Increment major version for incompatible field or semantic changes. Receivers shall reject unsupported major versions rather than guessing. Log protocol and configuration versions with every run.
