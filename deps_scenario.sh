#!/bin/bash

function deps_scenario()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    source $THIS_DIR/dependencies.sh
    source $THIS_DIR/deps_config.sh
    
    download_dependency "Utils" "$depsLocation" "git@github.com:skalexey/Utils.git"
    download_dependency "asio-1.28.0" "$depsLocation" "https://sourceforge.net/projects/asio/files/asio/1.28.0%20%28Stable%29/asio-1.28.0.tar.gz/download"
    download_dependency "OpenSSL-3.1.4" "$depsLocation" "https://www.openssl.org/source/openssl-3.1.4.tar.gz"
    # download_dependency "OpenSSL-1.1.1w" "$depsLocation" "https://www.openssl.org/source/openssl-1.1.1w.tar.gz"
}

deps_scenario $@
