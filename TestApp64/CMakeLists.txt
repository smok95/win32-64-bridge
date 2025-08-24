add_executable(TestApp64
    main.cpp
)

# Include Calculator32 headers for interface
target_include_directories(TestApp64 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Calculator32
)

# Link to Bridge64
target_link_libraries(TestApp64 PRIVATE Bridge64)

# This is a 64-bit test application
set_target_properties(TestApp64 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy Bridge64.dll to test directory
add_custom_command(TARGET TestApp64 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bridge64> $<TARGET_FILE_DIR:TestApp64>
    COMMENT "Copying Bridge64.dll to test directory"
)