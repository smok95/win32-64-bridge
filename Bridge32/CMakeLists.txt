add_executable(Bridge32
    main.cpp
    IPCServer.cpp
    IPCServer.h
    ../Common/IPCProtocol.h
)

# Include directories
target_include_directories(Bridge32 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../Calculator32
)

# Link Calculator32 for dependency
target_link_libraries(Bridge32 PRIVATE Calculator32)

# Force 32-bit if possible
set_target_properties(Bridge32 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin32
)

# Copy Calculator32.dll to the same directory as Bridge32.exe
add_custom_command(TARGET Bridge32 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Calculator32> $<TARGET_FILE_DIR:Bridge32>
    COMMENT "Copying Calculator32.dll to Bridge32 directory"
)