# Nereid ECU firmware

Layered (AUTOSAR-inspired) firmware for the ROV's vehicle ECU, a NUCLEO-F412ZG running FreeRTOS.
Start with [`docs/software/firmware_architecture.md`](../docs/software/firmware_architecture.md);
the state machines are specified in [`docs/software/state_machines.md`](../docs/software/state_machines.md).

```
app/        L4 application SWCs (VehicleMode, StatusLed, BenchPanel, Lights)
rte/        L3 hand-written RTE: SWC ports, signal buffers, task bodies
bsw/        L2 services (Os, SchM, EcuM, BswM, Det, Dem, WdgM, Log)
            L1 ECU abstraction (IoHwAb)   L0 MCAL API headers (Mcu, Dio, Gpt, Uart, Wdg)
lib/        platform-independent: Fsm (state machine engine), RingBuf, Crc
common/     Std_Types.h, ModuleIds.h
port/       MCAL implementations: stm32f412/ (ST HAL + CubeMX), host/ (Linux, FreeRTOS POSIX)
variants/   per-vehicle configuration and integration (nereid_v1)
tests/      host unit tests (Unity) + SIL smoke test
tools/      check_layers.py
third_party/ FreeRTOS-Kernel V11.3.1, Unity v2.7.0 (git submodules)
```

## First checkout

```bash
git submodule update --init --recursive
```

## Host build and tests (no hardware needed)

Needs `cmake` (≥ 3.22), `ninja` and a host GCC.

```bash
cd firmware
cmake --preset host
cmake --build --preset host
ctest --preset host
python3 tools/check_layers.py
```

Run the whole ECU on Linux with a simulated button:

```bash
./build/host/nereid_host --script "w500 s w400 s w400 l w400 s w600 q"
```

Script tokens: `w<ms>` wait, `s` short press, `l` long press, `p<ms>` press, `q` quit. With
no `--script`, tokens are read from stdin. Set `NEREID_TRACE_DIO=1` to print LED changes.

## Target build (NUCLEO-F412ZG)

1. Install the cross toolchain (Arch / CachyOS):
   ```bash
   sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib cmake ninja
   ```
2. Do the **one-time CubeMX regeneration** described in
   [`port/stm32f412/cubemx/README.md`](port/stm32f412/cubemx/README.md). It switches the HAL time base to TIM6 and hands the SysTick, SVC and PendSV handlers to FreeRTOS.
3. Build:
   ```bash
   cmake --preset f412-debug
   cmake --build --preset f412-debug
   ```
   This produces `build/f412-debug/nereid_ecu.elf`, `.hex`, `.bin` and `.map`.
4. Flash with STM32CubeProgrammer, or with OpenOCD:
   ```bash
   openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/f412-debug/nereid_ecu.elf verify reset exit"
   ```
5. Open the ST-LINK virtual COM port at 115200 8N1 to see the log. On the board:
   - **Green LED**: shows the mode.
   - **Red LED**: shows faults.
   - **User button**: a short press arms/disarms/resets; holding it for 1 s triggers an E-STOP.

## Adding things

| To add | Do this |
|---|---|
| A new ROV | Copy `variants/nereid_v1`, then configure with `-DNEREID_VARIANT=<name>` |
| A new MCU | Add `port/<mcu>/` implementing `bsw/mcal/include/*.h` (architecture doc §9.2) |
| A new SWC | Create `app/<Swc>/{include,src}`, add `rte/include/Rte_<Swc>.h`, its RTE functions and runnable call in `rte/src/Rte.c`, its name in `NEREID_SWCS` in `CMakeLists.txt`, and a unit test |
