#
#
#
cmake_minimum_required(VERSION 3.0.2)

project(WavPlayer)
find_package(OpenRTM REQUIRED)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
set(PROJECT_VERSION 2.0.0 CACHE STRING "WavPlayer version")
set(PROJECT_TYPE "c++/OpenHRI")

#
# Check Distribution and Package Manager
CheckPkgManager()

# Add an "uninstall" target
AddUninstallTarget()

option(BUILD_DOCUMENTATION "Build the documentation" OFF)
option(BUILD_IDL "Build and install idl" ON)

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

##############
#  Build
##############
if(BUILD_IDL)
  include("${OPENRTM_DIR}/cmake/idl_compile.cmake")
  set(idls
    ${CMAKE_CURRENT_SOURCE_DIR}/idl/BasicDataType.idl
  )
  OPENRTM_COMPILE_IDL_FILES(${idls})
  FILTER_LIST("ALL_IDL_SRCS" "hh$" idl_headers)
endif(BUILD_IDL)

#
#
set(comp_headers
  ${PROJECT_SOURCE_DIR}/include/WavPlayer.h
)

set(rtc_sources
    src/WavPlayer.cpp
)

set(rtc_exec_src src/WavPlayerComp.cpp)

RewriteBuildOptions()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${OPENRTM_INCLUDE_DIRS}
)

add_definitions(${OPENRTM_CFLAGS})
link_directories(${OPENRTM_LIBRARY_DIRS})

#############
#  3rdPary Libs
find_package(libsndfile REQUIRED)
include_directories(${SNDFILE_INCLUDE_DIR})
set(OPTION_LIBRARIES ${SNDFILE_LIBRARIES})

#############
#  RTC
set(PROJECT_RTC_SOURCES ${rtc_sources} ${comp_headers} ${ALL_IDL_SRCS})

#
#  compile IDL file
set_source_files_properties(${ALL_IDL_SRCS} PROPERTIES GENERATED 1)
if(NOT TARGET ALL_IDL_TGT)
  add_custom_target(ALL_IDL_TGT)
endif()

#
#  Dynamic local library
add_library(${PROJECT_NAME} ${LIB_TYPE} ${PROJECT_RTC_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
add_dependencies(${PROJECT_NAME} ALL_IDL_TGT)
target_link_libraries(${PROJECT_NAME} ${OPENRTM_LIBRARIES} ${OPTION_LIBRARIES})

#
# Standalone executable
add_executable(${PROJECT_NAME}Comp ${rtc_exec_src} ${PROJECT_RTC_SOURCES})
add_dependencies(${PROJECT_NAME}Comp ALL_IDL_TGT)
target_link_libraries(${PROJECT_NAME}Comp ${OPENRTM_LIBRARIES} ${OPTION_LIBRARIES})

#############
# Install
#############
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}Comp
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
    LIBRARY DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
    ARCHIVE DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
 )

install(FILES ${SNDFILE_RUNTIME} DESTINATION ${INSTALL_PREFIX})

####################
# Package creation
# By default, do not warn when built on machines using only VS Express:
#CreatePackageCPack()
