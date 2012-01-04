
#http://cmake.org/Wiki/CMake_Useful_Variables
#The java path is needed for the appropriate java includes
#http://www.cmake.org/cmake/help/cmake-2-8-docs.html#module:FindJNI

include_directories(${PROJECT_SOURCE_DIR}/src/c++/ ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

add_executable(synthesisTest synthesisTest.cpp)
target_link_libraries(synthesisTest ${JAVA_JVM_LIBRARY})

#TODO set component property here as well?
install(TARGETS synthesisTest
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION bin)
