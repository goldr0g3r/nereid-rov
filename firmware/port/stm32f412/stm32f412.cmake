# STM32F412ZG (NUCLEO-F412ZG) target build. Included from firmware/CMakeLists.txt.
# Requires the one-time CubeMX regeneration described in cubemx/README.md.
set(PORT_DIR   ${FW}/port/stm32f412)
set(CUBEMX_DIR ${PORT_DIR}/cubemx)

if(NOT EXISTS ${CUBEMX_DIR}/Core/Src/stm32f4xx_hal_timebase_tim.c)
    message(WARNING
        "CubeMX output still uses SysTick as HAL time base. Regenerate the project as described in "
        "${CUBEMX_DIR}/README.md, otherwise SysTick/SVC/PendSV handlers clash with FreeRTOS at link time.")
endif()

# ---- vendor: CubeMX init code + ST HAL + CMSIS start-up ---------------------------
add_library(stm32_hal_api INTERFACE)
target_compile_definitions(stm32_hal_api INTERFACE USE_HAL_DRIVER STM32F412Zx $<$<CONFIG:Debug>:DEBUG>)
target_include_directories(stm32_hal_api SYSTEM INTERFACE
    ${CUBEMX_DIR}/Core/Inc
    ${CUBEMX_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CUBEMX_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CUBEMX_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CUBEMX_DIR}/Drivers/CMSIS/Include)

file(GLOB CUBEMX_CORE_SOURCES CONFIGURE_DEPENDS ${CUBEMX_DIR}/Core/Src/*.c)
file(GLOB STM32_HAL_SOURCES CONFIGURE_DEPENDS ${CUBEMX_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/*.c)
list(FILTER STM32_HAL_SOURCES EXCLUDE REGEX "_template\\.c$")

if(EXISTS ${CUBEMX_DIR}/startup_stm32f412zx.s)
    set(STARTUP_FILE ${CUBEMX_DIR}/startup_stm32f412zx.s)
else()
    set(STARTUP_FILE ${CUBEMX_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f412zx.s)
endif()

add_library(cubemx OBJECT ${CUBEMX_CORE_SOURCES} ${STM32_HAL_SOURCES} ${STARTUP_FILE})
target_link_libraries(cubemx PUBLIC stm32_hal_api)
# main.c calls EcuM_Init(): give it exactly that header, nothing else.
target_include_directories(cubemx PRIVATE ${FW}/bsw/services/EcuM/include ${FW}/common/include)

# ---- MCAL implementation + variant pin tables ------------------------------------
file(GLOB PORT_SOURCES CONFIGURE_DEPENDS ${PORT_DIR}/src/*.c)
file(GLOB VARIANT_MCU_SOURCES CONFIGURE_DEPENDS ${VARIANT_DIR}/stm32f412/*.c)
nereid_add_module(port_stm32f412
    SOURCES  ${PORT_SOURCES} ${VARIANT_MCU_SOURCES}
    INCLUDES ${PORT_DIR}/include
    DEPS     mcal_api lib_ringbuf stm32_hal_api)

# ---- firmware image --------------------------------------------------------------
set(LINKER_SCRIPT ${PORT_DIR}/STM32F412ZGTx_FLASH.ld)
add_executable(nereid_ecu)
set_target_properties(nereid_ecu PROPERTIES SUFFIX ".elf" LINK_DEPENDS ${LINKER_SCRIPT})
target_link_libraries(nereid_ecu PRIVATE ${NEREID_PORTABLE_OBJECTS} port_stm32f412 cubemx freertos_kernel)
target_link_options(nereid_ecu PRIVATE -T${LINKER_SCRIPT} -Wl,-Map=$<TARGET_FILE_DIR:nereid_ecu>/nereid_ecu.map)

add_custom_command(TARGET nereid_ecu POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:nereid_ecu> $<TARGET_FILE_DIR:nereid_ecu>/nereid_ecu.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:nereid_ecu> $<TARGET_FILE_DIR:nereid_ecu>/nereid_ecu.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:nereid_ecu>
    COMMENT "Generating hex/bin and size report")
