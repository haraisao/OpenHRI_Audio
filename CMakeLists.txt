cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(OpenHRIAudio)
SUBDIRS(cmake_files)
set(SolutionDir ${PROJECT_SOURCE_DIR})

set(PACKAGE "${project}")
SET(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share")

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

IF(NOT EXISTS "${SolutionDir}/bin")
  file(MAKE_DIRECTORY "${SolutionDir}/bin")
ENDIF(NOT EXISTS "${SolutionDir}/bin")
