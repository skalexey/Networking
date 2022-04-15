#!/bin/sh
echo "OS: $OSTYPE"
generatorArg=" "
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        $generatorArg=" "
elif [[ "$OSTYPE" == "darwin"* ]]; then
		$generatorArg=" -GXcode"
        # Mac OSX
elif [[ "$OSTYPE" == "cygwin" ]]; then
        $generatorArg=" "
elif [[ "$OSTYPE" == "msys" ]]; then
        $generatorArg=" "
elif [[ "$OSTYPE" == "win32" ]]; then
        $generatorArg=" "
elif [[ "$OSTYPE" == "freebsd"* ]]; then
        $generatorArg=" "
else
        $generatorArg=" "
fi
config="Debug"
logArg=" -DLOG_ON=ON"
build="Build-cmake"
#asioPath=' -DASIO_PATH="C:/lib/asio-1.22.1/include"'
asioPath=" -DASIO_PATH=/Users/skalexey/lib/asio-1.22.1/include"

echo "Build directory: '$build'"

[ ! -d "$build" ] && mkdir $build
cd $build

cmake ..$generatorArg$logArg$asioPath

cmake --build . --config=$config