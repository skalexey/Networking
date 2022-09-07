#!/bin/bash

function deps_scenario()
{
    source dependencies.sh
    source deps_config.sh
    
    download_dependency "Utils" "$depsLocation" "git@github.com:skalexey/Utils.git"
    download_dependency "asio-1.22.1" "$depsLocation" "https://sourceforge.net/projects/asio/files/asio/1.22.1%20%28Stable%29/asio-1.22.1.tar.bz2/download"
}

deps_scenario $@
