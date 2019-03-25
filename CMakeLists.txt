CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_LANGUAGE(ASM)

SET(CMSIS_STARTUP_SOURCE Libraries/CMSIS/startup_stm32f10x_md.s)

SET(CMSIS_SOURCES
    Libraries/CMSIS/core_cm3.c
    Libraries/CMSIS/system_stm32f10x.c
    )

SET(StdPeriphLib_SOURCES
    Libraries/StdPeriph/src/misc.c
    Libraries/StdPeriph/src/stm32f10x_adc.c
    Libraries/StdPeriph/src/stm32f10x_can.c
    Libraries/StdPeriph/src/stm32f10x_dma.c
    Libraries/StdPeriph/src/stm32f10x_gpio.c
    Libraries/StdPeriph/src/stm32f10x_i2c.c
    Libraries/StdPeriph/src/stm32f10x_rcc.c
    Libraries/StdPeriph/src/stm32f10x_tim.c
    Libraries/StdPeriph/src/stm32f10x_usart.c)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}
        Libraries/CMSIS
        Libraries/StdPeriph/inc
        Libraries/UserLibrary
)

SET(PROJECT_SOURCES
    main.c
    Libraries/UserLibrary/ticks.c Libraries/UserLibrary/led.c Libraries/UserLibrary/led.h Libraries/UserLibrary/can_protocol.h Libraries/UserLibrary/can_protocol.c Libraries/UserLibrary/uart.c Libraries/UserLibrary/uart.h)

SET(STM32_LINKER_SCRIPT Libraries/CMSIS/stm32f1_flash.ld.in)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${CMSIS_STARTUP_SOURCE} ${CMSIS_SOURCES} ${StdPeriphLib_SOURCES})

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_SET_HSE_VALUE(${CMAKE_PROJECT_NAME} 8000000)
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})
