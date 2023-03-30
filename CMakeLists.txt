add_executable(pitsw
        pitsw.c getline.c
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

# add url via pico_set_program_url
example_auto_set_url(pitsw)
