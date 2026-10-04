# ECU Resource Allocation — NUCLEO-F412ZG

Status: **proposal**. The only hardware owned today is the Nucleo board. Everything marked
*planned* must be checked in STM32CubeMX before wiring. CubeMX reports pin and alternate-function
conflicts. Also cross-check against UM1974 (the STM32 Nucleo-144 user manual) and DS11139
(the STM32F412xE/G datasheet). If a row turns out to be wrong, change this file in the same
commit as the `.ioc`.

MCU: STM32F412ZGT6 (LQFP144), Cortex-M4F. SYSCLK 96 MHz from the 8 MHz ST-LINK MCO
(HSE bypass) and the PLL. APB1 48 MHz (timers 96 MHz), APB2 96 MHz.

## In use (M0/M1)

| Function | Peripheral | Pin | Nucleo label | Owner |
|---|---|---|---|---|
| Status LED (green) | GPIO out | PB0 | LD1 | `DIO_CH_LED_GREEN` → `IOHWAB_OUT_STATUS_LED` |
| Lights stand-in (blue) | GPIO out | PB7 | LD2 | `DIO_CH_LED_BLUE` → `IOHWAB_OUT_LIGHTS` |
| Fault LED (red) | GPIO out | PB14 | LD3 | `DIO_CH_LED_RED` → `IOHWAB_OUT_FAULT_LED` |
| User button | GPIO in | PC13 | B1 | `DIO_CH_USER_BUTTON` → `IOHWAB_IN_USER_BUTTON` |
| Debug console / log | USART3 115200 8N1 | PD8 TX / PD9 RX | ST-LINK VCP | `UART_CH_DEBUG` |
| µs time base | TIM2 (32-bit) | – | – | `Gpt` |
| HAL time base | TIM6 | – | – | CubeMX (after regeneration) |
| RTOS tick | SysTick | – | – | FreeRTOS |
| Watchdog | IWDG (LSI) | – | – | `Wdg` / `WdgM` |
| Debug | SWD | PA13 / PA14 / PB3 (SWO) | ST-LINK | – |

## Planned

| Function | Peripheral | Proposed pins | Notes | Milestone |
|---|---|---|---|---|
| Thrusters 1–4 | TIM1 CH1–CH4 PWM | PE9, PE11, PE13, PE14 | Servo-style ESC PWM at 50–400 Hz, 1100–1900 µs (typical for Blue Robotics Basic ESC). DShot would be a CDD using timer DMA. | M3 |
| Thrusters 5–8 | TIM3 CH1–CH4 PWM | PC6, PC7, PC8, PC9 | Needed only for 6+ thruster layouts | M3 |
| Lights (dimmable) | TIM4 CH1 PWM | PD12 | Replaces the blue-LED stand-in | M3 |
| Pressure / depth sensor | I2C1 | PB8 SCL / PB9 SDA (Arduino D15/D14) | e.g. MS5837 (Bar02/Bar30), fixed address 0x76. Use an I2C buffer or extender over long cables. | M4 |
| IMU | SPI1 (preferred) or I2C2 | SPI1: PA5/PA6/PA7 + CS GPIO; I2C2: PF1 SCL / PF0 SDA | SPI for ≥ 200 Hz without bus contention. Keep away from motor wiring for the magnetometer (hardware_design.md §5). | M4 |
| Tether link | USART2 | PD5 TX / PD6 RX, PD4 as RS-485 DE (GPIO) | The F4 USART has no hardware DE pin, so drive DE from Com's TX-complete. Link type is undecided (roadmap open decisions). | M2 |
| Companion computer | USART6 | PG14 TX / PG9 RX (Arduino D1/D0) | Or USB CDC if the USB stack is enabled later | M8 |
| Battery voltage | ADC1 | PA3 (A0) | Resistor divider into 0–3.3 V with RC filter | M6 |
| Battery current | ADC1 | PC0 (A1) | Hall or shunt amplifier | M6 |
| Leak sensor(s) | GPIO in + EXTI | PF13 (proposed) | Debounced in IoHwAb, Dem class EMERGENCY or FAILSAFE (safety review) | M6 |
| Hardware e-stop / kill status | GPIO in | PF14 (proposed) | Reads the state of the **independent** physical kill. The kill itself must not depend on software. | M6 |
| Parameter storage | Internal flash, last sector(s) | – | NvM with CRC and two copies | M6 |
| Logging | SDIO / SPI SD card | TBD | Optional; telemetry may be enough | later |

## Budget notes

- **Flash/RAM:** M1 firmware is small (well under 10 % of 1 MB / 256 KB, to be confirmed by the first `size` report). The FreeRTOS stack pool is 16 KB (`OS_STACK_POOL_WORDS`), and the log buffer is 2 KB.
- **Timers:** TIM2 (Gpt) and TIM6 (HAL) are reserved. TIM1, TIM3 and TIM4 are planned for PWM. TIM5 (32-bit) and TIM9–TIM14 are free.
- **DMA:** reserve DMA for the I2C/SPI sensor reads (M4) and tether UART RX (M2). Check stream/channel conflicts in CubeMX.
