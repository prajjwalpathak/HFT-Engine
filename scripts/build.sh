#!/bin/bash

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

chmod +x scripts/build.sh