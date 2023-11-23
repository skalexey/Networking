cmake_minimum_required (VERSION 3.15)

project(netlib C CXX)

macro(setup_module)
	module_message("Setting up module: ${MODULE_NAME}")

	# Boost dependency
	set(BOOST_PATH ${DEPS}/boost_1_81_0)
	module_message("Boost path: '${BOOST_PATH}'")

	# OpenSSL dependency
	set(OPENSSL_PATH ${DEPS}/openssl-3.1.4)
	module_message("OpenSSL path: '${OPENSSL_PATH}'")

	# Utils dependency
	set(UTILS_DIR "${DEPS}/Utils")

	# Certify dependency
	set(CERTIFY_DIR "${DEPS}/certify")

	module_add_includes( 
		${BOOST_PATH}
		${BOOST_PATH}/boost
		${OPENSSL_PATH}/include
		${PROJECT_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/common
		${UTILS_DIR}/include
		${CERTIFY_DIR}/include
	)

	# Group sources for IDE
	set_headers()
	GROUP_SRC("${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
	add_src(${HEADERS}) # Add headers as sources to make them visible in IDE

	# add library
	module_add_library()

	target_link_directories(netlib PUBLIC ${OPENSSL_PATH})
	target_link_libraries(netlib PUBLIC libssl_static libcrypto_static)
	module_message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(netlib PUBLIC crypt32)
	endif()
endmacro()

include(cmake_modules/modules.cmake)
modules_include_all(cmake_modules)
setup_module_project()
