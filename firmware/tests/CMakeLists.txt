# Host unit tests (Unity + CTest). Each test compiles the unit under test from source
# together with stubs for its dependencies, so every module is tested in isolation.
set(STUBS ${CMAKE_CURRENT_SOURCE_DIR}/stubs)

set(INC_COMMON   ${FW}/common/include ${VARIANT_DIR}/include)
set(INC_FSM      ${FW}/lib/Fsm/include)
set(INC_MCAL     ${FW}/bsw/mcal/include)
set(INC_SERVICES ${FW}/bsw/services/Os/include ${FW}/bsw/services/Log/include ${FW}/bsw/services/Dem/include
                 ${FW}/bsw/services/Det/include ${FW}/bsw/services/WdgM/include)
set(INC_RTE      ${FW}/rte/include)

# nereid_add_unit_test(<name> SOURCES ... INCLUDES ...)
function(nereid_add_unit_test NAME)
    cmake_parse_arguments(T "" "" "SOURCES;INCLUDES" ${ARGN})
    add_executable(${NAME} unit/${NAME}.c ${T_SOURCES})
    target_include_directories(${NAME} PRIVATE ${INC_COMMON} ${T_INCLUDES})
    target_link_libraries(${NAME} PRIVATE unity nereid_warnings)
    add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

# lib
nereid_add_unit_test(test_fsm     SOURCES ${FW}/lib/Fsm/src/Fsm.c         INCLUDES ${INC_FSM})
nereid_add_unit_test(test_ringbuf SOURCES ${FW}/lib/RingBuf/src/RingBuf.c INCLUDES ${FW}/lib/RingBuf/include)
nereid_add_unit_test(test_crc     SOURCES ${FW}/lib/Crc/src/Crc.c         INCLUDES ${FW}/lib/Crc/include)

# BSW
nereid_add_unit_test(test_dem
    SOURCES  ${FW}/bsw/services/Dem/src/Dem.c ${STUBS}/Stub_Os.c ${STUBS}/Stub_Log.c
    INCLUDES ${INC_SERVICES})
nereid_add_unit_test(test_wdgm
    SOURCES  ${FW}/bsw/services/WdgM/src/WdgM.c ${STUBS}/Stub_Os.c ${STUBS}/Stub_Log.c ${STUBS}/Stub_Wdg.c
             ${STUBS}/Stub_Dem.c
    INCLUDES ${INC_SERVICES} ${INC_MCAL})
nereid_add_unit_test(test_iohwab
    SOURCES  ${FW}/bsw/ecual/IoHwAb/src/IoHwAb.c ${STUBS}/Stub_Dio.c ${STUBS}/Stub_Det.c
    INCLUDES ${FW}/bsw/ecual/IoHwAb/include ${INC_MCAL} ${INC_SERVICES})

# Application SWCs (each test file provides the fake RTE)
nereid_add_unit_test(test_vehiclemode
    SOURCES  ${FW}/app/VehicleMode/src/VehicleMode.c ${FW}/lib/Fsm/src/Fsm.c
    INCLUDES ${FW}/app/VehicleMode/include ${INC_RTE} ${INC_FSM})
nereid_add_unit_test(test_benchpanel
    SOURCES  ${FW}/app/BenchPanel/src/BenchPanel.c
    INCLUDES ${FW}/app/BenchPanel/include ${INC_RTE})
nereid_add_unit_test(test_statusled
    SOURCES  ${FW}/app/StatusLed/src/StatusLed.c
    INCLUDES ${FW}/app/StatusLed/include ${INC_RTE})
nereid_add_unit_test(test_lights
    SOURCES  ${FW}/app/Lights/src/Lights.c ${FW}/lib/Fsm/src/Fsm.c
    INCLUDES ${FW}/app/Lights/include ${INC_RTE} ${INC_FSM})

# Integration smoke test: the full ECU software on the FreeRTOS POSIX port, driven by a
# scripted user button. Checks the whole mode sequence and that nothing faulted.
add_test(NAME host_smoke_mode_sequence
    COMMAND nereid_host --script "w500 s w400 s w400 l w400 s w600 q")
set_tests_properties(host_smoke_mode_sequence PROPERTIES
    TIMEOUT 20
    PASS_REGULAR_EXPRESSION
        "SAFE -> STANDBY.*STANDBY -> ARMED_MANUAL.*propulsion ENABLED.*ARMED_MANUAL -> STANDBY.*STANDBY -> EMERGENCY_STOP.*EMERGENCY_STOP -> SAFE.*SAFE -> STANDBY.*quit \\(exit 0\\)"
    FAIL_REGULAR_EXPRESSION "WATCHDOG|FATAL|Det:|FAILED|overran")
