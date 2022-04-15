#!/usr/bin/sh

config="Debug"
logArg="-DLOG_ON=ON"
build="Build-cmake"
asioPath=' -DASIO_PATH="C:/lib/asio-1.22.1/include"'

[ ! -d "$build" ] && mkdir $build
cd $build
echo $build

cmake ..$logArg$asioPath

cmake --build . --config=$config