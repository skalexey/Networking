#!/usr/bin/sh

config="Debug"

mkdir build
cd build
cmake ..
cmake --build . --config=$config