# CMakeList.txt : CMake project for TCPServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TCPServer)

include_directories("C:/lib/asio-1.22.1/include")
set(NETWORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(TCP_SERVER_DIR "${NETWORKING_DIR}/tcp/server")

message("TCPServer NETWORKING_DIR: '${NETWORKING_DIR}'")
message("TCPServer TCP_SERVER_DIR: '${TCP_SERVER_DIR}'")

include_directories("${NETWORKING_DIR}")
include_directories("${NETWORKING_DIR}/include")

add_subdirectory("${TCP_SERVER_DIR}" "${TCP_SERVER_DIR}/out")
add_executable (TCPServer "TCPServer.cpp" "TCPServer.h")
target_link_libraries(TCPServer tcp_server)
