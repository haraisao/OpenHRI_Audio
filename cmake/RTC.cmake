#
#  CMake for RT Component 
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

string(TOLOWER ${RTC_NAME} PACKAGE_NAME)
set(PACKAGE_VERSION "${RTC_VERSION}")

# check OpenRTM installed
find_package(OpenRTM REQUIRED)

if (DEFINED OPENRTM_INCLUDE_DIRS)
  string(REGEX REPLACE "-I" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
endif (DEFINED OPENRTM_INCLUDE_DIRS)

if (DEFINED OPENRTM_LIBRARY_DIRS)
  string(REGEX REPLACE "-L" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
endif (DEFINED OPENRTM_LIBRARY_DIRS)

if (DEFINED OPENRTM_LIBRARIES)
  string(REGEX REPLACE "-l" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
endif (DEFINED OPENRTM_LIBRARIES)



list(APPEND CFLAGS ${OMNIORB_CFLAGS} ${OPENRTM_CFLAGS})
list(APPEND INCLUDE_DIRS ${OMNIORB_INCLUDE_DIRS} ${OPENRTM_INCLUDE_DIRS})
list(APPEND LDFLAGS ${OMNIORB_LDFLAGS} ${OPENRTM_LDFLAGS})
list(APPEND LIBRARY_DIRS ${OMNIORB_LIBRARY_DIRS} ${OPENRTM_LIBRARY_DIRS})
list(APPEND LIBRARIES ${OMNIORB_LIBRARIES} ${OPENRTM_LIBRARIES})

# check doxygen installed
find_package(Doxygen)
#if(DOXYGEN_FOUND STREQUAL "NO")
#    message(FATAL_ERROR "Doxygen not found.")
#endif()


if(WIN32)
    # add command path
    set($ENV{PATH} $ENV{PATH} "${OPENRTM_DIR}/bin")
    set($ENV{PATH} $ENV{PATH} "${OMNIORB_DIR}/bin/x86_win32")
    # set install path prefix
    #set(CMAKE_INSTALL_PREFIX "${OPENRTM_DIR}")
    #set(INSTALL_PREFIX "components/${RTC_LANG}/${PACKAGE_NAME}/${PACKAGE_VERSION}")
    set(INSTALL_PREFIX "C:/hoge")
else()
    # add command path
    #set($ENV{PATH} $ENV{PATH} "${OPENRTM_DIR}/bin")
    #set($ENV{PATH} $ENV{PATH} "${OMNIORB_DIR}/bin")
    # set install path prefix
    #set(CMAKE_INSTALL_PREFIX "${OPENRTM_DIR}")
    set(OPENRTM_SHARE_PREFIX "share/openrtm-${OPENRTM_VERSION_MAJOR}.${OPENRTM_VERSION_MINOR}")
    set(INSTALL_PREFIX "${OPENRTM_SHARE_PREFIX}/components/${RTC_LANG}/${PACKAGE_NAME}${PACKAGE_VERSION}")
endif()


# add uninstall target
#configure_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#    IMMEDIATE @ONLY)
#add_custom_target(uninstall COMMAND 
#    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


# target settings for library/executable
include_directories(${INCLUDE_DIRS})

link_directories(${LIBRARY_DIRS})

add_definitions(${CFLAGS})
add_definitions(-I${CMAKE_CURRENT_BINARY_DIR})

#foreach(idl)
#    string(REGEX REPLACE "([^\\/]*)\\.idl" "\\1" idl_base ${idl})
#    add_custom_command(OUTPUT ${idl_base}Stub.cpp ${idl_base}Skel.cpp
#                       COMMAND ${OPENRTM_IDL_WRAPPER} ${OPENRTM_IDL_WRAPPER_FLAGS} --idl-file=${idl}
#                       COMMAND ${OPENRTM_IDLC} ${OPENRTM_IDLFLAGS} ${PROJECT_SOURCE_DIR}/${idl}
#                       DEPENDS ${idl})
#endforeach()

if(CMAKE_COMPILER_IS_GNUCC)
    string(REPLACE ";" " " LDFLAGS "${LDFLAGS}")
endif()

add_library(${RTC_NAME} SHARED
            ${LIBS_SRCS})
target_link_libraries(${RTC_NAME} ${LIBRARIES})
set_target_properties(${RTC_NAME} PROPERTIES PREFIX "")
set_target_properties(${RTC_NAME} PROPERTIES LINK_FLAGS "${LDFLAGS}")

if(UNIX)
    add_library(${RTC_NAME}-static STATIC
                ${LIBS_SRCS})
    target_link_libraries(${RTC_NAME}-static ${LIBRARIES})
    set_target_properties(${RTC_NAME}-static PROPERTIES PREFIX "")
    set_target_properties(${RTC_NAME}-static PROPERTIES LINK_FLAGS "${LDFLAGS}")

endif()

if(UNIX)

    #set_target_properties(${RTC_NAME}Comp PROPERTIES COMPILE_FLAGS -Wall)
    add_executable(${RTC_NAME}Comp ${EXEC_SRCS} )
    add_dependencies(${RTC_NAME}Comp ${RTC_NAME})
    add_dependencies(${RTC_NAME}Comp ${RTC_NAME}-static)
    target_link_libraries(${RTC_NAME}Comp ${RTC_NAME}-static ${LIBRARIES})

    add_executable(${RTC_NAME}Comp-dynamic ${EXEC_SRCS})
    add_dependencies(${RTC_NAME}Comp-dynamic ${RTC_NAME})
    target_link_libraries(${RTC_NAME}Comp-dynamic ${RTC_NAME} ${LIBRARIES})
    set_target_properties(${RTC_NAME}Comp-dynamic PROPERTIES LINK_FLAGS "${LDFLAGS}")

else()
    add_executable(${RTC_NAME}Comp ${EXEC_SRCS})
    add_dependencies(${RTC_NAME}Comp ${RTC_NAME})
    target_link_libraries(${RTC_NAME}Comp ${RTC_NAME} ${LIBRARIES})
    set_target_properties(${RTC_NAME}Comp PROPERTIES LINK_FLAGS "${LDFLAGS}")
endif()

install(TARGETS ${RTC_NAME} ${RTC_NAME}Comp
                            LIBRARY DESTINATION lib
                            ARCHIVE DESTINATION lib
                            RUNTIME DESTINATION bin 
        COMPONENT "applications")

install(FILES ${SRC_DIRS}/${RTC_NAME}.h
        DESTINATION include
        COMPONENT sources)


# target settings for document
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
#
#add_custom_target(doxygen COMMAND
#                  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES doc)

#get_target_property(DOC_TARGET doc TYPE)
#if(NOT DOC_TARGET)
#    add_custom_target(doc)
#endif()
#add_dependencies(doc doxygen)

#install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION "${INSTALL_PREFIX}"
#        COMPONENT "documents")


# install source files
#install(FILES ${IMPL_SRCS}
#        DESTINATION "${INSTALL_PREFIX}/src"
#        COMPONENT "sources")

#install(FILES ${LIBS_SRCS} ${LIBS_INCS} ${EXEC_SRCS}
#        DESTINATION include
#        COMPONENT sources)

#install(FILES ${OTHER_SRCS}
#        DESTINATION "${INSTALL_PREFIX}/src"
#        COMPONENT "sources")

#install(DIRECTORY cmake_modules cpack_resources
#        DESTINATION "${INSTALL_PREFIX}/src"
#        COMPONENT "sources")


# package settings
set(CPACK_PACKAGE_NAME "${RTC_NAME}")
set(CPACK_PACKAGE_VERSION "${RTC_VERSION}")
set(CPACK_PACKAGE_VENDOR "${RTC_VENDOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${RTC_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "${RTC_MAINTAINER}")

set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_CURRENT_SOURCE_DIR}/cpack_resources/License.txt")
set(CPACK_PACKAGE_DESCRIPTION_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/cpack_resources/Description.txt")

set(CPACK_COMPONENTS_ALL applications documents sources)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Applications")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "RT-Component applications")
set(CPACK_COMPONENT_APPLICATIONS_DISABLED FALSE)

set(CPACK_COMPONENT_DOCUMENTS_DISPLAY_NAME "Documents")
set(CPACK_COMPONENT_DOCUMENTS_DESCRIPTION "RT-Component documents")
set(CPACK_COMPONENT_DOCUMENTS_DISABLED TRUE)

set(CPACK_COMPONENT_SOURCES_DISPLAY_NAME "Source files")
set(CPACK_COMPONENT_SOURCES_DESCRIPTION "RT-Component source files")
set(CPACK_COMPONENT_SOURCES_DISABLED TRUE)

if(WIN32)
	#    set(CPACK_GENERATOR "WIX")

    # Windows WiX package settings

    #    set(CPACK_WIX_XSL "@CMAKE_CURRENT_BINARY_DIR@/wix.xsl")
    #set(CPACK_WIX_LANG "ja-jp")
    #set(CPACK_RESOURCE_FILE_LICENSE
    #    "@CMAKE_CURRENT_SOURCE_DIR@/cpack_resources/License.rtf")
    #configure_file(
    #    "${CMAKE_CURRENT_SOURCE_DIR}/cpack_resources/wix.xsl.in"
    #    "${CMAKE_CURRENT_BINARY_DIR}/wix.xsl" @ONLY)
    #
    #set(CPACK_PACKAGE_FILE_NAME
    #    "rtc${OPENRTM_VERSION}-${PACKAGE_NAME}${PACKAGE_VERSION}")

else()
    set(CPACK_GENERATOR "${LINUX_PACKAGE_GENERATOR}")

    if(CPACK_GENERATOR STREQUAL "DEB")
        # Linux Debian package settings

        set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-1")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
        set(CPACK_DEBIAN_PACKAGE_SECTION "main")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY  "extra")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "openrtm-aist (>= 1.0.0)")

        set(CPACK_PACKAGE_FILE_NAME
            "rtc${OPENRTM_VERSION}-${PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

    elseif(CPACK_GENERATOR STREQUAL "RPM")
        # Linux RPM package settings

        set(CPACK_RPM_PACKAGE_RELEASE "1.fc10")
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
        set(CPACK_RPM_PACKAGE_LICENSE "LGPL")
        set(CPACK_RPM_PACKAGE_REQUIRES "OpenRTM-aist >= 1.0.0")

        set(CPACK_PACKAGE_FILE_NAME
            "rtc${OPENRTM_VERSION}-${PACKAGE_NAME}-${PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CPACK_RPM_PACKAGE_ARCHITECTURE}")

    endif()
endif()

message(STATUS "Package type: ${CPACK_GENERATOR}")
message(STATUS "Package name: ${CPACK_PACKAGE_FILE_NAME}")

if (SolutionDir)

  IF(NOT EXISTS "${SolutionDir}/bin")
    add_CUSTOM_COMMAND(
      TARGET ${PROJECT_NAME}
      COMMAND mkdir ${SolutionDir}/bin)
  ENDIF(NOT EXISTS "${SolutionDir}/bin")

  if (MSVC)
    add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E 
        copy ${PROJECT_BINARY_DIR}/Release/${PROJECT_NAME}.dll ${SolutionDir}/bin
    )
    add_custom_command(
      TARGET ${PROJECT_NAME}Comp
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E 
        copy ${PROJECT_BINARY_DIR}/Release/${PROJECT_NAME}Comp.exe ${SolutionDir}/bin
    )

  else (MSVC)
    add_custom_command(
      TARGET ${PROJECT_NAME}Comp
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E 
        copy ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Comp ${SolutionDir}/bin
    )
    add_custom_command(
      TARGET ${PROJECT_NAME}Comp
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E 
        copy ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.so ${SolutionDir}/bin
    )

  endif (MSVC)
endif (SolutionDir)

include(CPack)
