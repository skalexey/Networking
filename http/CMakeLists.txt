# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (http)

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
	message("http: Log is ON")
	add_compile_definitions("LOG_ON")
else()
	message("http: Log is OFF")
endif ()
if (LOG_LEVEL_VERBOSE)
	message("http: LOG_LEVEL_VERBOSE is ON")
	add_compile_definitions("LOG_LEVEL_VERBOSE")
else()
	message("http: LOG_LEVEL_VERBOSE is OFF")
endif()

# set build folder name variable for any dependency
if (CMAKE_CXX_COMPILER EQUAL "g++")
	set(BUILD_DIR_NAME "Build-g++")
else()
	set(BUILD_DIR_NAME "Build-cmake")
endif()

# set deps variables
set(http_DEPS "$ENV{http_deps}")
set(http_DEPS "${http_DEPS}" PARENT_SCOPE)
if ("${http_DEPS}" STREQUAL "")
	set(DEPS "$ENV{build_deps}")
endif()
if ("${DEPS}" STREQUAL "")
	set(DEPS ${http_DEPS})
endif()
if ("${DEPS}" STREQUAL "")
	message(SEND_ERROR "http: ERROR: DEPS is not specified. Take the default value")
	set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
else()
	message("http: DEPS: ${DEPS}")
endif()
set(DEPS ${DEPS} PARENT_SCOPE)

option(HTTP_STANDALONE "Link netlib or not" ON)

set(NETLIB_DIR "${PROJECT_SOURCE_DIR}/../netlib")
if (HTTP_STANDALONE)
	add_subdirectory(${NETLIB_DIR} ${NETLIB_DIR}/${BUILD_DIR_NAME})
else()
	message("http: HTTP_STANDALONE is OFF")
endif()

set(http_INCLUDES 
	${PROJECT_SOURCE_DIR}
	${NETLIB_INCLUDES}
	${NETLIB_DIR}/..
)

set(http_INCLUDES ${http_INCLUDES} PARENT_SCOPE)
message("http: http_INCLUDES: ${http_INCLUDES}")
include_directories(${http_INCLUDES})

file(GLOB SRC CONFIGURE_DEPENDS "src/*")
file(GLOB HEADERS CONFIGURE_DEPENDS "*.h")

# if (HTTP)
	add_library(http ${SRC})
	target_link_libraries(http PRIVATE netlib)
# endif()
