# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Networking")

if(MSVC)
	# fix __cplusplus macro for MSVC
	add_compile_options(
		"/Zc:__cplusplus"
		"/std:c++latest"
	)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
	set(CMAKE_CXX_STANDARD 17)
endif()

# Include sub-projects.
option(NETLIB_EXTERNAL "Don't add netlib subdirectory inside nested projects" ON)
add_subdirectory("netlib")
set(HTTP_STANDALONE OFF)
add_subdirectory("http")
add_subdirectory("Test")
add_subdirectory("TCPClient")
add_subdirectory("TCPServer")
