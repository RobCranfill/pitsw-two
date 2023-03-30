cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

# cran - was: project(pico_examples C CXX ASM)
project(pitsw C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# do we really need the path to examples?
set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

# cran - no
# include(example_auto_set_url.cmake)


# cran - no - # add cran stuff
# cran - no - add_subdirectory(pitsw)

# cran - no - # Add blink example
# cran - no - add_subdirectory(blink)
# cran - no - 
# cran - no - # Add hello world example
# cran - no - add_subdirectory(hello_world)


# cran - why is this down here? should it be up there?
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )


# this project?
include(CMakeLists-pitsw.txt)



# cran - no - # Hardware-specific examples in subdirectories:
# cran - no - add_subdirectory(adc)
# cran - no - add_subdirectory(clocks)
# cran - no - add_subdirectory(cmake)
# cran - no - add_subdirectory(divider)
# cran - no - add_subdirectory(dma)
# cran - no - add_subdirectory(flash)
# cran - no - add_subdirectory(gpio)
# cran - no - add_subdirectory(i2c)
# cran - no - add_subdirectory(interp)
# cran - no - add_subdirectory(multicore)
# cran - no - add_subdirectory(picoboard)
# cran - no - add_subdirectory(pico_w)
# cran - no - add_subdirectory(pio)
# cran - no - add_subdirectory(pwm)
# cran - no - add_subdirectory(reset)
# cran - no - add_subdirectory(rtc)
# cran - no - add_subdirectory(spi)
# cran - no - add_subdirectory(system)
# cran - no - add_subdirectory(timer)
# cran - no - add_subdirectory(uart)
# cran - no - add_subdirectory(usb)
# cran - no - add_subdirectory(watchdog)
