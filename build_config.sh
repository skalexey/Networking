#!/bin/bash

source netlib/external_config.sh
source external_config.sh

buildFolderPrefix="Build"
extraArg=" -DDEPS=${depsLocation}"
extraArgWin="$extraArg"
extraArgMac="$extraArg"
buildConfig="Debug"
logArg=" -DLOG_ON=ON"
