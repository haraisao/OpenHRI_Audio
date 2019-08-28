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
#add_subdirectory(src)

macro(DecomposeRtmVersion)
  string(REGEX REPLACE "^([0-9]+).*" "\\1"
        OPENRTM_VERSION_MAJOR "${OPENRTM_VERSION}")
  string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1"
        OPENRTM_VERSION_MINOR "${OPENRTM_VERSION}")
  string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1"
        OPENRTM_VERSION_PATCH "${OPENRTM_VERSION}")
  set(OPENRTM_SHORT_VERSION ${OPENRTM_VERSION_MAJOR}${OPENRTM_VERSION_MINOR}${OPENRTM_VERSION_PATCH})
endmacro(DecomposeRtmVersion)

message("==============")

if (NOT OPENRTM_VERSION)
  set(OPENRTM_VERSION 1.2.0)
endif()
DecomposeRtmVersion()
message("==== ${OPENRTM_VERSION}")
message("==== ${OPENRTM_VERSION_MAJOR}")
message("==== ${OPENRTM_VERSION_MINOR}")
message("==== ${OPENRTM_VERSION_PATCH}")
message("==== ${OPENRTM_SHORT_VERSION}")