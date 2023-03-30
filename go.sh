#!/bin/bash
# copy new binary to Pico in thingy mode, and then open com port to it

sudo mount -t drvfs F: /mnt/CIRCUITPY
cp ./build/pitsw.uf2 /mnt/CIRCUITPY/

minicom /dev/uh,what?

