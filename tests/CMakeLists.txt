set(CMAKE_VERBOSE_MAKEFILE OFF)

# Output binary directory
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# Configure test header
set(TEST_DATAFILE "${PROJECT_DATA_DIR}/testdata.gif")
configure_file("${PROJECT_CONFIG_DIR}/testconfig.h.in" "${PROJECT_TEST_SRC_DIR}/testconfig.h")

# Define tests
set(TestsToRun
    #array1_test.cpp
    array2_test.cpp
    bispectrum_test.cpp
    #array3_test.cpp
    #array4_test.cpp
    #array5_test.cpp
    range_test.cpp
    correl_test.cpp
    opencv_test.cpp
    smip_test.cpp
)

# Generate main test runner
create_test_sourcelist(Tests smip_tests.cpp ${TestsToRun})

# Add executable target
add_executable(smip_tests ${Tests})
add_dependencies(smip_tests smip)

set_target_properties(smip_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

# Link required libraries
target_link_libraries(smip_tests PRIVATE
    smip
    ${OpenCV_LIBS}
    ${FFTW3_LIBRARIES}
)

# Add tests individually with optional Windows environment overrides
foreach(test ${TestsToRun})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND smip_tests ${TName})
    
    if(WIN32)
        set_tests_properties(${TName} PROPERTIES
            ENVIRONMENT "PATH=$ENV{PATH};${CMAKE_BINARY_DIR}/output/lib;${EXECUTABLE_OUTPUT_PATH};${CMAKE_BINARY_DIR}/tests;C:\\Users\\RUNNER~1\\AppData\\Local\\Temp\\fftw3"
            WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
        )
    endif()
endforeach()

# Collect test source files (for packaging, linting, etc.)
foreach(test ${TestsToRun})
    list(APPEND TESTFILES "${CMAKE_CURRENT_SOURCE_DIR}/${test}")
endforeach()
set(ALL_FILES ${ALL_FILES} ${TESTFILES} PARENT_SCOPE)

# Post-build actions (e.g., DLL copies for Windows)
add_custom_command(TARGET smip_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "smip.dll: $<TARGET_FILE:smip>"
    COMMAND ${CMAKE_COMMAND} -E echo "Output dir: $<TARGET_FILE_DIR:smip_tests>"
)

if(WIN32)
    # Copy smip.dll and FFTW3 to test runtime dir
    add_custom_command(TARGET smip_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:smip>
        $<TARGET_FILE_DIR:smip_tests>
    )
    add_custom_command(TARGET smip_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFTW3_LIBRARIES}"
        "${CMAKE_BINARY_DIR}"
    )
endif()
