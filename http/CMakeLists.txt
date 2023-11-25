cmake_minimum_required (VERSION 3.15)

project(http C CXX)

macro(setup_module)
	module_message("Setting up module: ${MODULE_NAME}")

	module_message("Parent dir: ${PARENT_DIR}")

	set(NETLIB_DIR "${PARENT_DIR}/netlib")
	add_dependency_module(${NETLIB_DIR} netlib)
	set(UTILS_DIR "${DEPS}/Utils")

	# set(http_INCLUDES ${SOME_INCLUDES})

	set_headers()
	GROUP_SRC("${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
	add_src(${HEADERS})

	file(GLOB SRC CONFIGURE_DEPENDS "src/*")
	add_src( 
		${UTILS_DIR}/src/string_utils.cpp
		${UTILS_DIR}/src/file_utils.cpp
		# for IDE:
		${UTILS_DIR}/include/utils/data_receiver_base.h
		${UTILS_DIR}/include/utils/data_receiver_memory.h
		${UTILS_DIR}/include/utils/data_receiver_file.h
		${UTILS_DIR}/include/utils/data_receiver.h
	)

	module_add_includes( 
		${UTILS_DIR}/include
		${CERTIFY_DIR}/include
	)

	# Group sources for IDE
	set_headers()
	GROUP_SRC("${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
	add_src(${HEADERS}) # Add headers as sources to make them visible in IDE

	# add library
	module_add_library()
endmacro()

include(cmake_modules/modules.cmake)
modules_include_all(cmake_modules)
setup_module_project()
