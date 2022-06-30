# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Networking")

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# Include sub-projects.
option(NETLIB_EXTERNAL "Don't add netlib subdirectory inside nested projects" ON)
add_subdirectory ("netlib")
add_subdirectory ("Test")
add_subdirectory ("TCPClient")
add_subdirectory ("TCPServer")
