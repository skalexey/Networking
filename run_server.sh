#!/bin/bash

function run_server()
{
	./Build-cmake/TCPServer/Release/TCPServer
}

run_server $@