#!/bin/bash

source external_config.sh

buildFolderPrefix="Build"
extraArg=" -DDEPS=${depsLocation}"
extraArgWin="$extraArg"
extraArgMac="$extraArg"
buildConfig="Debug"
logArg=" -DLOG_ON=ON"
