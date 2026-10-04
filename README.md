# Nereid ROV

A solo, from-scratch mini remotely operated vehicle (ROV). The repository covers modelling, simulation, control, embedded firmware, hardware, verification and operation.

> **Current state (2026-10):**
> - **ECU firmware baseline exists** in [`firmware/`](firmware/README.md). It runs on a NUCLEO-F412ZG (STM32F412, FreeRTOS) and uses an AUTOSAR-inspired layered architecture with hierarchical state machines.
> - Mode management, fault management and watchdog supervision are implemented. They are unit-tested and run end-to-end on Linux through the FreeRTOS POSIX port.
> - The on-target bring-up needs one CubeMX regeneration step.
> - Models, controllers, topside software and hardware are still documentation only.
> - Next steps are in the [roadmap](docs/project/roadmap.md).

## Firmware quick start

```bash
git submodule update --init --recursive
cd firmware && cmake --preset host && cmake --build --preset host && ctest --preset host
```

Architecture: [`docs/software/firmware_architecture.md`](docs/software/firmware_architecture.md) ·
State machines: [`docs/software/state_machines.md`](docs/software/state_machines.md) ·
Pin plan: [`docs/hardware/ecu_resource_allocation.md`](docs/hardware/ecu_resource_allocation.md)

## Design goals

- One physical parameter source of truth for Python and Simulink plants.
- Controller logic independent of whether it drives a Python plant, Simulink plant, SIL target, HIL rig, or the real ROV.
- Explicit coordinate frames, units, sign conventions, interfaces, timing, saturation, and safety states.
- Traceability from requirement to model, controller, test, evidence, and operational procedure.
- Progressive development from 1-DOF heave to validated 6-DOF operation.

## Repository map

- `config/`: version-controlled parameters and experiment configurations.
- `models/`: Python and Simulink plant-model workspaces.
- `controllers/`: guidance, estimation, control, filtering, and allocation.
- `simulation/`: scenarios, runners, SIL, HIL, and co-simulation.
- `firmware/`: **ECU firmware** (STM32 + FreeRTOS): layered BSW/RTE/SWCs, ports, variants, tests.
- `software/`: topside station, future companion-computer software, tether protocol definitions.
- `hardware/`: mechanical, electrical, wiring, fabrication, and BOM artifacts.
- `calibration/`: sensor and actuator calibration evidence.
- `experiments/`: bench, pool, and open-water plans and records.
- `tests/`: unit through acceptance-level verification.
- `data/`: raw, processed, calibration, identification, and reference data.
- `results/`: generated figures, logs, benchmarks, and reports.
- `docs/`: system engineering and mathematical design documentation.

## Documentation reading order

Firmware first:

1. [`docs/project/roadmap.md`](docs/project/roadmap.md)
2. [`docs/software/firmware_architecture.md`](docs/software/firmware_architecture.md)
3. [`docs/software/state_machines.md`](docs/software/state_machines.md)
4. [`docs/decisions/`](docs/decisions) (ADR-004 … ADR-008)

System and physics:

1. [`docs/architecture/system_overview.md`](docs/architecture/system_overview.md)
2. [`docs/modeling/coordinate_frames.md`](docs/modeling/coordinate_frames.md)
3. [`docs/modeling/six_dof_dynamics.md`](docs/modeling/six_dof_dynamics.md)
4. [`docs/modeling/hydrodynamics_hydrostatics.md`](docs/modeling/hydrodynamics_hydrostatics.md)
5. [`docs/modeling/thruster_model.md`](docs/modeling/thruster_model.md)
6. [`docs/controls/control_architecture.md`](docs/controls/control_architecture.md)
7. [`docs/controls/thruster_allocation.md`](docs/controls/thruster_allocation.md)
8. [`docs/simulation/verification_strategy.md`](docs/simulation/verification_strategy.md)
9. [`docs/safety/safety_case.md`](docs/safety/safety_case.md)
10. [`docs/operations/test_and_dive_procedure.md`](docs/operations/test_and_dive_procedure.md)

## Important safety boundary

This repository is an engineering starting point, not a certification package. Propellers, batteries, pressure vessels, water ingress, tether forces, stored energy, and electrical faults can injure people or damage equipment. Use guarded thrusters, fusing, emergency isolation, current limiting, leak detection, pressure-tested enclosures, a physical kill method, and supervised testing. Never rely on software as the sole protection.

## Initial completion criteria

The first release should demonstrate: parameter traceability; open-loop plant sanity checks; bounded actuator commands; depth-hold SIL; loss-of-command safe state; bench verification with propellers removed or guarded; restrained wet test; and a signed test report.
