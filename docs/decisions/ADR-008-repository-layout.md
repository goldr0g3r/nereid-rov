# ADR-008: Repository layout and firmware ownership

- Status: Accepted
- Date: 2026-10-04

## Context

The original scaffold had four places that could hold embedded code: `firmware/`, `software/onboard`, `software/drivers` and `controllers/`. The CubeMX project sat in `firmware/workspace/Neried-ROV` (misspelt).

## Decision

| Path | Owns |
|---|---|
| `firmware/` | **All code that runs on the vehicle ECU** (STM32): layered firmware, ports, variants, tests, tools. Self-contained, so it can be copied to another ROV project. |
| `software/onboard/` | Future companion-computer software (Linux on the vehicle). |
| `software/topside/` | Operator station, joystick, telemetry tools. |
| `software/protocols/` | Tether protocol definition shared by firmware Com and topside (from M2). |
| `controllers/` | Controller design and prototyping (Python/Simulink), not deployed code. Proven algorithms are re-implemented in C as `firmware/app` SWCs or `firmware/lib` functions, with tests comparing against the design reference vectors (`tests/test_vectors`). |
| `tests/` (top level) | System-level verification: SIL scenarios, HIL, acceptance. Firmware unit tests live in `firmware/tests`. |

- `software/drivers/` is removed; drivers belong in `firmware/bsw` or `firmware/port`.
- The CubeMX project moves to `firmware/port/stm32f412/cubemx/` and is renamed `Nereid-ROV.ioc`.

## Consequences

- One unambiguous home per artefact. `docs/architecture/directory_manifest.md` describes the layout.
