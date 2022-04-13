# CMakeList.txt : CMake project for TCPClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TCPClient)

include_directories("C:/lib/asio-1.22.1/include")

set(NETWORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(TCP_CLIENT_DIR "${NETWORKING_DIR}/tcp/client")

include_directories("${TCP_CLIENT_DIR}")
include_directories("${NETWORKING_DIR}")
include_directories("${NETWORKING_DIR}/include")

message("TCPClient TCP_CLIENT_DIR: '${TCP_CLIENT_DIR}'")

add_subdirectory("${TCP_CLIENT_DIR}" "${TCP_CLIENT_DIR}/out")

add_executable (TCPClient "TCPClient.cpp" "TCPClient.h")

target_link_libraries(TCPClient tcp_client)
