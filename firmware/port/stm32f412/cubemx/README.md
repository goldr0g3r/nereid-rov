# STM32CubeMX project (NUCLEO-F412ZG)

`Nereid-ROV.ioc` owns **clock tree, pin multiplexing and peripheral init code**. In AUTOSAR terms
that is the Mcu + Port configuration. Everything else (drivers, services, application) lives in
the layered firmware and must not be written into CubeMX files, except the two hooks in
`USER CODE` blocks:

| File | Hook |
|---|---|
| `Core/Src/main.c` | `EcuM_Init();` in `USER CODE BEGIN 2`. It hands control to the layered software and never returns. |
| `Core/Src/stm32f4xx_it.c` | `NVIC_SystemReset();` in `USER CODE BEGIN HardFault_IRQn 0`. After a reset every output is in its safe state. |

CubeMX keeps `USER CODE` blocks when it regenerates.

## One-time regeneration (required before the first target build)

The project was created for IAR EWARM with SysTick as the HAL time base, which conflicts with
FreeRTOS. The `.ioc` already says `Toolchain = CMake` and "copy only necessary library files".
Open it in **STM32CubeMX 6.18+** and change the following:

1. **System Core → SYS → Timebase Source: `TIM6`**. FreeRTOS needs SysTick.
2. **System Core → NVIC → Code generation tab**: untick *Generate IRQ handler* for
   - System service call via SWI instruction (SVC)
   - Pendable request for system service (PendSV)
   - System tick timer (SysTick)

   FreeRTOS provides these three handlers.
3. **NVIC**: set *Priority Group* to 4 bits pre-emption (already the case). Leave USART3
   global interrupt **disabled**, because `port/stm32f412/src/Uart_Stm32.c` owns
   `USART3_IRQHandler`.
4. **Connectivity → USB_OTG_FS: Disable**. No USB stack is planned for the ECU.
5. **PC13 (USER_Btn)**: change from `GPIO_EXTI13` to **`GPIO_Input`**. The button is polled
   and debounced by IoHwAb.
6. **Middleware → FREERTOS: leave disabled**. The kernel comes from
   `firmware/third_party/FreeRTOS-Kernel` (a pinned submodule).
7. **Project Manager → Code Generator**: tick *Generate peripheral initialization as a
   pair of .c/.h files per peripheral*. Keep *Keep User Code when re-generating* ticked.
8. Click **GENERATE CODE**.

After generating:

```bash
git status firmware/port/stm32f412/cubemx    # review what CubeMX changed
```

CubeMX's own `CMakeLists.txt` / `cmake/` files are not used. `firmware/CMakeLists.txt` compiles
`Core/Src/*.c` and the HAL sources directly, so generated build files can be deleted or
ignored. Once the HAL is copied as "only necessary files", the unused CMSIS folders
(`Core_A`, `DAP`, `DSP`, `NN`, `RTOS*`, `Documentation`) can be removed with `git rm`. Bring
DSP/NN back when on-board ML is started (roadmap M8).
