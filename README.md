# pitsw-two
Pi-In-The-Sky-Writer 2

Picking up where I left off.... but with an RP2040!

This project attempts to re-create the old toy "Sky Writer", which displayed messages in the air using persistance of vision.

The hardware is based on a Raspberry Pi RP2040 microcontroller; 
I intend to support both the Raspberry Pi Pico development board
as well as the Adafruit Feather RP2040 board (my favorite).
Due to the number of GPIO pins required (at least 9), other, smaller, dev boards probably won't suffice.

For now, I am prototyping this with solderless breadboard(s), but intend to create a custom PC board for it. 
I have already used DIY Layout Creator to create the board.

## Dev Environment
This project is developed using Visual Studio Code (version 1.76 currently) under WSL2 on Windows, if that matters.

Don't forget to set the various PICO_xxx environment variables! (See 'setenv.sh')

