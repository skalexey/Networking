# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (netlib)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

message("Asio path: '${ASIO_PATH}'")

include_directories("${ASIO_PATH}")
include_directories(".")
include_directories("common")

file(GLOB SRC CONFIGURE_DEPENDS "tcp/src/*")
file(GLOB HEADERS CONFIGURE_DEPENDS "tcp/*" "common/*")

add_library(netlib ${SRC} ${HEADERS})