#
#  CMake for DSArray Component
#

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(fftwrap)

set(SRC_DIRS "${PROJECT_SOURCE_DIR}/../../src")

if (WIN32)
add_definitions(-DUSE_SAMLLFT)
else()
add_definitions(-DUSE_SMALLFT -DFLOATING_POINT)
endif()

add_library(fftwrap SHARED ${SRC_DIRS}/fftwrap.c ${SRC_DIRS}/smallft.c)


install(TARGETS fftwrap
				LIBRARY DESTINATION lib
				COMPONENT "applications")
