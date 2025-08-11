#!/bin/bash

# Default config
CONFIG=Debug

# Cek flag --release
if [ "$1" == "--release" ]; then
    CONFIG=Release
fi

# Build dengan CMake preset
cmake --preset vs2022-x86-$CONFIG
cmake --build out/$CONFIG --config $CONFIG

# Pindahkan folder hasil build ke bin/
mv out/$CONFIG/$CONFIG bin/$CONFIG

echo "Build selesai untuk konfigurasi $CONFIG. Output ada di folder bin/"