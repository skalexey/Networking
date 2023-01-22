#!/bin/bash

function deps_scenario()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    source $THIS_DIR/dependencies.sh
    source $THIS_DIR/deps_config.sh
    
    download_dependency "Utils" "$depsLocation" "git@github.com:skalexey/Utils.git"
    download_dependency "asio-1.22.1" "$depsLocation" "https://sourceforge.net/projects/asio/files/asio/1.22.1%20%28Stable%29/asio-1.22.1.tar.bz2/download"
}

deps_scenario $@
