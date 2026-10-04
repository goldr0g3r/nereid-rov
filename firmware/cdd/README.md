# Complex device drivers (CDD)

In AUTOSAR terms a CDD is a driver that needs direct hardware access *and* timing behaviour
that does not fit the MCAL → ECU abstraction stack, and so spans several layers. Candidates
for this ROV:

- **DShot ESC protocol**: timer + DMA bit-banging with µs timing, if ESCs move from PWM to DShot.
- **Sonar / DVL interfaces** with strict trigger timing.

A CDD exposes a normal `<Module>.h` API to services and the RTE, and keeps its MCU-specific
parts in `port/<mcu>/`. Nothing lives here yet.
