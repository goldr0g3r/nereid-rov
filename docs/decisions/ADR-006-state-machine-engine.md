# ADR-006: State machine engine

- Status: Accepted
- Date: 2026-10-04

## Context

Behaviour (ECU states, vehicle modes, lights, and later the link, drivers and ESCs) should be written as explicit state machines. Vehicle modes need hierarchy: ARMED contains MANUAL, ASSISTED and DEGRADED, and E-STOP must be reachable from everywhere. Without hierarchy, transitions get duplicated.

## Decision

Write a small in-house **hierarchical state machine engine**, `firmware/lib/Fsm`:

- **States** are const descriptors (parent, initial child, entry/exit actions, handler).
- **Dispatch** offers the event to the active leaf and then its ancestors. External transitions run exit actions up to the LCA and entry actions down to the target, then drill into initial children.
- **Run-to-completion**, no dynamic memory, bounded depth, re-entrant dispatch rejected.
- **Machines own their event sources.** Queues come from the RTE, and periodic level evaluation uses a `TICK` event.

## Alternatives

- **QP/C (Quantum Leaps).** Mature UML statechart framework, but dual-licensed (GPL / commercial) and it brings its own active-object runtime, which overlaps with FreeRTOS + RTE.
- **Zephyr SMF.** Tied to Zephyr (see ADR-004).
- **Switch-case per machine.** Fine for flat 2-state machines, but repeats exit/entry logic and makes "from any state" transitions error-prone.
- **Table-driven flat FSM.** Simple, but no hierarchy.

## Consequences

- About 150 lines of engine code, covered by `tests/unit/test_fsm.c`.
- Every machine is specified in `docs/software/state_machines.md`, and each transition there maps to a unit test.
