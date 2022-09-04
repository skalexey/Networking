#!/bin/bash

export netlib_deps="${HOME}/Projects"

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $THIS_DIR/os.sh

if is_windows; then
	export netlib_libs="C:/lib"
else
	export netlib_libs="~/lib"
fi