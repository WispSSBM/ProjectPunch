#!/bin/bash

set -eu
PPLUS_DIR="${HOME}/Documents/WiiSDSync/Project+"
make clean build
make
cp ProjectPunch.rel ${PPLUS_DIR}/pf/plugins/
