#SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set(SMIP_COMPILING OFF)
#configure_file("${PROJECT_CONFIG_DIR}/config.h" "${PROJECT_HEADER_DIR}/global.h")

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

set_target_properties(smip_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Add all the ADD_TEST for each test
foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND smip_tests ${TName})
#  set_tests_properties(${TName} PROPERTIES
#    ENVIRONMENT "PATH=$<$<BOOL:${WIN32}>:${OpenCV_DIR}/x64/mingw/bin>;${CMAKE_LIBRARY_OUTPUT_DIRECTORY};${CMAKE_BINARY_DIR}/bin;${CMAKE_BINARY_DIR};$ENV{PATH}"
#  )
    if(WIN32)
        #set_tests_properties(${TName} PROPERTIES ENVIRONMENT
            #"PATH=$ENV{PATH};${OpenCV_DIR}/x64/mingw/bin\\;${FFTW3_LIBRARY_DIR}\\;${CMAKE_LIBRARY_OUTPUT_DIRECTORY}\\;${CMAKE_BINARY_DIR}/bin\\;${CMAKE_BINARY_DIR}"
        #)
        set_tests_properties(${TName} PROPERTIES ENVIRONMENT
            "PATH=$ENV{PATH};C:/OpenCV4.10.0/x64/mingw/bin;;D:/a/speckle-stacker/speckle-stacker/build/output/lib;D:/a/speckle-stacker/speckle-stacker/build/bin;D:/a/speckle-stacker/speckle-stacker/build"
        )
    endif()
endforeach ()

foreach (test ${TestsToRun})
  set(TESTFILES "${TESTFILES};${CMAKE_CURRENT_SOURCE_DIR}/${test}")
endforeach ()
message(STATUS "TESTFILES=${TESTFILES}")

set(ALL_FILES
    "${ALL_FILES}"
    "${TESTFILES}" PARENT_SCOPE
)

TARGET_LINK_LIBRARIES(smip_tests PRIVATE 
    smip
)

message(STATUS "set BUILD_RPATH to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set_target_properties(smip_tests PROPERTIES
    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

add_custom_command(TARGET smip_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "smip.dll: $<TARGET_FILE:smip>"
  COMMAND ${CMAKE_COMMAND} -E echo "Output dir: $<TARGET_FILE_DIR:smip_tests>"
)

if(WIN32)
# Copy the DLL after build to the runtime directory
add_custom_command(TARGET smip_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:smip>
    $<TARGET_FILE_DIR:smip_tests>
)
endif()
