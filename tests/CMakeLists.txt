#SET(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${SMIP_INCLUDE_DIR})

# create the testing file and list of tests
set (TestsToRun
    array1_test.cpp
    array2_test.cpp
    array3_test.cpp
#    correl_test.cpp
    smip_test.cpp
)

create_test_sourcelist (Tests smip_tests.cpp ${TestsToRun})

# add the executable
add_executable (smip_tests ${Tests})

# Add all the ADD_TEST for each test
foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND smip_tests ${TName}
  )
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
