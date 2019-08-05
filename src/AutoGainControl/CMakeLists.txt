#
#  CMake for AutoGainControl Component
#

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(AutoGainControl)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

find_package(Portaudio REQUIRED)
find_package(OpenRTM REQUIRED)

if (WIN32)
  list(APPEND LIBRARIES ${PORTAUDIO_LIBRARIES} "winmm.lib")
else (WIN32)
#  list(APPEND LIBRARIES ${PORTAUDIO_LIBRARIES})
endif(WIN32)

ADD_DEFINITIONS(-DPACKAGE="${PACKAGE}" -DLOCALEDIR="${LOCALEDIR}")

# component settings
set(RTC_NAME "${PROJECT_NAME}")
set(RTC_VERSION "1.0.0")
set(RTC_LANG "c++")
set(RTC_DESCRIPTION "Automatic gain control Module by using PortAudio Library")
set(RTC_VENDOR "AIST")
set(RTC_MAINTAINER "isao-hara@aist.go.jp")

string(TOLOWER ${RTC_NAME} PACKAGE_NAME)
set(PACKAGE_VERSION "${RTC_VERSION_MAJOR}.${RTC_VERSION_MINOR}.${RTC_VERSION_PATCH}")

set(LIBS_SRCS ${PROJECT_NAME}.cpp)
set(LIBS_INCS ${PROJECT_NAME}.h)
set(EXEC_SRCS ${PROJECT_NAME}Comp.cpp)

set(OTHER_SRCS CMakeLists.txt
               ${PROJECT_NAME}.conf rtc.conf)

include(RTC)
