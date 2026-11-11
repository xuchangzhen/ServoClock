
cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

project(ServoClock C ASM)
set(CMAKE_C_STANDARD 11)

# 保留原来的标准库和用户模块
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS
    "Library/*.c"
)

file(GLOB_RECURSE USER_SOURCES CONFIGURE_DEPENDS
    "User/*.c"
)

add_executable(ServoClock.elf
    Start/startup_stm32f10x_md_gcc.s
    Start/core_cm3.c
    Start/system_stm32f10x.c
    System/Delay.c
    ${LIB_SOURCES}
    ${USER_SOURCES}
)

target_include_directories(ServoClock.elf PRIVATE
    Start
    Library
    System
    User
    User/BSP
    User/Drivers
    User/App
    User/Test
)

target_compile_definitions(ServoClock.elf PRIVATE
    STM32F10X_MD
    USE_STDPERIPH_DRIVER
    HSE_VALUE=8000000
)

target_compile_options(ServoClock.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -Og
    -g3
    -ffunction-sections
    -fdata-sections
)

target_link_options(ServoClock.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld
    -Wl,--gc-sections
    --specs=nano.specs
    --specs=nosys.specs
)

target_link_libraries(ServoClock.elf PRIVATE m)
