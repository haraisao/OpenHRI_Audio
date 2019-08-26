#
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0077 NEW)

project(OpenHRIAudio VERSION "2.0.0")

set(NO_INFO True)
get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/Components" ABSOLUTE)

set(INSTALL_LOCAL False)
add_subdirectory(src)
