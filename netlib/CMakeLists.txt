cmake_minimum_required (VERSION 3.15)

project(netlib C CXX)

macro(setup_module)
	module_message("Setting up module: ${MODULE_NAME}")
	# Boost dependency
	set(BOOST_PATH ${DEPS}/boost_1_81_0)
	module_message("Boost path: '${BOOST_PATH}'")

	# OpenSSL dependency
	if(NOT "${ANDROID_ABI}" STREQUAL "")
		module_message("ANDROID_ABI: '${ANDROID_ABI}'")
		set(OPENSSL_BUILD_FOLDER ${ANDROID_ABI})
	elseif(WIN32)
		set(OPENSSL_BUILD_FOLDER "win32")
	else()
		set(OPENSSL_BUILD_FOLDER "linux")
	endif()
	set(OPENSSL_PATH ${DEPS}/openssl-3.1.4)
	module_message("OpenSSL path: '${OPENSSL_PATH}'")

	# Utils dependency
	set(UTILS_DIR "${DEPS}/Utils")

	# Certify dependency
	set(CERTIFY_DIR "${DEPS}/certify")
	if (ANDROID)
		# TODO: resolve Boost paths for Android
	else()
		add_dependency(${CERTIFY_DIR} certify)
	endif()

	module_message("Parent dir: ${PARENT_DIR}")

	module_add_includes( 
		${BOOST_PATH}
		${BOOST_PATH}/boost
		${OPENSSL_PATH}/include
		${PARENT_DIR}/include
		${UTILS_DIR}/include
		${CERTIFY_DIR}/include
	)

	# Group sources for IDE
	set_headers()
	GROUP_SRC("${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
	# Add headers as sources to make them visible in IDE
	add_src(${HEADERS})
	file(GLOB_RECURSE PARENT_DIR_HEADERS "${PARENT_DIR}/include/*")
	module_message("PARENT_DIR_HEADERS: ${PARENT_DIR_HEADERS}")
	add_src(${PARENT_DIR_HEADERS})

	# add library
	module_add_library()

	target_link_directories(${MODULE_NAME} PUBLIC ${OPENSSL_PATH}/build/${OPENSSL_BUILD_FOLDER})
	if(WIN32)
		target_link_libraries(${MODULE_NAME} PUBLIC libssl_static libcrypto_static)
	else()
		# Check if the target system is macOS
		if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
			# Find the Security framework
			find_library(SECURITY_LIBRARY Security)

			# Check if the Security framework is found
			if(SECURITY_LIBRARY)
				# Add the Security framework to the target
				target_link_libraries(${MODULE_NAME} PRIVATE ${SECURITY_LIBRARY})
			else()
				message(FATAL_ERROR "${MODULE_NAME}: Security framework not found.")
			endif()
		endif()
		target_link_libraries(${MODULE_NAME} PUBLIC libssl.a libcrypto.a)
	endif()
	if (APPLE)
		# TODO: investigate: Certify is complaining about missing CoreFoundation on macOS though it links it by itself, but privately
		target_link_libraries(${MODULE_NAME} PUBLIC "-framework CoreFoundation")
	endif()
	module_message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(${MODULE_NAME} PUBLIC crypt32)
	endif()
endmacro()

include(cmake_modules/modules.cmake)
modules_include_all(cmake_modules)
setup_module_project()
