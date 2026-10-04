# ADR-007: Firmware toolchain

- Status: Accepted
- Date: 2026-10-04

## Context

The CubeMX project was generated for IAR EWARM (Windows, licensed) while the developer works on Linux. The firmware must also build for the host, for tests and SIL.

## Decision

- **Build system**: CMake (≥ 3.22) with presets `host`, `f412-debug` and `f412-release`, using the Ninja generator.
- **Compilers**: `arm-none-eabi-gcc` with newlib-nano for the target; the system GCC for the host.
- **STM32CubeMX** stays the tool for clock, pinout and peripheral init (`.ioc`, toolchain set to CMake, HAL copied as "only necessary files"). Its generated build files are not used. `firmware/CMakeLists.txt` compiles `Core/` and the HAL directly.
- **Tests**: Unity (submodule, pinned) and CTest. CI on GitHub Actions.
- **Flashing and debugging**: STM32CubeProgrammer, or OpenOCD / probe-rs with the on-board ST-LINK. Any IDE that reads `compile_commands.json` works, e.g. VS Code with the CMake Tools and Cortex-Debug extensions.

## Consequences

- One build definition serves CI, the command line and IDEs.
- The CubeMX regeneration is a manual, documented step (`firmware/port/stm32f412/cubemx/README.md`). Its output is committed and reviewed with `git diff`.
