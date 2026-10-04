# ADR-004: Real-time operating system

- Status: Accepted
- Date: 2026-10-04

## Context

The ECU needs deterministic periodic tasks (10 ms control, 20 ms communication, 100 ms housekeeping), a way to test on a PC, and room to grow (communication stack, sensor drivers, possibly on-board ML). The developer wants AUTOSAR-style layering and builds it alone. Candidates: bare-metal cyclic executive, FreeRTOS, Zephyr.

## Decision

Use **FreeRTOS kernel V11** (git submodule `firmware/third_party/FreeRTOS-Kernel`, pinned tag) behind a thin OS abstraction (`bsw/services/Os`).

- Static allocation only. Strictly periodic tasks created from a variant table. No dynamic heap.
- Target port `GCC_ARM_CM4F`. Host port `GCC_POSIX`, so the complete ECU runs on Linux for smoke tests now and SIL later.
- Only `Os.c` includes FreeRTOS headers (enforced by `tools/check_layers.py`).

## Alternatives

- **Zephyr.** Strong portability through devicetree, built-in state machine framework (SMF), zbus message bus, logging, settings, `native_sim`, and a TFLite Micro module. Rejected *for now*: Zephyr's device model and subsystems would replace most of the hand-built MCAL / ECU abstraction / RTE layers that are a stated goal of the project, and the learning curve competes with building the ROV. The Os abstraction keeps a later migration contained.
- **Bare-metal cyclic executive.** Simplest and fully deterministic, but blocking communication and future driver state machines would force hand-made cooperative scheduling.
- **CubeMX's FreeRTOS middleware (CMSIS-RTOS2 wrapper).** Rejected to keep the kernel version pinned independently of CubeMX and to avoid CubeMX owning task code.

## Consequences

- The HAL time base moves from SysTick to TIM6. FreeRTOS owns the SVC, PendSV and SysTick handlers (CubeMX regeneration step).
- Interrupts that call FreeRTOS APIs must have priority ≥ `configMAX_SYSCALL_INTERRUPT_PRIORITY` (5).
- AI models do not drive this choice. Heavy models run on a companion computer, and small models (TFLite Micro / CMSIS-NN) run under either RTOS.
