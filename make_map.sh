#!/bin/bash

set -u

OUTPUT_DIR="${HOME}/Documents/Dolphin Emulator/Maps/"

python tools/convertMap.py ProjectPunch.map "${1}" "${OUTPUT_DIR}/ProjectPunch.map"
