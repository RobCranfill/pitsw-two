#!/bin/bash
# copy new binary to Pico in thingy mode, and then open com port to it

cp ../build/pitsw/pitsw.uf2 /mnt/CIRCUITPY/

telecom

