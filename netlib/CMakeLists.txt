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
	set(CMAKE_CXX_STANDARD 20)
endif()

# if (LOG_ON)
# 	message("netlib: Log is ON")
# 	add_compile_definitions("LOG_ON")
# else()
# 	message("netlib: Log is OFF")
# endif ()
# if (LOG_LEVEL_VERBOSE)
# 	message("netlib: LOG_LEVEL_VERBOSE is ON")
# 	add_compile_definitions("LOG_LEVEL_VERBOSE")
# else()
# 	message("netlib: LOG_LEVEL_VERBOSE is OFF")
# endif()

if ("${ASIO_PATH}" STREQUAL "")
	set (ASIO_PATH $ENV{netlib_asio_path})
endif()

message("netlib: Asio path: '${ASIO_PATH}'")

if ("${DEPS}" STREQUAL "")
	message("netlib: WARNING: DEPS is not specified. Take the default value")
	set(DEPS "$ENV{networking_deps}")
else()
	message("netlib: DEPS: ${DEPS}")
endif()

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
file(GLOB HEADERS CONFIGURE_DEPENDS "tcp/*" "common/*")

add_library(netlib ${SRC} ${HEADERS})
