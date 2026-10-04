# Firmware Coding Standard

This is a pragmatic subset inspired by **MISRA C:2012** and AUTOSAR C conventions, sized for
one developer. Every rule exists to keep the firmware deterministic, portable and testable.

## Language and build

- **C11** (`gnu11`, because vendor code and the FreeRTOS POSIX port need GNU extensions). No C++ in the ECU firmware.
- All project code compiles warning-free with `-Wall -Wextra -Wshadow -Wundef -Wdouble-promotion -Wstrict-prototypes -Werror` on both GCC targets.
- Use fixed-width types from `<stdint.h>` and `bool` from `<stdbool.h>`, via `Std_Types.h`. Use `float` (single precision, which the M4F has in hardware). Never `double` in target code; `-Wdouble-promotion` catches it.
- Format with `firmware/.clang-format`. Vendor and generated code are excluded.

## Memory and execution

- **No dynamic allocation.** No `malloc`/`free`, and FreeRTOS uses static allocation only (`configSUPPORT_DYNAMIC_ALLOCATION 0`). Every buffer has a fixed size known at link time.
- **No recursion.** Loops have a bound you can see in the code, for example `VEHMODE_MAX_REQUESTS_PER_CYCLE` or `FSM_MAX_DEPTH`.
- **No blocking in control-path runnables.** Blocking I/O (the UART TX) happens only in the lowest-priority task.
- **ISRs** only move data into ring buffers or set flags. No logging, no floating point, no HAL calls that can time out.
- Shared data between tasks goes through the RTE or a module API using `Os_EnterCritical/Os_ExitCritical`. No unprotected global variables shared between tasks.

## Structure and naming

- One module = one directory with `include/` (public API) and `src/`. Everything not in the public header is `static`.
- Public symbols are prefixed with the module name: `Dem_SetEventStatus`, `IoHwAb_Init`, `DEM_EVENT_*`. Types end in `Type`.
- Configuration goes in `<Module>_Cfg.h/.c` inside the variant. A module never hard-codes vehicle-specific numbers.
- Respect the layer rules (`firmware_architecture.md` §2.1). Run `python3 firmware/tools/check_layers.py`.

## Robustness

- Check every pointer and index parameter at the public API. Report programming errors to **Det**, operational problems to **Dem**.
- Check return values, or discard them explicitly with a `(void)` cast.
- Every `switch` has a `default`. Every `if … else if` chain ends with `else` where the remaining case matters.
- When a read can fail, assume the safe value. For example, VehicleMode treats an unreadable fault class as EMERGENCY.
- Integer time comparisons use unsigned subtraction (`Gpt_ElapsedUs`), so they stay correct across wrap-around.

## Testing

- Every module in `lib/`, every SWC, and every service with logic (Dem, WdgM) has a host unit test in `firmware/tests/unit`.
- SWCs are tested against a fake RTE written in the test file.
- A behaviour change means updating the state machine document (`state_machines.md`) and its tests in the same commit.

## Static analysis (recommended, not yet in CI)

```bash
cppcheck --enable=warning,style,performance --addon=misra --std=c11 -I firmware/common/include firmware/app firmware/bsw firmware/lib firmware/rte
clang-tidy -p firmware/build/host <file.c>
```
