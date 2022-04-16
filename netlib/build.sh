#!/bin/sh

echo "Build for OS: $OSTYPE"

buildFolderPrefix="Build"
generatorArg=" "
onlyLibArg=" "
cmakeTestsArg=" "
cmakeGppArg=" "
asioPathArg=" "
asioPathArgWin=" -DASIO_PATH=C:/lib/asio-1.22.1/include"
asioPathArgMac=" -DASIO_PATH=~/lib/asio-1.22.1/include"
buildConfig="Debug"
logArg=" -DLOG_ON=ON"
build="Build-cmake"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	generatorArg=" "
elif [[ "$OSTYPE" == "darwin"* ]]; then
	# Mac OSX
	generatorArg=" -GXcode"
	asioPathArg=$asioPathArgMac
elif [[ "$OSTYPE" == "cygwin" ]]; then
	generatorArg=" "
	asioPathArg=$asioPathArgWin
elif [[ "$OSTYPE" == "msys" ]]; then
	generatorArg=" "
	asioPathArg=$asioPathArgWin
elif [[ "$OSTYPE" == "win32" ]]; then
	generatorArg=" "
	asioPathArg=$asioPathArgWin
elif [[ "$OSTYPE" == "freebsd"* ]]; then
	generatorArg=" "
else
	generatorArg=" "
fi

echo "ASIO path arg: '$asioPathArg'"

for arg in "$@" 
do
	echo "Passed arg: '$arg'"
		
	if [[ "$arg" == "only-lib" ]]; then
		echo "--- 'only-lib' option passed. Build only library without tests"
		onlyLibArg=" only-lib"
		cmakeTestsArg=" "
	elif [[ "$arg" == "g++" ]]; then
		echo "--- 'g++' option passed. Build with g++ compiler"
		cmakeGppArg= -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gpp
		gppArg="g++"
		buildFolderPrefix="Build-g++"
	elif [[ "$arg" == "no-log" ]]; then
		echo "--- 'no-log' option passed. Turn off LOG_ON compile definition"
		logArg=" "
	elif [[ "$arg" == "release" ]]; then
		echo "--- 'release' option passed. Set Release build type"
		buildConfig="Release"
	fi
done

build="${buildFolderPrefix}-cmake"

echo "--- Build directory: '$build' --- "

[ ! -d "$build" ] && mkdir $build || echo "	already exists"
cd $build

cmake ..$generatorArg$logArg$asioPathArg

cmake --build . --config=$buildConfig

echo "Build finished"