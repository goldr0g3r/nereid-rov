# Contributing

This is currently a solo project; the rules below keep it reviewable for future-me.

1. Describe the change and the affected requirement, in an issue or in the commit message.
2. Use a short-lived branch. `main` must always pass `ctest --preset host` and `firmware/tools/check_layers.py`.
3. Update design documentation and tests with every behavioral change.
4. Never mix generated files with source artifacts.
5. Record coordinate frame, unit, sample time, valid range, and failure behavior for every interface.
6. Require review for safety limits, controller gains, thruster maps, and hardware changes.
7. Preserve raw experiment data as immutable evidence; derive processed data reproducibly.
8. Firmware: follow [`docs/software/coding_standard.md`](docs/software/coding_standard.md) and the layer rules in [`firmware_architecture.md`](docs/software/firmware_architecture.md). Changing a state machine means changing its spec in [`state_machines.md`](docs/software/state_machines.md) and its unit tests together.
9. Architectural decisions get an ADR in `docs/decisions/`.
