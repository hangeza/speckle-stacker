#SET(CMAKE_VERBOSE_MAKEFILE ON)

set(TEST_DATAFILE "${PROJECT_DATA_DIR}/testdata.gif")
configure_file("${PROJECT_CONFIG_DIR}/testconfig.h.in" "${PROJECT_TEST_SRC_DIR}/testconfig.h")

# create the testing file and list of tests
set (TestsToRun
    array1_test.cpp
    array2_test.cpp
    array3_test.cpp
    array4_test.cpp
    range_test.cpp
    correl_test.cpp
    opencv_test.cpp
    smip_test.cpp
)

create_test_sourcelist (Tests smip_tests.cpp ${TestsToRun})

# add the executable
add_executable(smip_tests
    ${Tests}
)

add_dependencies(smip_tests smip)

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
    smip
)

message(STATUS "set BUILD_RPATH to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set_target_properties(smip_tests PROPERTIES
    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)
