# ADR-001: Coordinate Convention

- Status: Accepted (2026-10-04)
- Decision: Use NED inertial frame and forward-starboard-down BODY frame. Use SI units and scalar-first BODY-to-NED quaternion internally.
- Consequences: Depth and body heave are positive down. CAD and sensor frames require explicit transforms. Euler angles are display quantities; quaternions are preferred for propagation.
