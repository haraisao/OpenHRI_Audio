#
#
set(comp_srcs PortAudioInput.cpp)
set(standalone_srcs PortAudioInputComp.cpp)

#############
#  BUILD
#############
RewriteBuildOptions()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${OPENRTM_INCLUDE_DIRS}
)

add_definitions(${OPENRTM_CFLAGS})
link_directories(${OPENRTM_LIBRARY_DIRS})

#############
#  3rdPary Libs
find_package(portaudio REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIR})
set(OPTION_LIBRARIES ${PORTAUDIO_LIBRARIES})

#############
#  RTC
set(PROJECT_RTC_SOURCES ${comp_srcs} ${comp_headers} ${ALL_IDL_SRCS})

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
add_executable(${PROJECT_NAME}Comp ${standalone_srcs} ${PROJECT_RTC_SOURCES})
add_dependencies(${PROJECT_NAME}Comp ALL_IDL_TGT)
target_link_libraries(${PROJECT_NAME}Comp ${OPENRTM_LIBRARIES} ${OPTION_LIBRARIES})

message("---${PROJECT_TYPE}  ${CMAKE_INSTALL_PREFIX}")
#############
# Install
#############
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}Comp
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
    LIBRARY DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
    ARCHIVE DESTINATION ${INSTALL_PREFIX} COMPONENT componenet
 )

if(NOT ${PORTAUDIO_STATIC} )
  install(FILES ${PORTAUDIO_RUNTIME}  DESTINATION ${INSTALL_PREFIX} )
endif()

 #install(FILES ${PROKECT_SOURCE_DIR}/RTC.xml
 #    DESTINATION ${INSTALL_PREFIX} COMPONENT component)
