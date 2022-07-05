#!/bin/bash

buildFolderPrefix="Build"
extraArg=" -DDEPS=${depsLocation}"
extraArgWin="$extraArg -DASIO_PATH=C:/lib/asio-1.22.1/include"
extraArgMac="$extraArg -DASIO_PATH=~/lib/asio-1.22.1/include"
buildConfig="Debug"
logArg=" -DLOG_ON=ON"
