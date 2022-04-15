#!/usr/bin/sh

config="Debug"
logArg="-DLOG_ON=ON"
build="Build-cmake"

mkdir $build
cd $build
cmake .. $logArg
cmake --build . --config=$config