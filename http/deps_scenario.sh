#!/bin/bash

function deps_scenario()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
	source $THIS_DIR/../deps_scenario.sh

	download_dependency "zlib-1.3" "$depsLocation" "https://zlib.net/zlib-1.3.tar.gz"
}

deps_scenario $@
