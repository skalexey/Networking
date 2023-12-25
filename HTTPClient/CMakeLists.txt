cmake_minimum_required (VERSION 3.15)

project(HTTPClient C CXX)

macro(setup_module)
	module_message("Setting up module: ${MODULE_NAME}")
	module_message("Parent dir: ${PARENT_DIR}")
	
	# HTTP dependency
	set(HTTP_DIR "${PARENT_DIR}/http")
	module_message("HTTPClient HTTP_DIR: '${HTTP_DIR}'")
	add_dependency_module(${HTTP_DIR} http)
	# DMBCore dependency
	add_dependency_module(${DEPS}/DataModelBuilder/Core DMBCore)

	set(UTILS_DIR ${DEPS}/Utils)

	module_add_includes(
		${PARENT_DIR}/include
	)
	# Group sources for IDE
	set_headers()
	GROUP_SRC("${HEADERS}" "include")
	# Add headers as sources to make them visible in IDE
	add_src(
		${HEADERS}
		${UTILS_DIR}/src/networking/uploader_with_auth.cpp
	)
	file(GLOB_RECURSE PARENT_DIR_HEADERS "${PARENT_DIR}/include/*")
	module_message("PARENT_DIR_HEADERS: ${PARENT_DIR_HEADERS}")
	add_src(${PARENT_DIR_HEADERS})
	# add executable
	module_add_executable()
	configure_file(test.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
	configure_file(test2.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endmacro()

include(cmake_modules/modules.cmake)
modules_include_all(cmake_modules)
setup_module_project()
