# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (netlib)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

include_directories("${ASIO_PATH}")

set(NETWORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

include_directories("${NETWORKING_DIR}")
include_directories("${NETWORKING_DIR}/netlib")
include_directories("${NETWORKING_DIR}/include")

file(GLOB SRC CONFIGURE_DEPENDS "tcp/src/*")
file(GLOB HEADERS "tcp/*")
add_library(netlib ${SRC} ${HEADERS})