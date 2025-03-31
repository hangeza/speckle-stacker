#SET(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${SMIP_INCLUDE_DIR})

set(TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/smip_test.cpp"
)

message(STATUS "SMIP_INCLUDE_DIR=${SMIP_INCLUDE_DIR}")

# tell cmake to build our executable
ADD_EXECUTABLE(smip_test
    ${TEST_SOURCES}
)

set(ALL_FILES
    "${ALL_FILES}"
    "${TEST_SOURCES}" PARENT_SCOPE
)

TARGET_LINK_LIBRARIES(smip_test
    pthread
    ${OpenCV_LIBS}
    ${FFTW3_LIBRARIES}
)

add_test(NAME MainTest COMMAND smip_test)
