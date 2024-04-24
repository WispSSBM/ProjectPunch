#!/bin/bash

set -u

VER=${1}
./build.sh || exit -1

if [ -d tmp ]; then
    rm -r tmp
fi

mkdir tmp

cd tmp
mkdir -p Source/
mkdir -p pf/module
mkdir -p pf/plugins

cp ../ProjectPunch.map ./
cp ../ProjectPunch.rel ./pf/plugins
cp ../README.md ./
cp ../tools/sy_core.rel ./pf/module
cp ../tools/syriinge.asm ./Source
cp ../tools/pre_syriinge.asm ./Source

zip -r "project-punch-${VER}.zip" ./*
mv ./*.zip ../
cd ../
rm -r tmp