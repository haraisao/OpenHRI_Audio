#
#
#
cmake_minimum_required(VERSION 3.0.2)

project(PortAudioOutput)
find_package(OpenRTM REQUIRED)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
set(PROJECT_VERSION 2.0.0 CACHE STRING "PortAudioOutput version")
set(PROJECT_TYPE "c++/OpenHRI")

#
# Check Distribution and Package Manager
CheckPkgManager()

# Add an "uninstall" target
AddUninstallTarget()

option(BUILD_DOCUMENTATION "Build the documentation" OFF)
option(BUILD_IDL "Build and install idl" OFF)

option(STATIC_LIBS "Build static libraries" OFF)
if(STATIC_LIBS)
    set(LIB_TYPE STATIC)
else(STATIC_LIBS)
    set(LIB_TYPE SHARED)
endif(STATIC_LIBS)

# Set up installation directories
SetupInstDirs()

# Subdirectories
#add_subdirectory(cmake)
if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

if(BUILD_IDL)
    add_subdirectory(idl)
endif(BUILD_IDL)

add_subdirectory(include)
MAP_ADD_STR(headers  "${PROJECT_SOURCE_DIR}/include/" comp_headers)
add_subdirectory(src)

# Package creation
# By default, do not warn when built on machines using only VS Express:
#CreatePackageCPack()
