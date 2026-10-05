# Roadmap

A solo, firmware-first plan. Each milestone has entry and exit criteria so progress stays
measurable. Dates are deliberately absent. Update the **Status** column when a milestone closes.

| # | Milestone | Status |
|---|---|---|
| M0 | Foundations: toolchain, builds, CI, blinky through all layers | host part **done**; target bring-up pending CubeMX regeneration |
| M1 | Mode management: Fsm, EcuM, VehicleMode, Dem, WdgM/IWDG | **done on host** (unit + SIL smoke tests); verify on board |
| M2 | Communication: Com protocol, link supervision, topside test tool | next |
| M3 | Actuation: PWM, ESC driver, thrust allocation (props off) | – |
| M4 | Sensing: IMU + pressure drivers, estimation | – |
| M5 | Closed loop: depth/heading hold, SIL against the Python plant | – |
| M6 | Power and safety: battery, leak, current, full fault matrix, NvM | – |
| M7 | HIL, bench, restrained wet, pool tests | – |
| M8 | Companion computer, station keeping, AI | – |

## M0 — Foundations

**Exit criteria:**
- [x] `firmware/` layered skeleton, CMake presets, FreeRTOS + Unity submodules pinned
- [x] Host build, unit tests and layering check in CI (`.github/workflows/firmware.yml`)
- [ ] Install the toolchain: `sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib cmake ninja openocd`
- [ ] Install STM32CubeMX ≥ 6.18 (download from st.com, needs an ST account; not installed on the dev machine as of 2026-10-05)
- [ ] Regenerate the CubeMX project with the 8 settings in `firmware/port/stm32f412/cubemx/README.md`, review `git diff`, commit
- [ ] `cmake --preset f412-debug` builds; size report recorded in `ecu_resource_allocation.md`
- [ ] Flashed: green LED pattern visible, boot banner on the ST-LINK VCP
- [ ] Remove `continue-on-error` from the CI target job

## M1 — Mode management

**Exit criteria:**
- [x] `lib/Fsm` HSM engine with tests
- [x] VehicleMode with every transition, guard and invariant tested (`state_machines.md`)
- [x] Dem classes, WdgM alive supervision, watchdog reset verified on host
- [ ] On board: button ARM / DISARM / long-press E-STOP / RESET, matching LED patterns
- [ ] On board: stall the control task (temporary busy loop in `Rte_Task_Control10ms`) → IWDG reset → boot banner shows `reset reason WATCHDOG`

## M2 — Communication

**Scope:**
- **Protocol** in `software/protocols/`: a versioned frame with type, sequence, timestamp, length, payload and CRC-16, per `software_architecture.md` §3. COBS framing.
- **ECU side**: `ComIf` (byte transport over Uart) → `Com` (encode/decode) → RTE queues. `ComM` link state machine (DISCONNECTED / CONNECTED / STALE).
- **Topside test tool** (`software/topside/`, Python): keyboard or gamepad input, live mode and fault display.
- **Wiring the guards**: heartbeat timeout becomes Dem `COMM_LOST` (FAILSAFE class). This replaces the `commandFresh` and `pilotNeutral` stubs.

**Exit criteria:**
- Command loss → FAILSAFE within the configured timeout. This is a tested requirement, SAF-COMM-001.
- Telemetry carries the mode, fault class, Dem events, task timing (`SchM_GetTaskStats`) and the arm-reject mask.

**Decide before starting** (ADR-009): the physical tether link, and whether to use a custom protocol or MAVLink. MAVLink brings QGroundControl/BlueOS compatibility, at the cost of complexity.

## M3 — Actuation (propellers removed)

- Pwm MCAL. `Esc` driver state machine: DISABLED → ARMING (neutral) → ACTIVE. Neutral at boot. Any loss of `PropulsionEnable` forces DISABLED.
- `ThrustAlloc` SWC: geometry matrix B from variant config, pseudo-inverse, saturation, achieved wrench (`thruster_allocation.md`).
- Authority limit applied after allocation.
- **Exit criteria:** a bench test with props off shows correct channel identity and sign for every thruster, and the logic analyser shows neutral pulses whenever the vehicle is not ARMED.

## M4 — Sensing and estimation

- I2C/SPI with DMA, IMU and pressure drivers with INIT/RUN/FAULT state machines, health monitors feeding Dem.
- `StateEst`: depth from pressure (with surface datum), attitude from a complementary filter (`state_estimation.md`). Replaces the `estimatorValid` stub.

## M5 — Closed loop

- `lib/Pid` (derivative on measurement, anti-windup, per `control_architecture.md`). `DepthCtrl` and `HeadingCtrl` SWCs in ARMED_ASSISTED.
- **SIL**: the host build talks to the Python plant (`models/python`) over a socket MCAL port. The same binary logic runs as on target.

## M6 — Power, leak and safety completion

- PowerMon (battery voltage and current), LeakMon, the physical kill status input, the full Dem table reviewed against `safety_case.md` §4.
- NvM parameters generated from `config/` (ADR-002), with configuration validation feeding `DEM_EVENT_CONFIG_INVALID`.
- **Exit criteria:** every safety test in `test_plan.md` §5 has passed on the bench.

## M7 — Integration testing

HIL rig, then the procedures in `operations/test_and_dive_procedure.md`: dry bench, restrained wet, pool.

## M8 — Companion computer and AI

- Companion computer (e.g. Raspberry Pi 5 or Jetson) for video, a DVL/vision-aided estimator and an ML station-keeping policy. It sends references or wrench setpoints through Com and is supervised by the ECU like any other command source (`firmware_architecture.md` §10).
- Optional on-ECU `AiCtrl` SWC (TFLite Micro / CMSIS-NN) behind the controller interface, with PID fallback.

## Open decisions (track as ADRs)

| Topic | Needed by | Notes |
|---|---|---|
| Tether physical link (RS-485 UART, Ethernet over Fathom-X style powerline, or companion computer) | M2 | Drives the `ComIf` transport and the `Uart_Cfg`. |
| Protocol: custom vs MAVLink | M2 | MAVLink brings ecosystem tools; custom keeps it small and tailored. |
| Thrusters + ESCs (count, layout, PWM vs DShot) | M3 | Defines the allocation matrix and variant config. |
| IMU and pressure sensor parts | M4 | Bus choice (SPI vs I2C), driver effort. |
| Companion computer and station-keeping sensor (DVL/USBL/vision) | M8 | Station keeping isn't observable with IMU + pressure alone. |
| License | before publishing | `LICENSE` is a placeholder. |
