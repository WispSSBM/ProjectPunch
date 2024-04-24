#!/bin/bash

set -u

VER=${1}
./build.sh || exit -1

if [ -d tmp ]; then
    rm -r tmp
fi

mkdir tmp

cd tmp
cp ../ProjectPunch.map ./
cp ../ProjectPunch.rel ./
cp ../README.md ./
cp ../tools/sy_core.rel ./
cp ../tools/syriinge.asm ./

zip "project-punch-${VER}.zip" ./*
mv ./*.zip ../
cd ../
rm -r tmp