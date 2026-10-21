#!/bin/bash

set -eu
PPLUS_DIR="${HOME}/Documents/WiiSDSync/Project+"
MAPS_DIR="${HOME}/Documents/Dolphin Emulator/Maps"
make clean 
make -j 8 build

if [[ ! -d "${PPLUS_DIR}/pf/plugins" ]]; then
	mkdir -p "${PPLUS_DIR}/pf/plugins"
fi
cp ProjectPunch.rel "${PPLUS_DIR}/pf/plugins/"



if [[ ! -d "${PPLUS_DIR}/ppunch" ]]; then
	mkdir -p  "${PPLUS_DIR}/ppunch"
fi
cp tools/actions.bin "${PPLUS_DIR}/ppunch/actions.bin"
