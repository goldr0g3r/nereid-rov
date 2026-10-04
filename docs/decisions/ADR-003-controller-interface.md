# ADR-003: Controller Interface

- Status: Accepted (2026-10-04)
- Decision: Controllers consume reference, estimated state, health, and measured sample interval, and output a desired BODY wrench plus status.
- Consequences: Simulation engines and hardware drivers remain outside controller logic. Allocation and electrical command conversion are separate stages.
