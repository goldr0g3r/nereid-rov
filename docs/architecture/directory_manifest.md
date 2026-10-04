# Directory Manifest

Purpose of every top-level area. Ownership rules are in ADR-008. Empty directories keep a
`.gitkeep` until they get content.

```text
nereid-rov/
├── firmware/                  ECU firmware (STM32, FreeRTOS), self-contained. See firmware/README.md
│   ├── app/                   L4 application SWCs: VehicleMode, StatusLed, BenchPanel, Lights
│   ├── rte/                   L3 hand-written RTE (SWC ports, task bodies)
│   ├── bsw/
│   │   ├── services/          L2 Os, SchM, EcuM, BswM, Det, Dem, WdgM, Log (+ Os/config: FreeRTOS common config)
│   │   ├── ecual/             L1 IoHwAb (later ComIf, sensor and ESC drivers)
│   │   └── mcal/include/      L0 MCAL API headers: Mcu, Dio, Gpt, Uart, Wdg
│   ├── cdd/                   complex device drivers (placeholder)
│   ├── lib/                   Fsm, RingBuf, Crc (platform independent)
│   ├── common/                Std_Types.h, ModuleIds.h
│   ├── port/
│   │   ├── stm32f412/         MCAL implementation on ST HAL, FreeRTOSConfig, linker script
│   │   │   └── cubemx/        Nereid-ROV.ioc + generated Core/ and Drivers/ (vendor)
│   │   └── host/              Linux MCAL simulation + FreeRTOS POSIX entry point
│   ├── variants/nereid_v1/    vehicle configuration + integration (pins, tasks, faults, calibration)
│   ├── tests/                 Unity unit tests, stubs, SIL smoke test
│   ├── tools/                 check_layers.py
│   ├── cmake/                 arm-none-eabi toolchain file
│   └── third_party/           FreeRTOS-Kernel, Unity (git submodules)
├── software/
│   ├── onboard/               future companion-computer software (Linux on the vehicle)
│   ├── topside/               operator station, joystick, mission control, telemetry tools
│   └── protocols/             tether protocol definition shared by firmware and topside (M2)
├── controllers/               controller design/prototyping (Python/Simulink): guidance, estimation,
│                              pid, cascaded, advanced, allocation, filters, validation, common
├── models/                    plant models: python/, simulink/, gazebo/
├── simulation/                scenarios and runners: python/, simulink/, sil/, hil/, co_simulation/
├── config/                    single source of truth for parameters: vehicle/, controllers/,
│                              safety/, experiments/, logging/  (ADR-002)
├── calibration/               sensor and thruster calibration evidence
├── experiments/               bench/, pool/, open_water/ plans and records
├── tests/                     system-level verification: sil/, hil/, integration/, regression/,
│                              acceptance/, fixtures/, test_vectors/, unit/{controllers,models}
├── data/                      raw/ (immutable), processed/, calibration/, system_identification/, reference/
├── results/                   generated figures, logs, reports (git-ignored content)
├── notebooks/                 analysis, modelling, tuning, system identification
├── hardware/                  mechanical/ (cad, drawings, manufacturing), electrical/ (schematics, pcb,
│                              wiring, power_budget), bom/
├── scripts/                   setup, simulation, testing, deployment helpers
├── tools/                     calibration, conversion, deployment, maintenance, sysid, visualization
├── third_party/, vendor/      external code that is not a firmware submodule
├── docs/                      see below
└── .github/workflows/         CI (firmware.yml)
```

## Documentation map

| Folder | Content |
|---|---|
| `docs/architecture/` | system overview, requirements template, interface control, this manifest |
| `docs/software/` | **firmware architecture**, **state machines**, **coding standard**, onboard/topside software |
| `docs/hardware/` | hardware design guide, **ECU resource allocation** |
| `docs/project/` | **roadmap** and open decisions |
| `docs/decisions/` | ADR-001 … ADR-008 |
| `docs/modeling/`, `docs/controls/`, `docs/simulation/` | dynamics, control, estimation, allocation, verification strategy |
| `docs/safety/`, `docs/operations/`, `docs/testing/` | safety case, procedures, test plan |
| `docs/obsidian/NereidROV/` | personal Obsidian vault (sketches). Formal specs live in the Markdown docs. |
