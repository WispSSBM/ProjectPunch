#!/bin/bash

MOUNT="P"


imdisk -l -m "${MOUNT}:"; 
if [[ $? -eq 0 ]]; then
    imdisk -d -m "${MOUNT}:"
fi

set -eu
imdisk -a -m "${MOUNT}:" -t file -f sd.raw -v 1 -o rem
PPLUS_DIR="${MOUNT}/Project+"
MAPS_DIR="${HOME}/Documents/Dolphin Emulator/Maps"
make clean 
make -j 8 build
cp ProjectPunch.rel "/${PPLUS_DIR}/pf/plugins/"
imdisk -d -m "${MOUNT}:"
