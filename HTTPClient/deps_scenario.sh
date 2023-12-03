#!/bin/bash

function deps_scenario()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    source $THIS_DIR/../deps_scenario.sh

	download_dependency "DataModelBuilder" "$depsLocation" "git@github.com:skalexey/DataModelBuilder.git"
}

deps_scenario $@
