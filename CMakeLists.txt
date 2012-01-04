cmake_minimum_required(VERSION 2.8.4)

project(voce)

set(CPACK_PACKAGE_VENDOR "Iowa State University - Vance Research Group")
set(CPACK_PACKAGE_CONTACT "Ryan Pavlik <rpavlik@iastate.edu>")

find_package(JNI REQUIRED)

add_subdirectory(samples)

get_filename_component(JAVA_JVM_LIBRARY_DIR "${JAVA_JVM_LIBRARY}" PATH)
if(WIN32)
	find_file(JAVA_JVM_RUNTIME_LIBRARY jvm.dll
		HINTS
		"${JAVA_JVM_LIBRARY_DIR}/../jre/bin/client")
	if(NOT JAVA_JVM_RUNTIME_LIBRARY)
		message(FATAL_ERROR "Cannot find jvm.dll - it should have been right near your JDK's link library!")
	endif()
	install(FILES ${JAVA_JVM_RUNTIME_LIBRARY}
		DESTINATION bin)
endif()