# CMakeList.txt : CMake project for tcp_server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(tcp_server)

include_directories("C:/lib/asio-1.22.1/include")
message("server CMAKE_CURRENT_SOURCE_DIR: '${CMAKE_CURRENT_SOURCE_DIR}'")
message("server PROJECT_SOURCE_DIR: '${PROJECT_SOURCE_DIR}'")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")
add_library (tcp_server "src/server.cpp" "server.h")
