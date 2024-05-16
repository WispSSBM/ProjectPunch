#!/bin/bash

set -eu
PPLUS_DIR="${HOME}/Documents/WiiSDSync/Project+"
MAPS_DIR="${HOME}/Documents/Dolphin Emulator/Maps"
make clean 
make -j 8 build
cp ProjectPunch.rel "${PPLUS_DIR}/pf/plugins/"
