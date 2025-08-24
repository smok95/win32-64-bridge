cmake_minimum_required(VERSION 3.16)
project(BridgeExample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601)
    add_compile_definitions(UNICODE _UNICODE)
endif()

# Create output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(Calculator32)
add_subdirectory(Bridge32)
add_subdirectory(Bridge64)
add_subdirectory(TestApp64)

# Create a custom target to copy the 32-bit files to a separate directory for testing
add_custom_target(setup_test_environment ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin32
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Calculator32> ${CMAKE_BINARY_DIR}/bin32/
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Bridge32> ${CMAKE_BINARY_DIR}/bin32/
    DEPENDS Calculator32 Bridge32
    COMMENT "Setting up test environment"
)