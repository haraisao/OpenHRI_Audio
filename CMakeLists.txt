#
#
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0077 NEW)

project(OpenHRIAudio VERSION "2.0.0")
set(PROJECT_TYPE "c++/OpenHRI")

set(NO_INFO True)
get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/Components" ABSOLUTE)
set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)

set(INSTALL_LOCAL False)
set(UTF_8 True)
add_subdirectory(src)
