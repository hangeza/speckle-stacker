
set(APP_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# tell cmake to build our executable
ADD_EXECUTABLE(smip-cli
    ${APP_SOURCE_FILES}
)

add_dependencies(smip-cli smip)

TARGET_LINK_LIBRARIES(smip-cli
    smip
    #${PROJECT_INCLUDE_LIBS}
)

# Set the build RPATH to include the directory where the shared library is built
set_target_properties(smip-cli PROPERTIES
    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)
