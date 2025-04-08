cmake_minimum_required(VERSION 3.5)

PROJECT(SMIP LANGUAGES CXX C)
string(TIMESTAMP PROJECT_DATE_STRING "%b %d, %Y")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wshadow -Wpedantic -O3 -ffast-math)
include(GNUInstallDirs)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
ENABLE_TESTING()
#INCLUDE(CTest)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(SMIP_IS_RELEASE ON)
else()
    set(SMIP_IS_RELEASE OFF)
endif()

set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(PROJECT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
#set(PROJECT_DETAIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/detail")
set(PROJECT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/config")
set(PROJECT_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/lib")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/packages/")
include_directories(${PROJECT_HEADER_DIR} ${PROJECT_SRC_DIR})

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
#get_filename_component(OpenCV_BIN_PATH "${OpenCV_LIB_PATH}/../bin" ABSOLUTE)
      #file(TO_NATIVE_PATH "${OpenCV_BIN_PATH}" OpenCV_BIN_PATH)
message(STATUS "OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_DIR=${OpenCV_DIR}")
message(STATUS "OpenCV_LIB_PATH=${OpenCV_LIB_PATH}")
message(STATUS "_OpenCV_LIB_PATH=${_OpenCV_LIB_PATH}")
message(STATUS "OpenCV_LIBRARIES=${OpenCV_LIBRARIES}")
message(STATUS "OpenCV_LIBRARY_DIR=${OpenCV_LIBRARY_DIR}")

# Add support for FFTW3 on Windows
if(WIN32)
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
        # Optionally, you can specify the exact library (e.g., if it's located in the lib folder)
        SET(NEW_LIB "$ENV{FFTW3_LIBRARIES}")
        string(REPLACE "\\" "/" NEW_LIB "${NEW_LIB}")
        set(FFTW3_LIBRARIES ${NEW_LIB})  # Adjust the library name if necessary (check for the correct file name)
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

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cmake")
configure_file("${PROJECT_CONFIG_DIR}/config.h" "${PROJECT_HEADER_DIR}/global.h")

SET(SMIP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILES
    "${PROJECT_SRC_DIR}/main.cpp"
    "${PROJECT_SRC_DIR}/videoio.cpp"
    "${PROJECT_SRC_DIR}/phasemap.cpp"
    "${PROJECT_SRC_DIR}/phasereco.cpp"
    "${PROJECT_SRC_DIR}/log.cpp"
    "${PROJECT_SRC_DIR}/utility.cpp"
)

set(HEADER_FILES
    "${PROJECT_HEADER_DIR}/array_base.h"
    "${PROJECT_HEADER_DIR}/array2.h"
    "${PROJECT_HEADER_DIR}/bispectrum.h"
    "${PROJECT_HEADER_DIR}/videoio.h"
    "${PROJECT_HEADER_DIR}/phasemap.h"
    "${PROJECT_HEADER_DIR}/phasereco.h"
    "${PROJECT_HEADER_DIR}/window_function.h"
    "${PROJECT_HEADER_DIR}/log.h"
    "${PROJECT_HEADER_DIR}/point.h"
    "${PROJECT_HEADER_DIR}/rect.h"
    "${PROJECT_HEADER_DIR}/dimvector.h"
    "${PROJECT_HEADER_DIR}/crosscorrel.h"
    "${PROJECT_HEADER_DIR}/types.h"
    "${PROJECT_HEADER_DIR}/utility.h"
    "${PROJECT_HEADER_DIR}/math_functions.h"
    "${PROJECT_HEADER_DIR}/units.h"
    "${PROJECT_HEADER_DIR}/constants.h"
    "${PROJECT_HEADER_DIR}/range.h"
)

# tell cmake to build our executable
ADD_EXECUTABLE(smip
    ${SOURCE_FILES}
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

add_subdirectory(tests)

add_custom_target(clangformat COMMAND clang-format -style=WebKit -i ${ALL_FILES})
