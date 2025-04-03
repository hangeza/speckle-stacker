cmake_minimum_required(VERSION 3.5)

PROJECT(SMIP)

include(GNUInstallDirs)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

#The following lines are required to use Dart
ENABLE_TESTING()
#INCLUDE(CTest)

#SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS(-std=gnu++2a)
ADD_DEFINITIONS(-O3)
ADD_DEFINITIONS(-ffast-math)
ADD_DEFINITIONS(-Wall -Wextra -pedantic)

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
    include(ExternalProject)

    MESSAGE(STATUS "Trying to install fftw...")

    ExternalProject_Add(project_fftw
        #GIT_REPOSITORY  https://github.com/FFTW/fftw3
        URL http://www.fftw.org/fftw-3.3.10.tar.gz
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fftw
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/fftw/install -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/fftw/install
    )

    add_library(fftw STATIC IMPORTED)

    # Copy the DLL to the Windows system32 directory after the library is built
    MESSAGE(STATUS "copy library to system dir")
    add_custom_command(TARGET fftw POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/fftw/install/bin/libfftw3.dll"
        "C:/Windows/System32/"
        COMMENT "Copying libfftw3.dll to system folder"
    )
    
    set(lib_fftw_name
    ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3.dll${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(fftw PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_BINARY_DIR}/fftw/install/lib/${lib_fftw_name})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/fftw/install/include)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/fftw/install/lib)
    #link_libraries(${fftw})
else()
    ADD_DEFINITIONS(-flto)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
    include_directories(PkgConfig::FFTW)
    link_libraries(PkgConfig::FFTW)
endif()


SET(SMIP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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
    "${CMAKE_CURRENT_SOURCE_DIR}/math_functions.h"
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

if(WIN32)
    TARGET_LINK_LIBRARIES(smip
        ${OpenCV_LIBS}
        ${lib_fftw_name}
    )
    add_dependencies(smip project_fftw)

else()
    TARGET_LINK_LIBRARIES(smip
        pthread
        ${OpenCV_LIBS}
    )
endif()

add_subdirectory(tests)

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i ${ALL_FILES})
