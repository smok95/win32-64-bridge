add_library(Bridge64 SHARED
    Bridge64.cpp
    IPCClient.cpp
    IPCClient.h
    ../Common/IPCProtocol.h
)

# Include directories
target_include_directories(Bridge64 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
)

target_compile_definitions(Bridge64 PRIVATE BRIDGE64_EXPORTS)

# This is the 64-bit bridge DLL
set_target_properties(Bridge64 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)