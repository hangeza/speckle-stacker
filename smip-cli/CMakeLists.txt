SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(APP_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# tell cmake to build our executable
ADD_EXECUTABLE(smip-cli
    ${APP_SOURCE_FILES}
)

add_dependencies(smip-cli smip)

TARGET_LINK_LIBRARIES(smip-cli PRIVATE 
    smip
    #${PROJECT_INCLUDE_LIBS}
)

# Set the build RPATH to include the directory where the shared library is built
set_target_properties(smip-cli PROPERTIES
    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

# Copy the DLL after build to the runtime directory
add_custom_command(TARGET smip-cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:smip>"  # Full path to libsmip.dll
    "$<TARGET_FILE_DIR:smip-cli>"  # Output dir of smip_tests.exe
)
