#
#
#
cmake_minimum_required(VERSION 3.0.2)

project(BeamForming)

option(STATIC_LIBS "Build static libraries" OFF)
option(INSTALL_LOCAL "Install local directory" OFF)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
set(PROJECT_VERSION 2.0.0 CACHE STRING "BeamForming version")
set(PROJECT_TYPE "c++/OpenHRI")

find_package(OpenRTM-aist REQUIRED)

# Add an "uninstall" target
AddUninstallTarget()

if (INSTALL_LOCAL)
  get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../Components" ABSOLUTE)
endif()
RewriteBuildOptions()

##############
#  Build
##############
#
#  Addtional IDL files
#add_idl_files(
#  idl/BasicDataType.idl
#  idl/ExtendedDataTypes.idl
#)
#
#  compile IDL file
#compile_idl_files()

#
#
set(rtc_headers
  include/BeamForming.h
)

set(rtc_srcs
    src/BeamForming.cpp
)

set(rtc_exec_src src/BeamFormingComp.cpp src/utils.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${OPENRTM_INCLUDE_DIRS}
)

add_definitions(${OPENRTM_CFLAGS})
link_directories(${OPENRTM_LIBRARY_DIRS})

#############
#  3rdPary Libs
#find_package(libsndfile REQUIRED)
#include_directories(${SNDFILE_INCLUDE_DIR})
#set(OPTION_LIBRARIES ${SNDFILE_LIBRARIES})
set(OPTION_LIBRARIES)

#############
#  RTC
set(RTC_SOURCES ${rtc_srcs} ${rtc_headers} ${ALL_IDL_SRCS})
set(RTC_EXEC_SOURCES  ${rtc_exec_src} ${RTC_SOURCES})

#
#  Dynamic local library
add_rtc_library(${PROJECT_NAME} ${RTC_SOURCES})

#
# Standalone executable
add_rtc_executable(${PROJECT_NAME}Comp ${RTC_EXEC_SOURCES})

#############
# Install
#############
message("--- INSTALL_PREFIX:${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}Comp
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION bin COMPONENT componenet
#    LIBRARY DESTINATION lib COMPONENT componenet
#    ARCHIVE DESTINATION lib COMPONENT componenet
 )

gen_setup_bat()

#
#  install 3rdpary runtime libraries
#install(FILES ${SNDFILE_RUNTIME} DESTINATION bin)

####################
# Package creation
# By default, do not warn when built on machines using only VS Express:
#CreatePackageCPack()
