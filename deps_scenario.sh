#!/bin/bash

function deps_scenario()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    source $THIS_DIR/dependencies.sh
    source $THIS_DIR/deps_config.sh
    
    download_dependency "Utils" "$depsLocation" "git@github.com:skalexey/Utils.git"
    download_dependency "boost_1_81_0" "$depsLocation" "https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.bz2"
    download_dependency "certify" "$depsLocation" "https://github.com/djarek/certify.git"
    download_dependency "openssl-3.1.4" "$depsLocation" "https://www.openssl.org/source/openssl-3.1.4.tar.gz"
}

deps_scenario $@
