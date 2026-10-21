#!/bin/bash

set -u

VER=${1}
./build.sh || exit -1

if [ -d tmp ]; then
    rm -r tmp
fi

mkdir tmp

cd tmp
mkdir -p "Project+"
mkdir -p "Project+/pf/plugins"
mkdir -p "Project+/ppunch"

cp ../ProjectPunch.map ./
cp ../ProjectPunch.rel ./Project+/pf/plugins/
cp ../README.md ./
cp ../tools/actions.bin ./Project+/ppunch/

zip -r "ProjectPunch-${VER}.zip" ./*
mv ./*.zip ../
cd ../
rm -r tmp
