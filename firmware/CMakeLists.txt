cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(safe-f411 LANGUAGES C CXX ASM)

set(BUILD_UNIT_0_NAME safe-f411)

add_executable(${BUILD_UNIT_0_NAME})

include(cmake/vscode_generated.cmake)

target_compile_definitions(${BUILD_UNIT_0_NAME} PRIVATE
  USE_HAL_DRIVER
  STM32F411xE
)

target_compile_options(${BUILD_UNIT_0_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:C>:
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -std=gnu11
    -O0
    -g3
    -ffunction-sections
    -fdata-sections
    -Wall
  >
  $<$<COMPILE_LANGUAGE:CXX>:
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -std=gnu++14
    -O0
    -g3
    -ffunction-sections
    -fdata-sections
    -fno-exceptions
    -fno-rtti
    -fno-use-cxa-atexit
    -Wall
  >
  $<$<COMPILE_LANGUAGE:ASM>:
    -x assembler-with-cpp
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
  >
)

target_link_options(${BUILD_UNIT_0_NAME} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard
  -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F411RETX_FLASH.ld
  -Wl,-Map=${CMAKE_BINARY_DIR}/${BUILD_UNIT_0_NAME}.map,--gc-sections
  --specs=nano.specs
  --specs=nosys.specs
  -static
)

target_include_directories(${BUILD_UNIT_0_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/app
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/common
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/rtos
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/framework
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/runtime
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/verify
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_00
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_01
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_02
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_03
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_04
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_05
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_06
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/test/tc_rtos

  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include

  ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source
  ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
  ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
  ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang
)