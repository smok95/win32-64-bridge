add_library(Calculator32 SHARED
    Calculator32.cpp
    Calculator32.h
)

target_compile_definitions(Calculator32 PRIVATE CALCULATOR32_EXPORTS)

# Force 32-bit compilation
set_target_properties(Calculator32 PROPERTIES
    WIN32_EXECUTABLE FALSE
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib32
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib32
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin32
)

# For 32-bit compilation on 64-bit systems
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(WARNING "Building Calculator32 as 64-bit library. For true 32-bit, use a 32-bit compiler.")
endif()