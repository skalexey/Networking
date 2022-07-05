# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (netlib)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

if ("${ASIO_PATH}" STREQUAL "")
	set (ASIO_PATH $ENV{netlib_asio_path})
endif()

message("Asio path: '${ASIO_PATH}'")

set(NETLIB_INCLUDES 
	${ASIO_PATH}
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/common
)

set(NETLIB_INCLUDES ${NETLIB_INCLUDES} PARENT_SCOPE)

include_directories("${NETLIB_INCLUDES}")

file(GLOB SRC CONFIGURE_DEPENDS "tcp/src/*")
file(GLOB HEADERS CONFIGURE_DEPENDS "tcp/*" "common/*")

add_library(netlib ${SRC} ${HEADERS})