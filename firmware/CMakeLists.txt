# Nereid ROV ECU firmware.
#
#   cmake --preset host        && cmake --build --preset host && ctest --preset host
#   cmake --preset f412-debug  && cmake --build --preset f412-debug
#
# Layering is enforced by target usage requirements: every module is an OBJECT
# library that only sees the include directories of the layers it may use
# (docs/software/firmware_architecture.md). tools/check_layers.py double-checks it.
cmake_minimum_required(VERSION 3.22)

set(NEREID_TARGET "host" CACHE STRING "Build target: host | stm32f412")
set_property(CACHE NEREID_TARGET PROPERTY STRINGS host stm32f412)
set(NEREID_VARIANT "nereid_v1" CACHE STRING "Vehicle variant (directory under variants/)")
option(NEREID_WERROR "Treat warnings in project code as errors" ON)

project(nereid_firmware VERSION 0.1.0 LANGUAGES C)
if(NEREID_TARGET STREQUAL "stm32f412")
    enable_language(ASM)
elseif(NOT NEREID_TARGET STREQUAL "host")
    message(FATAL_ERROR "Unknown NEREID_TARGET '${NEREID_TARGET}'")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) # gnu11: needed by the FreeRTOS POSIX port and vendor code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FW ${CMAKE_CURRENT_SOURCE_DIR})
set(VARIANT_DIR ${FW}/variants/${NEREID_VARIANT})
if(NOT EXISTS ${VARIANT_DIR}/include)
    message(FATAL_ERROR "Variant '${NEREID_VARIANT}' not found in ${FW}/variants")
endif()

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
add_library(nereid_warnings INTERFACE)
target_compile_options(nereid_warnings INTERFACE
    -Wall -Wextra -Wshadow -Wundef -Wdouble-promotion -Wstrict-prototypes
    $<$<BOOL:${NEREID_WERROR}>:-Werror>)

# nereid_add_module(<name> SOURCES ... INCLUDES ... DEPS ...)
function(nereid_add_module NAME)
    cmake_parse_arguments(M "" "" "SOURCES;INCLUDES;DEPS" ${ARGN})
    add_library(${NAME} OBJECT ${M_SOURCES})
    target_include_directories(${NAME} PUBLIC ${M_INCLUDES})
    target_link_libraries(${NAME} PUBLIC ${M_DEPS} PRIVATE nereid_warnings)
endfunction()

# ---------------------------------------------------------------------------
# Common, libraries (platform independent)
# ---------------------------------------------------------------------------
add_library(nereid_common INTERFACE)
target_include_directories(nereid_common INTERFACE ${FW}/common/include)

add_library(nereid_variant_api INTERFACE)
target_include_directories(nereid_variant_api INTERFACE ${VARIANT_DIR}/include)
target_link_libraries(nereid_variant_api INTERFACE nereid_common)

nereid_add_module(lib_fsm     SOURCES ${FW}/lib/Fsm/src/Fsm.c         INCLUDES ${FW}/lib/Fsm/include     DEPS nereid_common)
nereid_add_module(lib_ringbuf SOURCES ${FW}/lib/RingBuf/src/RingBuf.c INCLUDES ${FW}/lib/RingBuf/include DEPS nereid_common)
nereid_add_module(lib_crc     SOURCES ${FW}/lib/Crc/src/Crc.c         INCLUDES ${FW}/lib/Crc/include     DEPS nereid_common)

# ---------------------------------------------------------------------------
# FreeRTOS kernel (only Os.c may include its headers)
# ---------------------------------------------------------------------------
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
    ${FW}/port/${NEREID_TARGET}/include
    ${FW}/bsw/services/Os/config)
if(NEREID_TARGET STREQUAL "host")
    set(FREERTOS_PORT GCC_POSIX CACHE STRING "" FORCE)
else()
    set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "" FORCE)
endif()
add_subdirectory(${FW}/third_party/FreeRTOS-Kernel EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# L0 MCAL API (implementations live in port/<target>)
# ---------------------------------------------------------------------------
add_library(mcal_api INTERFACE)
target_include_directories(mcal_api INTERFACE ${FW}/bsw/mcal/include)
target_link_libraries(mcal_api INTERFACE nereid_variant_api)

# ---------------------------------------------------------------------------
# L1 ECU abstraction + L2 services (one object set: they call each other through
# cross-cutting services such as Det/Dem/Log; intra-BSW rules are checked by
# tools/check_layers.py)
# ---------------------------------------------------------------------------
set(BSW_MODULES Os SchM EcuM BswM Det Dem WdgM Log)
set(BSW_SOURCES ${FW}/bsw/ecual/IoHwAb/src/IoHwAb.c)
set(BSW_INCLUDES ${FW}/bsw/ecual/IoHwAb/include)
foreach(m IN LISTS BSW_MODULES)
    list(APPEND BSW_SOURCES ${FW}/bsw/services/${m}/src/${m}.c)
    list(APPEND BSW_INCLUDES ${FW}/bsw/services/${m}/include)
endforeach()
nereid_add_module(bsw
    SOURCES  ${BSW_SOURCES}
    INCLUDES ${BSW_INCLUDES}
    DEPS     mcal_api lib_fsm lib_ringbuf lib_crc)
target_link_libraries(bsw PRIVATE freertos_kernel)

# ---------------------------------------------------------------------------
# L4 application SWCs: see ONLY the RTE API, lib/ and common/
# ---------------------------------------------------------------------------
add_library(rte_api INTERFACE)
target_include_directories(rte_api INTERFACE ${FW}/rte/include)
target_link_libraries(rte_api INTERFACE nereid_common)

set(NEREID_SWCS VehicleMode StatusLed BenchPanel Lights)
set(SWC_TARGETS)
foreach(swc IN LISTS NEREID_SWCS)
    string(TOLOWER ${swc} swc_lc)
    nereid_add_module(swc_${swc_lc}
        SOURCES  ${FW}/app/${swc}/src/${swc}.c
        INCLUDES ${FW}/app/${swc}/include
        DEPS     rte_api lib_fsm)
    list(APPEND SWC_TARGETS swc_${swc_lc})
endforeach()

# ---------------------------------------------------------------------------
# L3 RTE and variant integration
# ---------------------------------------------------------------------------
nereid_add_module(rte SOURCES ${FW}/rte/src/Rte.c DEPS rte_api bsw ${SWC_TARGETS})

file(GLOB VARIANT_SOURCES CONFIGURE_DEPENDS ${VARIANT_DIR}/src/*.c)
nereid_add_module(variant SOURCES ${VARIANT_SOURCES} DEPS bsw rte ${SWC_TARGETS})

set(NEREID_PORTABLE_OBJECTS lib_fsm lib_ringbuf lib_crc bsw rte variant ${SWC_TARGETS})

# ---------------------------------------------------------------------------
# Target specific
# ---------------------------------------------------------------------------
if(NEREID_TARGET STREQUAL "host")
    find_package(Threads REQUIRED)

    nereid_add_module(port_host
        SOURCES  ${FW}/port/host/src/Mcu_Host.c ${FW}/port/host/src/Dio_Host.c ${FW}/port/host/src/Gpt_Host.c
                 ${FW}/port/host/src/Uart_Host.c ${FW}/port/host/src/Wdg_Host.c
        INCLUDES ${FW}/port/host/include
        DEPS     mcal_api lib_ringbuf)

    add_executable(nereid_host ${FW}/port/host/src/main_host.c)
    target_link_libraries(nereid_host PRIVATE ${NEREID_PORTABLE_OBJECTS} port_host freertos_kernel
                                              Threads::Threads nereid_warnings)

    enable_testing()
    add_subdirectory(third_party/Unity EXCLUDE_FROM_ALL)
    add_subdirectory(tests)
else()
    include(${FW}/port/stm32f412/stm32f412.cmake)
endif()
