if (NOT WIN32)
    # Add paths for GL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    set(OPENGL_LINK_LIBRARIES ${OPENGL_LIBRARIES})
else()
    set(OPENGL_LINK_LIBRARIES "opengl32")
endif()

target_link_libraries(BambooEngine ${OPENGL_LINK_LIBRARIES})