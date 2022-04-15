# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Networking")

# Include sub-projects.
option(NETLIB_EXTERNAL "Don't add netlib subdirectory inside nested projects" ON)
add_subdirectory ("netlib")
add_subdirectory ("Test")
add_subdirectory ("TCPClient")
add_subdirectory ("TCPServer")
