cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project command)
include(pico_sdk_import.cmake)
# include(pico_extras_import_optional.cmake)
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(pitsw C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(
        -Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # i'm just experimenting, here
        -Wno-maybe-uninitialized
        )

# ---------------------------------------------------
# this project's make stuff (in addition to the 'project' command, above)

add_executable(pitsw
        pitsw.c accel.c getline.c
        )

# Add pico_multicore which is required for multicore functionality
target_link_libraries(pitsw
        hardware_i2c
        pico_stdlib
        pico_multicore)

# cran - added stdio stuff
pico_enable_stdio_usb(pitsw 1)
pico_enable_stdio_uart(pitsw 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(pitsw)

# cran - can we use this somehow? removed for now
# add url via pico_set_program_url
# example_auto_set_url(pitsw)
