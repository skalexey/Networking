#!/bin/sh

echo " -- Build for OS: $OSTYPE"

buildFolderPrefix="Build"
generatorArg=" "
onlyLibArg=" "
cmakeTestsArg=" "
cmakeGppArg=" "
extraArg=" "
extraArgWin=" -DASIO_PATH=C:/lib/asio-1.22.1/include"
extraArgMac=" -DASIO_PATH=~/lib/asio-1.22.1/include"
buildConfig="Debug"
logArg=" -DLOG_ON=ON"
build="Build-cmake"
rootDirectory="."
folderName=${PWD##*/}

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	generatorArg=" "
elif [[ "$OSTYPE" == "darwin"* ]]; then
	# Mac OSX
	generatorArg=" -GXcode"
	extraArg=$extraArgMac
elif [[ "$OSTYPE" == "cygwin" ]]; then
	generatorArg=" "
	extraArg=$extraArgWin
elif [[ "$OSTYPE" == "msys" ]]; then
	generatorArg=" "
	extraArg=$extraArgWin
elif [[ "$OSTYPE" == "win32" ]]; then
	generatorArg=" "
	extraArg=$extraArgWin
elif [[ "$OSTYPE" == "freebsd"* ]]; then
	generatorArg=" "
else
	generatorArg=" "
fi

[ ! -z "$extraArg" ] && echo " --- Extra arguments: '$extraArg'"

argIndex=0
for arg in "$@" 
do
	#echo "arg[$argIndex]: '$arg'"
	
	if [[ $argIndex -eq 0 ]]; then
		rootDirectory=$arg
	else
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
	fi	
	argIndex=$((argIndex + 1))
done

enterDirectory=${pwd}

[ ! -d "$rootDirectory" ] && echo "Non-existent project directory passed '$rootDirectory'" && exit 1 || cd "$rootDirectory"

if [[ "$rootDirectory" != "." ]]; then
	folderName=$rootDirectory
fi

echo "Build folder '$folderName'"

build="${buildFolderPrefix}-cmake"

echo "--- Output directory: '$build' --- "

[ ! -d "$build" ] && mkdir $build || echo "	already exists"
cd $build

cmake ..$generatorArg$logArg$extraArg

retval=$?
if [ $retval -ne 0 ]; then
	echo " --- CMake configure error of folder '$folderName' --- "
	cd "$enterDirectory"
	exit
else
	echo " --- CMake configuring of folder '$folderName' successfully done ---"
fi

cmake --build . --config=$buildConfig

retval=$?
if [ $retval -ne 0 ]; then
	echo " --- CMake build error of folder '$folderName' --- "
	cd "$enterDirectory"
	exit
else
	echo " --- CMake building of folder '$folderName' successfully done ---"
fi

cd "$enterDirectory"

echo " --- Finished build of '$folderName' ---"