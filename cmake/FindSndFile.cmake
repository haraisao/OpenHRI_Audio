# - Find SndFile library
# Find the native SndFile includes and library
# This module defines
#  SNDFILE_INCLUDE_DIRS, where to find sndfile.h, Set when
#                        SNDFILE_INCLUDE_DIR is found.
#  SNDFILE_LIBRARIES, libraries to link against to use SndFile.
#  SNDFILE_ROOT_DIR, The base directory to search for SndFile.
#                    This can also be an environment variable.
#  SNDFILE_FOUND, If false, do not try to use SndFile.
#
# also defined, but not for general use are
#  SNDFILE_LIBRARY, where to find the SndFile library.

#=============================================================================
# Copyright 2002-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# If SNDFILE_ROOT_DIR was defined in the environment, use it.

if(UNIX)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(SNDFILE REQUIRED sndfile>=1.0.11)

#SET( SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIRS} )
#SET( SNDFILE_LIB_DIRS ${SNDFILE_LIBRARY_DIRS} )
#SET( SNDFILE_LIBS     ${SNDFILE_LIBRARIES} )

#message(STATUS "SNDFILE_INC_DIRS ---> ${SNDFILE_INCLUDE_DIRS}")
#message(STATUS "SNDFILE_LIB_DIRS ---> ${SNDFILE_LIB_DIRS}")
#message(STATUS "SNDFILE_LIBS ---> ${SNDFILE_LIBS}")


else(UNIX)


IF(NOT SNDFILE_ROOT_DIR AND NOT $ENV{SNDFILE_ROOT_DIR} STREQUAL "")
  SET(SNDFILE_ROOT_DIR $ENV{SNDFILE_ROOT_DIR})
else()
  set(SNDFILE_ROOT_DIR CACHE PATH "")
ENDIF()

SET(_sndfile_SEARCH_DIRS
  ${SNDFILE_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
  HINTS
    ${_sndfile_SEARCH_DIRS}
  PATH_SUFFIXES
    include
)

FIND_LIBRARY(SNDFILE_LIBRARY
  NAMES "libsndfile-1"
  HINTS ${_sndfile_SEARCH_DIRS}
  PATH_SUFFIXES lib64 lib
  )

# handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SndFile DEFAULT_MSG
    SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)

IF(SNDFILE_FOUND)
  SET(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  SET(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
ENDIF(SNDFILE_FOUND)

MARK_AS_ADVANCED(
  SNDFILE_INCLUDE_DIR
  SNDFILE_LIBRARY
)
endif(UNIX)
