# Add paths for GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

target_link_libraries(BambooEngine GLEW::GLEW)

if (WIN32)
    add_custom_command(TARGET BambooEngine POST_BUILD        # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
            "${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll"      # <--this is in-file
            $<TARGET_FILE_DIR:BambooEngine>)                 # <--this is out-file path
endif()
