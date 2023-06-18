# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (netlib)

if(MSVC)
	# fix __cplusplus macro for MSVC
	add_compile_options(
		"/Zc:__cplusplus"
		"/std:c++latest"
	)
else()
	set(CMAKE_CXX_STANDARD 17)
endif()

if (LOG_ON)
	message("netlib: Log is ON")
	add_compile_definitions("LOG_ON")
else()
	message("netlib: Log is OFF")
endif ()
if (LOG_LEVEL_VERBOSE)
	message("netlib: LOG_LEVEL_VERBOSE is ON")
	add_compile_definitions("LOG_LEVEL_VERBOSE")
else()
	message("netlib: LOG_LEVEL_VERBOSE is OFF")
endif()

# set deps variables
set(netlib_DEPS "$ENV{netlib_deps}")
set(netlib_DEPS "${netlib_DEPS}" PARENT_SCOPE)
if ("${netlib_DEPS}" STREQUAL "")
	file(TO_CMAKE_PATH "$ENV{build_deps}" DEPS)
endif()
if ("${DEPS}" STREQUAL "")
	set(DEPS ${netlib_DEPS})
endif()
if ("${DEPS}" STREQUAL "")
	message(SEND_ERROR "netlib: ERROR: DEPS is not specified. Take the default value")
	set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
else()
	message("netlib: DEPS: ${DEPS}")
endif()
set(DEPS ${DEPS} PARENT_SCOPE)

set(ASIO_PATH ${DEPS}/asio-1.22.1/include)
message("netlib: Asio path: '${ASIO_PATH}'")

set(UTILS_DIR "${DEPS}/Utils")

set(NETLIB_INCLUDES 
	${ASIO_PATH}
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/common
	${UTILS_DIR}/include
)

set(NETLIB_INCLUDES ${NETLIB_INCLUDES} PARENT_SCOPE)

include_directories("${NETLIB_INCLUDES}")

file(GLOB SRC CONFIGURE_DEPENDS "tcp/src/*")
message("netlib: SRC: ${SRC}")
file(GLOB HEADERS CONFIGURE_DEPENDS "tcp/*" "common/*")

add_library(netlib STATIC ${SRC} ${HEADERS})
