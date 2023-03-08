#!/bin/bash

export netlib_deps="${HOME}/Projects"

function job()
{
	local THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
	source $THIS_DIR/os.sh
}

job $@