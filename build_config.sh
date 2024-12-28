#!/bin/bash

source netlib/external_config.sh
source external_config.sh

buildFolderPrefix="Build"
extraArg=" "
extraArgWin="$extraArg"
extraArgMac="$extraArg"
buildConfig="Debug"
logArg=" -DLOG_ON=ON -DLOG_LEVEL_DEBUG=ON -DLOG_LEVEL_VERBOSE=OFF "
