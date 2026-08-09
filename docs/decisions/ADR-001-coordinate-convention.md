# ADR-001: Coordinate Convention

- Status: Proposed
- Decision: Use NED inertial frame and forward-starboard-down BODY frame. Use SI units and scalar-first BODY-to-NED quaternion internally.
- Consequences: Depth and body heave are positive down. CAD and sensor frames require explicit transforms. Euler angles are display quantities; quaternions are preferred for propagation.
