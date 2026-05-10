#!/bin/bash

set -e

BUILD_TYPE=${1:-Release}

echo "Build Type: $BUILD_TYPE"

mkdir -p build

cd build

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

make -j$(nproc)