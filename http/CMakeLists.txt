cmake_minimum_required (VERSION 3.15)

project(http C CXX)

macro(setup_module)
	module_message("Setting up module: ${MODULE_NAME}")
	module_message("Parent dir: ${PARENT_DIR}")

	set(NETLIB_DIR "${PARENT_DIR}/netlib")
	add_dependency_module(${NETLIB_DIR} netlib)
	set(UTILS_DIR "${DEPS}/Utils")
	set(ZLIB_DIR "${DEPS}/zlib-1.3")
	add_subdirectory(${ZLIB_DIR} ${ZLIB_DIR})
	# set(http_INCLUDES ${SOME_INCLUDES})

	add_src( 
		${UTILS_DIR}/src/string_utils.cpp
		${UTILS_DIR}/src/file_utils.cpp
		# for IDE:
		${UTILS_DIR}/include/utils/data_receiver_base.h
		${UTILS_DIR}/include/utils/data_receiver_memory.h
		${UTILS_DIR}/include/utils/data_receiver_file.h
		${UTILS_DIR}/include/utils/data_receiver.h
		${UTILS_DIR}/include/utils/networking/url.h
		${UTILS_DIR}/include/utils/networking/http.h
		${UTILS_DIR}/include/utils/zip.h
		${UTILS_DIR}/src/zip.cpp
	)

	module_add_includes( 
		${UTILS_DIR}/include
		${ZLIB_DIR}
	)

	# Group sources for IDE
	group_headers()
	add_headers_to_sources()

	# add library
	module_add_library()

	target_link_libraries(${MODULE_NAME} PUBLIC zlibstatic)
endmacro()

include(cmake_modules/modules.cmake)
modules_include_all(cmake_modules)
setup_module_project()
