# ADR-005: AUTOSAR-inspired layered firmware architecture

- Status: Accepted
- Date: 2026-10-04

## Context

The firmware should be reusable on another ROV, and on another MCU, without rewriting functionality. AUTOSAR Classic solves this in the automotive world with standardised layers, but full AUTOSAR depends on commercial tooling (ARXML, RTE and BSW generators) that a solo project can't use.

## Decision

Adopt AUTOSAR Classic **concepts and naming**, not compliance:

- **Layers**: MCAL API (`bsw/mcal`) → ECU abstraction (`bsw/ecual`) → services (`bsw/services`) → RTE (`rte/`) → application SWCs (`app/`). Platform-independent `lib/` and `common/` sit beside them.
- **MCU specifics** live in `port/<mcu>/` (MCAL implementations, vendor code). **Vehicle specifics** live in `variants/<rov>/` (configuration and integration code).
- **SWCs** communicate only through their `Rte_<Swc>.h` port API. The RTE is hand-written.
- **Module set**, named after AUTOSAR: EcuM, BswM, SchM, Det, Dem, WdgM, IoHwAb, plus Os, Log, and Com later.
- **Rules are enforced** by the CMake target graph (an SWC can't see BSW include paths) and by `tools/check_layers.py` in CI.

## Consequences

- There is a little more boilerplate than in a flat design (RTE functions, config tables). In return, SWCs and services can be unit-tested in isolation, and porting is limited to `port/` and `variants/`.
- Without an RTE generator, adding a port means editing `Rte_<Swc>.h` and `Rte.c` by hand. Revisit if the number of SWCs grows past about 15 (option: generate the RTE from YAML).
- This is not an AUTOSAR-certified stack and makes no claims of compliance.
