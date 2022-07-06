#!/bin/bash

export netlib_deps="${HOME}/Projects"

THIS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $THIS_DIR/os.sh

if is_windows; then
	export netlib_asio_path="C:/lib/asio-1.22.1/include"
else
	export netlib_asio_path="~/lib/asio-1.22.1/include"
fi