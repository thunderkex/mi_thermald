#!/bin/bash

# Update version
chmod +x update_version.sh
./update_version.sh

mkdir -p build
cd build
cmake ..
make -j$(nproc)

if [ -f "mi_thermald" ]; then
    echo "Build successful! Binary located at: $(pwd)/mi_thermald"
else
    echo "Build failed!"
    exit 1
fi