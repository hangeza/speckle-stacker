cmake_minimum_required(VERSION 2.6)

PROJECT(SMIP)

include(GNUInstallDirs)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

#The following lines are required to use Dart
#ENABLE_TESTING()
#INCLUDE(Dart)

SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS(-std=gnu++2a)
ADD_DEFINITIONS(-O3)
ADD_DEFINITIONS(-ffast-math)
ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-flto)
# uncomment to use lld linker when compiling with clang
#ADD_LINK_OPTIONS(-fuse-ld=lld)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
include_directories(PkgConfig::FFTW)
link_libraries(PkgConfig::FFTW)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/videoio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/phasemap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/phasereco.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility.cpp"
)

set(HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/array_base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/array2.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/bispectrum.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/multidimarray.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/videoio.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/phasemap.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/phasereco.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/window_function.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/point.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/rect.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/dimvector.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/crosscorrel.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/types.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility.h"
)

# tell cmake to build our executable
ADD_EXECUTABLE(smip
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

set(ALL_FILES
    "${SOURCE_FILES}"
    "${HEADER_FILES}"
)

TARGET_LINK_LIBRARIES(smip
 pthread
 ${OpenCV_LIBS}
)

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i ${ALL_FILES})
