cmake_minimum_required(VERSION 2.8.4)

project(voce)

set(CPACK_PACKAGE_VENDOR "Iowa State University - Vance Research Group")
set(CPACK_PACKAGE_CONTACT "Ryan Pavlik <rpavlik@iastate.edu>")

find_package(JNI REQUIRED)

add_subdirectory(samples)
