# Changelog

## Unreleased

### Docs: math and Markdown rendering fix (2026-10-25)

- Equations were written as `\[ … \]` and `\( … \)`. GitHub and the VS Code preview don't render those, so they showed as raw `[ e_z=z_d-z … ]` text. All 147 equations in 10 docs are converted to `$$ … $$` / `$ … $`, and each one was checked to compile with KaTeX.
- Added the blank lines around lists, headings and code fences that stricter Markdown renderers need. No text changed.
- When writing new equations, use `$…$` inline and `$$` on its own line for display maths.

### Firmware baseline (2026-10-04)

- Added the layered ECU firmware in `firmware/`. It is AUTOSAR-inspired: MCAL API → IoHwAb → services (Os, SchM, EcuM, BswM, Det, Dem, WdgM, Log) → hand-written RTE → SWCs (VehicleMode, StatusLed, BenchPanel, Lights).
- Added the hierarchical state machine engine `lib/Fsm`. The VehicleMode state machine implements the operating modes from the system overview, with arming guards, fault reactions and a latched E-STOP.
- Added FreeRTOS V11.3.1 and Unity v2.7.0 as pinned submodules. CMake presets for the host (unit tests and a FreeRTOS POSIX simulation) and the NUCLEO-F412ZG.
- Added unit tests for every lib module, service with logic, and SWC, plus a SIL smoke test of the full mode sequence and a layering checker. CI added.
- Moved the CubeMX project to `firmware/port/stm32f412/cubemx/Nereid-ROV.ioc`, switched the toolchain from IAR EWARM to CMake/GCC, and removed the EWARM files.
- Fixed the spelling "Neried" → "Nereid".
- Removed the `software/drivers/` and `software/onboard/*` embedded placeholders; firmware owns ECU code (ADR-008).
- New docs:
  - firmware architecture, state machines, coding standard, ECU resource allocation, roadmap
  - ADR-004 (RTOS), ADR-005 (layering), ADR-006 (state machine engine), ADR-007 (toolchain), ADR-008 (repository layout)
- ADR-001 … 003 accepted.

- Created complete documentation-first mini ROV repository scaffold.
