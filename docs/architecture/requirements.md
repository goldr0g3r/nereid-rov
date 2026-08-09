# System Requirements Template

Use identifiers such as `SYS-`, `MOD-`, `CTL-`, `SAF-`, `HW-`, and `TST-`. Requirements should be necessary, singular, unambiguous, feasible, and verifiable.

Example:

**SAF-ARM-001:** The propulsion subsystem shall remain inhibited from power-up until a deliberate arming command is received and all configured arming guards are valid.

- Rationale: prevents unexpected startup.
- Verification: HIL and bench test.
- Pass criterion: all actuator outputs remain within configured neutral tolerance until guards pass.
- Evidence: `[experiment-id]/report`.

Avoid vague words such as fast, robust, user-friendly, or sufficient unless quantified.
