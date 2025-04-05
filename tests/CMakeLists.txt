#SET(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${SMIP_INCLUDE_DIR})

set(TEST_DATAFILE "${PROJECT_DATA_DIR}/testdata.gif")
configure_file("${PROJECT_CONFIG_DIR}/testconfig.h.in" "${PROJECT_TEST_SRC_DIR}/testconfig.h")

# create the testing file and list of tests
set (TestsToRun
    array1_test.cpp
    array2_test.cpp
    array3_test.cpp
    correl_test.cpp
    opencv_test.cpp
    smip_test.cpp
)

create_test_sourcelist (Tests smip_tests.cpp ${TestsToRun})

set(EXT_SOURCE_FILES
    "${PROJECT_SRC_DIR}/videoio.cpp"
    "${PROJECT_SRC_DIR}/phasemap.cpp"
    "${PROJECT_SRC_DIR}/phasereco.cpp"
    "${PROJECT_SRC_DIR}/log.cpp"
    "${PROJECT_SRC_DIR}/utility.cpp"
)

# add the executable
add_executable (smip_tests ${Tests} ${EXT_SOURCE_FILES})
#C:\OpenCV4.10.0\x64\mingw\bin
# Add all the ADD_TEST for each test
foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND smip_tests ${TName})
  set_tests_properties(${TName} PROPERTIES ENVIRONMENT_MODIFICATION
                     "PATH=path_list_prepend:$<$<BOOL:${WIN32}>:${OpenCV_DIR}/x64/mingw/bin>")
endforeach ()

foreach (test ${TestsToRun})
  set(TESTFILES "${TESTFILES};${CMAKE_CURRENT_SOURCE_DIR}/${test}")
endforeach ()
message(STATUS "TESTFILES=${TESTFILES}")

set(ALL_FILES
    "${ALL_FILES}"
    "${TESTFILES}" PARENT_SCOPE
)

TARGET_LINK_LIBRARIES(smip_tests
    pthread
    ${OpenCV_LIBS}
    ${FFTW3_LIBRARIES}
)

