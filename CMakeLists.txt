cmake_minimum_required(VERSION 3.1)

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

MESSAGE("Compiler is " ${CMAKE_CXX_COMPILER_ID})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using Clang
  ADD_LINK_OPTIONS(-fuse-ld=lld)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  #MESSAGE("GNU")
  # using GCC
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # using Visual Studio C++
endif()
# uncomment to use lld linker when compiling with clang
#ADD_LINK_OPTIONS(-fuse-ld=lld)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Add support for FFTW3 on Windows
if(WIN32)
    SET(CMAKE_AR  "gcc-ar")
    # Look for the environment variables we set in the GitHub Actions workflow
    if(DEFINED ENV{FFTW3_INCLUDE_DIR} AND DEFINED ENV{FFTW3_LIBRARY_DIR})
        message(STATUS "Found FFTW3 in environment")
        
        # Include the FFTW3 headers
        include_directories("$ENV{FFTW3_INCLUDE_DIR}")
        message(STATUS "FFTW3_INCLUDE_DIR=$ENV{FFTW3_INCLUDE_DIR}")
        message(STATUS "FFTW3_LIBRARY_DIR=$ENV{FFTW3_LIBRARY_DIR}")
        message(STATUS "FFTW3_LIBRARIES=$ENV{FFTW3_LIBRARIES}")

        SET(NEW_LIB_PATH "$ENV{FFTW3_LIBRARY_DIR}")
        string(REPLACE "\\" "/" NEW_LIB_PATH "${NEW_LIB_PATH}")
        # Link the FFTW3 libraries
        link_directories(${NEW_LIB_PATH})
#        link_directories("$ENV{FFTW3_LIBRARY_DIR}")
        # Optionally, you can specify the exact library (e.g., if it's located in the lib folder)
        
        SET(NEW_LIB "$ENV{FFTW3_LIBRARIES}")
        string(REPLACE "\\" "/" NEW_LIB "${NEW_LIB}")
        set(FFTW3_LIBRARIES ${NEW_LIB})  # Adjust the library name if necessary (check for the correct 
        #set(FFTW3_LIBRARIES "$ENV{FFTW3_LIBRARIES}")  # Adjust the library name if necessary (check for the correct file name)
    else()
        message(WARNING "FFTW3 not found in environment, skipping FFTW3 support")
    endif()
else()
    ADD_DEFINITIONS(-flto)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
    include_directories(PkgConfig::FFTW)
    link_libraries(PkgConfig::FFTW)
endif()


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
 ${FFTW3_LIBRARIES}
)
#if(FFTW3_LIBRARIES)
#    target_link_libraries(SpeckleStacker PRIVATE ${FFTW3_LIBRARIES})
#endif()

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i ${ALL_FILES})
