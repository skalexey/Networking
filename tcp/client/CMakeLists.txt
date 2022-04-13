# CMakeList.txt : CMake project for tcp_client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (tcp_client)

include_directories("C:/lib/asio-1.22.1/include")
message("client CMAKE_CURRENT_SOURCE_DIR: '${CMAKE_CURRENT_SOURCE_DIR}'")
message("client PROJECT_SOURCE_DIR: '${PROJECT_SOURCE_DIR}'")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")
# Add source to this project's executable.
add_library (tcp_client "src/client.cpp" "client.h")
