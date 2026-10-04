# Companion-computer software (future, roadmap M8)

Linux software running **on the vehicle** next to the STM32 ECU: video, sensor fusion with
DVL/vision, ML-based station keeping, mission logic. It talks to the ECU through the tether
protocol in `software/protocols/`, and the ECU supervises it like any other command source
(see `docs/software/firmware_architecture.md` §10).

ECU firmware is **not** here: it lives in `firmware/` (ADR-008).
