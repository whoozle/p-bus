#!/bin/sh

set -euo pipefail
rm -rf build
mkdir build
pushd build
cmake .. -DPBUS_DEVEL_MODE:BOOL=ON -DCMAKE_INSTALL_PREFIX:PATH=./rootfs -DCMAKE_BUILD_TYPE:STRING=Debug
make -j$(nproc) install
popd
