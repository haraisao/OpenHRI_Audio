#
#
#
cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0077 NEW)

project(DSArray)

option(STATIC_LIBS "Build static libraries" OFF)
option(INSTALL_LOCAL "Install local directory" OFF)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
set(PROJECT_VERSION 2.0.0 CACHE STRING "DSArray version")
set(PROJECT_TYPE "c++/OpenHRI")

find_package(OpenRTM REQUIRED)

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
  include/DSArray.h
  include/windowing.h
)

set(rtc_srcs
    src/DSArray.cpp
    src/windowing.cpp
)

set(rtc_exec_src src/DSArrayComp.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${OPENRTM_INCLUDE_DIRS}
)

add_definitions(${OPENRTM_CFLAGS})
link_directories(${OPENRTM_LIBRARY_DIRS})

#############
#  3rdPary Libs
find_package(speex REQUIRED)
include_directories(${SPEEX_INCLUDE_DIR})
set(OPTION_LIBRARIES ${SPEEX_LIBRARIES})


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
    LIBRARY DESTINATION lib COMPONENT componenet
    ARCHIVE DESTINATION lib COMPONENT componenet
 )

#
#  install 3rdpary runtime libraries
#install(FILES ${SPEEX_RUNTIME} DESTINATION bin)

####################
# Package creation
# By default, do not warn when built on machines using only VS Express:
#CreatePackageCPack()
